#include "desktopfile.h"
#include <algorithm>
#include <sstream>
#include "gio/gdesktopappinfo.h"
#include "utils/moreader.h"
#include "utils/string.h"

DL_NS_BEGIN

namespace desktopfile_util
{
    // clang-format off
        const string desktop_directories[] = {
            {"/usr/share/applications/"},
            {"/usr/local/share/applications/"}};
    // clang-format on

    void set(GKeyFile* key_file, const string& desktop_file, appinfo_t& info)
    {
        GError* error = NULL;
        // error = NULL;
        info.m_desktop_file = desktop_file;

        char* icon = g_key_file_get_string(key_file, "Desktop Entry", "Icon", &error);
        if (error) {
            g_error_free(error);
            error = nullptr;
        }
        if (!icon) {
        }

        char* name = g_key_file_get_string(key_file, "Desktop Entry", "Name", &error);
        if (error) {
            g_error_free(error);
            error = nullptr;
        }
        if (!name) {
            // if (error) {
            // g_error_free(error);
            // error = nullptr;
            //}
        }
        info.m_desktop_icon_name = icon;
        info.m_title = name;

        info.m_lang = getenv("LANG");  // en_US.UTF-8
        info.m_lang = info.m_lang.substr(0, 2);

        string lang_name_key = "Name[" + info.m_lang + "]";
        name = g_key_file_get_string(key_file, "Desktop Entry", lang_name_key.c_str(), &error);
        if (name) {
            info.m_title = name;
        }
        if (error) {
            g_error_free(error);
            error = nullptr;
        }
        if (!name) {
            lang_name_key = "GenericName[" + info.m_lang + "]";
            name = g_key_file_get_string(key_file, "Desktop Entry", lang_name_key.c_str(), &error);
            if (error) {
                g_error_free(error);
                error = nullptr;
            }
            if (name) {
                info.m_title = name;
            }

            // if (error) {
            // g_error_free(error);
            // error = nullptr;
            //}
        }

        lang_name_key = "Comment[" + info.m_lang + "]";
        char* comment =
            g_key_file_get_string(key_file, "Desktop Entry", lang_name_key.c_str(), &error);
        if (comment) {
            info.m_comment = comment;
        }

        if (error) {
            g_error_free(error);
            error = nullptr;
        }
        if (!comment) {
            lang_name_key = "Comment";
            comment =
                g_key_file_get_string(key_file, "Desktop Entry", lang_name_key.c_str(), &error);
            if (error) {
                g_error_free(error);
                error = nullptr;
            }
            if (comment) {
                //  info.m_comment = comment;
            }

            // if (error) {
            // g_error_free(error);
            // error = nullptr;
            //}
        }

        g_print("name...%s\n", info.m_name.c_str());
        g_print("desktop_file..%s\n", info.m_desktop_file.c_str());
        g_print("icon..%s\n", info.m_desktop_icon_name.c_str());
        g_print("title..%s\n", info.m_title.c_str());
        g_print("comment..%s\n", info.m_comment.c_str());
        g_print("lang..%s\n", info.m_lang.c_str());
    }

    GKeyFile* get_key_file(const string& app_name, const string& desktop_file)
    {
        GKeyFile* key_file = nullptr;
        GError* error = nullptr;

        key_file = g_key_file_new();
        auto const found = g_key_file_load_from_file(key_file, desktop_file.c_str(),
                                                     GKeyFileFlags::G_KEY_FILE_NONE, &error);

        if (error) {
            g_debug("desktopfile:get_key_file: ERROR: %s", error->message);
            g_error_free(error);
            error = nullptr;
        }

        if (!found || error) {
            if (key_file) {
                g_key_file_free(key_file);
            }

            return nullptr;
        }

        // validate
        // avoid NoDisplay if exits should be false if and erro rmeant it not exits and this file is
        // valid
        auto nodisplay = g_key_file_get_boolean(key_file, "Desktop Entry", "NoDisplay", &error);
        if (error) {
            g_error_free(error);
            error = nullptr;
        }
        if (nodisplay) {
            if (key_file) {
                g_key_file_free(key_file);
            }
            return nullptr;
        }

        char* exec = g_key_file_get_string(key_file, "Desktop Entry", "Exec", &error);
        if (!exec || error) {
            if (error) {
                g_error_free(error);
                error = nullptr;
            }
            if (key_file) {
                g_key_file_free(key_file);
            }
            return nullptr;
        }

        return key_file;
    }

    GKeyFile* scann_files(const string& app_name, const string& assumtion_name,
                          string& desktop_file)
    {
        if (app_name.empty() || assumtion_name.empty()) return nullptr;

        GKeyFile* key_file = nullptr;
        const string delimeters("-.");
        string assumtion_name_desktop(assumtion_name + ".desktop");
        DIR* dirFile = nullptr;
        desktop_file = "";

        for (auto directory : desktop_directories) {
            dirFile = opendir(directory.c_str());
            if (dirFile == 0) {
                continue;
            }

            struct dirent* hFile;
            while ((hFile = readdir(dirFile)) != nullptr) {
                if (!strcmp(hFile->d_name, ".")) continue;
                if (!strcmp(hFile->d_name, "..")) continue;
                if ((hFile->d_name[0] == '.')) continue;  //  hidden files
                if (!strstr(hFile->d_name, ".desktop")) continue;

                size_t len1 = strlen(hFile->d_name);
                size_t len2 = string(assumtion_name_desktop).size();
                if (len1 < len2) {
                    continue;
                }

                string file_name_lower = string_util::string_to_lower(hFile->d_name);

                if (file_name_lower == assumtion_name_desktop) {
                    desktop_file = directory + hFile->d_name;
                    key_file = get_key_file(app_name, desktop_file);

                    if (key_file == nullptr) {
                        desktop_file = "";
                        continue;
                    };

                    closedir(dirFile);
                    return key_file;
                }

                if (string_util::split_token_exist(file_name_lower, assumtion_name, delimeters)) {
                    desktop_file = directory + hFile->d_name;
                    key_file = get_key_file(app_name, desktop_file);

                    if (key_file == nullptr) {
                        desktop_file = "";
                        continue;
                    };

                    closedir(dirFile);
                    return key_file;
                }
            }
            closedir(dirFile);
        }

        if (key_file) g_key_file_free(key_file);
        return nullptr;
    }

    bool scanning(appinfo_t& info)
    {
        // string s = "org.gnome.gedit.desktop";
        // const string delimeter = "-.";
        // if (string_util::split_token_exist(s, "gedit", delimeter)) {
        // g_print("FOUND\n");
        //}

        //// g_print("%d\n", split(s, '.').size());

        // return false;

        if (info.m_name.empty() || strcmp(info.m_name.c_str(), "Untitled window") == 0) return {};

        GKeyFile* key_file = nullptr;
        //  GError* error = nullptr;

        // heuristic app names
        string app_name_lower = info.m_name;
        std::transform(app_name_lower.begin(), app_name_lower.end(), app_name_lower.begin(),
                       ::tolower);

        string app_name_concat = "";
        string app_name_stroke = "";
        string app_name_dot = "";

        if (app_name_lower.find(' ') != string::npos) {
            app_name_concat = app_name_lower;
            app_name_concat.erase(std::remove(app_name_concat.begin(), app_name_concat.end(), ' '),
                                  app_name_concat.end());

            app_name_stroke = app_name_lower;
            replace(app_name_stroke.begin(), app_name_stroke.end(), ' ', '-');

            app_name_dot = app_name_lower;
            replace(app_name_dot.begin(), app_name_dot.end(), ' ', '.');
        }

        if (strstr(app_name_lower.c_str(), "-")) {
            app_name_dot = app_name_lower;
            replace(app_name_dot.begin(), app_name_dot.end(), ' ', '.');
        }

        string title_lower = string_util::string_to_lower(info.m_title.c_str());
        string title_concat = "";
        string title_stroke = "";
        string title_dot = "";

        if (!title_lower.empty() && title_lower.find(' ') != string::npos) {
            title_concat = title_lower;
            title_concat.erase(std::remove(title_concat.begin(), title_concat.end(), ' '),
                               title_concat.end());
            g_print("...............................................%s\n", title_concat.c_str());

            title_stroke = title_lower;
            replace(title_stroke.begin(), title_stroke.end(), ' ', '-');

            title_dot = title_lower;
            replace(title_dot.begin(), title_dot.end(), ' ', '.');
        }

        if (strstr(title_lower.c_str(), "-")) {
            title_dot = title_lower;
            replace(title_dot.begin(), title_dot.end(), ' ', '.');
        }

        // clang-format off
        const string assumtion_names[] =
            {
            {app_name_lower},
            {title_lower},
            {app_name_dot},
            {app_name_concat},
            {title_concat},
            {app_name_stroke},
            {title_stroke},
            {title_dot}
            };
        // clang-format on

        // pass 1/2
        int pass_nr = 1;

        vector<string> list;
        for (auto const assumtion_name : assumtion_names) {
            if (assumtion_name.empty()) {
                continue;
            }

            g_print("SEARCH %d FOR %s \n", pass_nr, assumtion_name.c_str());
            key_file = scann_files(info.m_name, assumtion_name, info.m_desktop_file);

            if (key_file) {
                g_print("FOUND:%s\n", info.m_desktop_file.c_str());
                g_key_file_free(key_file);
                return false;
            }

            /*
                        string exec_string = "";
                        for (auto desktop_file_path : list) {
                            key_file = g_key_file_new();

                            g_print("%s\n", desktop_file_path.c_str());
                            break;
                            auto const found = g_key_file_load_from_file(
                                key_file, desktop_file_path.c_str(), GKeyFileFlags::G_KEY_FILE_NONE,
               &error); if (!found) { if (error) { g_error_free(error); error = nullptr;
                                }
                                g_key_file_free(key_file);
                                continue;
                            }

                            g_key_file_get_boolean(key_file, "Desktop Entry", "NoDisplay", &error);
                            if (!error) {
                                error = nullptr;
                                g_key_file_free(key_file);
                                continue;
                            }

                            if (error) {
                                g_error_free(error);
                                error = nullptr;
                            }

                            char* exec = g_key_file_get_string(key_file, "Desktop Entry", "Exec",
               &error); if (!exec) { if (error) { g_error_free(error); error = nullptr;
                                }
                                g_key_file_free(key_file);
                                continue;
                            }

                            // check with fast method first;
                            if (exec && strncmp(exec, app.c_str(), app.size()) == 0) {
                                // TODO:: add to cache
                                //        g_print("%s\n", desktop_file_path.c_str());
                                set(key_file, desktop_file_path, info);
                                g_key_file_free(key_file);
                                return true;
                            }

                            // try to find any appname in the string.
                            exec_string = exec;
                            auto pos = exec_string.find(exec, 0, app.size());
                            if (pos != std::string::npos) {
                                //      g_print("%s\n", desktop_file_path.c_str());
                                error = NULL;
                                set(key_file, desktop_file_path, info);
                                g_key_file_free(key_file);
                                return true;
                            }

                            g_key_file_free(key_file);
                        }
            */
            pass_nr++;
        }
        return false;
    }
}  // namespace desktopfile_util

DL_NS_END
