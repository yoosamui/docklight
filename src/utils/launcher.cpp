#include "launcher.h"
#include <glibmm/i18n.h>
#include "common.h"
#include "gio/gdesktopappinfo.h"
#include "utils/moreader.h"
#include "utils/string.h"

DL_NS_BEGIN

namespace launcher_util
{
    map<std::string, std::string> dictionary;

    bool get_app_info(WnckWindow* window, appinfo_t& info)
    {
        const char* _group = wnck_window_get_class_group_name(window);
        if (_group == nullptr) {
            WnckClassGroup* wcg = wnck_window_get_class_group(window);
            if (wcg == nullptr) {
                g_warning("App %s class group could not be found.\n",
                          wnck_window_get_class_instance_name(window));
                return false;
            }

            _group = wnck_class_group_get_name(wcg);
            if (_group == nullptr) {
                g_warning("App %s class group could not be found.\n",
                          wnck_window_get_class_instance_name(window));
                return false;
            }
        }

        const std::string extensions[] = {".py", ".exe", ".sh"};
        // Checks whether or not the window has a name. wnck_window_get_name()
        // will always return some value, even if window has no name set;

        info.m_group = _group;
        info.m_group = string_util::remove_extension(info.m_group, extensions);
        info.m_name = string_util::string_to_lower(info.m_group.c_str());

        size_t idx = info.m_name.find(" ");
        if (idx != string::npos) {
            info.m_name = info.m_name.substr(0, idx);
        }

        const char* _instance = wnck_window_get_class_instance_name(window);
        if (_instance == nullptr) {
            _instance = wnck_window_get_name(window);
            g_warning("App %s instance could not be found. Use name instead\n",
                      _instance);
        }

        info.m_instance = _instance;
        info.m_instance =
            string_util::remove_extension(info.m_instance, extensions);

        info.m_title = wnck_window_get_name(window);

        GKeyFile* key_file = g_key_file_new();
        info.m_error = get_desktop_file(key_file, info.m_group.c_str(),
                                        info.m_desktop_file) == false;

        if (!info.m_error) {
            GError* error = nullptr;
            info.m_desktop_icon_name = g_key_file_get_string(
                key_file, "Desktop Entry", "Icon", &error);
            if (error) {
                g_error_free(error);
                error = nullptr;
            }
        } else {
            // Icon name could not be found.
            // We build an icon name from the app name without '-' char
            info.m_desktop_icon_name = info.m_name;
            size_t idx = info.m_name.find("-");
            if (idx != string::npos) {
                info.m_desktop_icon_name =
                    info.m_desktop_icon_name.substr(idx + 1);
            }
        }

        g_key_file_free(key_file);

        info.m_desktop_name = get_name_from_desktopfile(info.m_group.c_str());

        info.m_wnckwindow = window;
        info.m_xid = wnck_window_get_xid(window);

        return info.m_error;
    }

    string get_desktop_file(const char* appname)
    {
        GKeyFile* key_file = g_key_file_new();
        string result = "";
        get_desktop_file(key_file, appname, result);
        g_key_file_free(key_file);

        return result;
    }

    bool get_desktop_file(GKeyFile* key_file, const char* appname,
                          string& desktop_file)
    {
        if (appname == nullptr) return false;
        if (strcmp(appname, "Untitled window") == 0) return false;

        string name = appname;
        string lowername = string_util::string_to_lower(appname);

        // clang-format off
        const string desktopfilename[] =
            {{appname},
            {lowername},
            {"org.gnome." + name},
            {"org.gnome." + lowername}};

        const string filelocaltion[] = {
            {"/usr/share/applications/"},
            {"/usr/local/share/applications/"}};
        // clang-format on

        GError* error = nullptr;
        char filepath[PATH_MAX];
        for (auto p : filelocaltion) {
            for (auto f : desktopfilename) {
                sprintf(filepath, "%s%s.desktop", p.c_str(), f.c_str());

                gboolean found = g_key_file_load_from_file(
                    key_file, filepath, GKeyFileFlags::G_KEY_FILE_NONE, &error);

                if (error) {
                    g_error_free(error);
                    error = nullptr;
                    continue;
                }

                if (found) {
                    desktop_file = filepath;
                    return true;
                }
            }
        }

        return false;
    }

    string get_name_from_desktopfile(const char* appname)
    {
        string theappname = appname;
        replace(theappname.begin(), theappname.end(), ' ', '-');

        if (dictionary.count(theappname) == 1) {
            string value = dictionary[theappname];
            if (value.empty()) return theappname;

            return gettext(value.c_str());
        }

        GError* error = nullptr;
        GKeyFile* key_file = g_key_file_new();
        const char* _appname = theappname.c_str();
        string desktop_file;
        if (!get_desktop_file(key_file, _appname, desktop_file)) {
            dictionary[theappname] = theappname;
            g_key_file_free(key_file);
            return theappname;
        }

        char* elang = getenv("LANG");
        if (elang == nullptr) {
            dictionary[theappname] = theappname;
            g_key_file_free(key_file);
            return theappname;
        }

        string lang(elang);
        lang = lang.substr(0, 2);
        if (lang == "en") {
            char* titlename = g_key_file_get_string(key_file, "Desktop Entry",
                                                    "Name", &error);
            if (titlename != nullptr) {
                dictionary[theappname] = titlename;
                g_key_file_free(key_file);
                return titlename;
            }

            if (error) {
                g_error_free(error);
                error = nullptr;
            }

            dictionary[theappname] = theappname;
            g_key_file_free(key_file);
            return theappname;
        }

        // check if the GenericName Entry exists
        string langNameKey = "GenericName[" + lang + "]";
        char* titlename = g_key_file_get_string(key_file, "Desktop Entry",
                                                langNameKey.c_str(), &error);
        if (titlename != nullptr) {
            dictionary[theappname] = titlename;
            g_key_file_free(key_file);
            return titlename;
        }

        if (error) {
            g_error_free(error);
            error = nullptr;
        }

        // check if the Name Entry exists
        langNameKey = "Name[" + lang + "]";
        titlename = g_key_file_get_string(key_file, "Desktop Entry",
                                          langNameKey.c_str(), &error);
        if (titlename != nullptr) {
            dictionary[theappname] = titlename;
            g_key_file_free(key_file);
            return titlename;
        }

        if (error) {
            g_error_free(error);
            error = nullptr;
        }

        // check if the dektop file is a Domain file
        langNameKey = "X-Ubuntu-Gettext-Domain";
        char* mofile = g_key_file_get_string(key_file, "Desktop Entry",
                                             langNameKey.c_str(), &error);
        if (mofile == nullptr) {
            if (error) {
                g_error_free(error);
                error = nullptr;
            }

            dictionary[theappname] = theappname;
            g_key_file_free(key_file);
            return theappname;
        }

        langNameKey = "X-GNOME-FullName";
        char* xFullname = g_key_file_get_string(key_file, "Desktop Entry",
                                                langNameKey.c_str(), &error);
        if (xFullname == nullptr) {
            if (error) {
                g_error_free(error);
                error = nullptr;
            }
        }

        langNameKey = "GenericName";
        char* xGenericName = g_key_file_get_string(key_file, "Desktop Entry",
                                                   langNameKey.c_str(), &error);
        if (xGenericName == nullptr) {
            if (error) {
                g_error_free(error);
                error = nullptr;
            }
        }

        langNameKey = "Name";
        char* xName = g_key_file_get_string(key_file, "Desktop Entry",
                                            langNameKey.c_str(), &error);
        if (xName == nullptr) {
            if (error) {
                g_error_free(error);
                error = nullptr;
            }
        }

        MoReader* mo = new MoReader();
        string moFilePath("/usr/share/locale-langpack/" + lang +
                          "/LC_MESSAGES/" + mofile + ".mo");

        if (mo->Load(moFilePath.c_str()) < 0) {
            dictionary[theappname] = theappname;
            g_key_file_free(key_file);

            delete mo;
            return theappname;
        }

        string xFullnameTranslatedText = "";
        string xGenericNameTranslatedText = "";
        string xNameTranslatedText = "";

        if (xFullname != nullptr)
            xFullnameTranslatedText = mo->getText(xFullname);

        if (xGenericName != nullptr)
            xGenericNameTranslatedText = mo->getText(xGenericName);

        if (xName != nullptr) xNameTranslatedText = mo->getText(xName);

        if (!xFullnameTranslatedText.empty() &&
            xFullnameTranslatedText != theappname) {
            dictionary[theappname] = xFullnameTranslatedText;
            g_key_file_free(key_file);
            delete mo;
            return dictionary[theappname];
        }

        if (!xGenericNameTranslatedText.empty() &&
            xGenericNameTranslatedText != theappname) {
            dictionary[theappname] = xGenericNameTranslatedText;
            g_key_file_free(key_file);
            delete mo;
            return dictionary[theappname];
        }

        if (!xNameTranslatedText.empty() && xNameTranslatedText != theappname) {
            dictionary[theappname] = xNameTranslatedText;
            g_key_file_free(key_file);
            delete mo;
            return dictionary[theappname];
        }

        dictionary[theappname] = theappname;
        g_key_file_free(key_file);
        delete mo;

        return dictionary[theappname];
    }

    bool launch(const string& appname, const string& desktop_filename)
    {
        GError* error = nullptr;
        GAppLaunchContext* context = nullptr;
        GAppInfo* app_info = nullptr;
        GKeyFile* key_file = g_key_file_new();

        string desktopfile(desktop_filename);

        // if no desktop file provide try to find it by app name
        if (desktopfile.empty()) {
            desktopfile = get_desktop_file(appname.c_str());
        }

        if (!desktopfile.empty()) {
            gboolean found = g_key_file_load_from_file(
                key_file, desktopfile.c_str(), GKeyFileFlags::G_KEY_FILE_NONE,
                &error);

            if (error) {
                g_error_free(error);
                error = nullptr;
            }

            if (found) {
                app_info =
                    (GAppInfo*)g_desktop_app_info_new_from_keyfile(key_file);

                if (app_info != nullptr) {
                    char* uri = nullptr;
                    GFile* file = nullptr;
                    GList* glist_parameters = nullptr;

                    // TODO: check parameters

                    GdkDisplay* display = gdk_display_get_default();
                    context =
                        (GAppLaunchContext*)gdk_display_get_app_launch_context(
                            display);

                    gboolean launched = g_app_info_launch_uris(
                        app_info, glist_parameters, context, &error);

                    if (error) {
                        g_warning("Launcher: Error %s %s \n", appname.c_str(),
                                  error->message);
                        g_error_free(error);
                        error = nullptr;
                    }

                    g_object_unref(app_info);
                    g_object_unref(context);

                    if (uri != nullptr) g_free(uri);

                    if (file != nullptr) g_object_unref(file);

                    if (launched) {
                        g_key_file_free(key_file);
                        g_print("Launch via Desktop file %s\n",
                                appname.c_str());
                        return launched;
                    }
                }
            }
        }

        // desktop file could not be found
        // launch via command line
        gboolean launched = g_spawn_command_line_async(appname.c_str(), &error);

        if (error) {
            g_warning("Lauch via command line: Error (%s) %s \n",
                      appname.c_str(), error->message);
            g_error_free(error);
            error = nullptr;
            g_key_file_free(key_file);
            return false;
        }

        g_print("Launch via command line: %s\n", appname.c_str());
        g_key_file_free(key_file);
        return launched;
    }
}  // namespace launcher_util

DL_NS_END
