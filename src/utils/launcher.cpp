#include "utils/launcher.h"

#include <gdkmm/display.h>

#include "gio/gdesktopappinfo.h"
namespace docklight
{

    void launcher(const Glib::ustring& desktop_file, const Glib::ustring& instance_name)
    {
        Glib::RefPtr<Gio::DesktopAppInfo> appinfo =
            Gio::DesktopAppInfo::create_from_filename(desktop_file);

        if (!appinfo) {
            g_warning("get_theme_icon::Gio::DesktopAppInfo: the object has not been created.");
            return;
        }
        // https://glibmm-j-bu-3a9a6d7c542ea4fb7b0c9fe83930335cc321f61bcc0d6d59b4e.pages.gitlab.gnome.org/classGio_1_1AppInfo.html
        //  AYNC
        appinfo->launch_uri(instance_name);
    }

    void launcher2(const Glib::ustring& desktop_file, const Glib::ustring& instance_name,
                   const Glib::ustring& group_name, const Glib::ustring& icon_name)
    {
        g_print("APP %s\n", instance_name.c_str());

        GAppLaunchContext* context = nullptr;
        GAppInfo* app_info = nullptr;
        GKeyFile* key_file = g_key_file_new();
        GError* error = nullptr;

        if (!desktop_file.empty()) {
            gboolean found = g_key_file_load_from_file(key_file, desktop_file.c_str(),
                                                       GKeyFileFlags::G_KEY_FILE_NONE, nullptr);

            if (found) {
                app_info = (GAppInfo*)g_desktop_app_info_new_from_keyfile(key_file);
                if (app_info) {
                    char* uri = nullptr;
                    GFile* file = nullptr;
                    GList* glist_parameters = nullptr;

                    GdkDisplay* display = gdk_display_get_default();
                    context = (GAppLaunchContext*)gdk_display_get_app_launch_context(display);

                    gboolean launched =
                        g_app_info_launch_uris(app_info, glist_parameters, context, &error);

                    if (error) {
                        g_warning("Launcher: Error %s %s \n", instance_name.c_str(),
                                  error->message);

                        g_error_free(error);
                        error = nullptr;
                    }

                    g_object_unref(app_info);
                    g_object_unref(context);

                    if (uri) g_free(uri);

                    if (file) g_object_unref(file);

                    if (launched) {
                        g_key_file_free(key_file);
                        g_print("Launch via Desktop file %s\n", instance_name.c_str());
                        return;
                    }
                }
            }
        }

        // desktop file could not be found
        // launch via command line
        // g_spawn_command_line_async(instance_name.c_str(), &error);
        // g_spawn_command_line_async(group_name.c_str(), &error);
        g_spawn_command_line_async(instance_name.c_str(), &error);

        if (error) {
            g_warning("Lauch via command line: Error (%s) %s \n", instance_name.c_str(),
                      error->message);
            g_error_free(error);
            error = nullptr;
            g_key_file_free(key_file);
        }

        g_spawn_command_line_async(group_name.c_str(), &error);
        g_print("Launch via command line: %s\n", instance_name.c_str());
        g_key_file_free(key_file);
    }
}  // namespace docklight
