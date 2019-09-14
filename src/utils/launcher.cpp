#include "launcher.h"
#include <glibmm/i18n.h>
#include "common.h"
#include "gio/gdesktopappinfo.h"
#include "utils/moreader.h"
#include "utils/string.h"

DL_NS_BEGIN

namespace launcher_util
{
    bool launch(const string& appname, const string& desktop_file)
    {
        GAppLaunchContext* context = nullptr;
        GAppInfo* app_info = nullptr;
        GKeyFile* key_file = g_key_file_new();
        GError* error = nullptr;

        if (!desktop_file.empty()) {
            gboolean found = g_key_file_load_from_file(key_file, desktop_file.c_str(),
                                                       GKeyFileFlags::G_KEY_FILE_NONE, NULL);

            if (found) {
                app_info = (GAppInfo*)g_desktop_app_info_new_from_keyfile(key_file);

                if (app_info != nullptr) {
                    char* uri = nullptr;
                    GFile* file = nullptr;
                    GList* glist_parameters = nullptr;

                    // TODO: check parameters

                    GdkDisplay* display = gdk_display_get_default();
                    context = (GAppLaunchContext*)gdk_display_get_app_launch_context(display);

                    gboolean launched =
                        g_app_info_launch_uris(app_info, glist_parameters, context, &error);

                    if (error) {
                        g_warning("Launcher: Error %s %s \n", appname.c_str(), error->message);
                        g_error_free(error);
                        error = nullptr;
                    }

                    g_object_unref(app_info);
                    g_object_unref(context);

                    if (uri != nullptr) g_free(uri);

                    if (file != nullptr) g_object_unref(file);

                    if (launched) {
                        g_key_file_free(key_file);
                        g_print("Launch via Desktop file %s\n", appname.c_str());
                        return launched;
                    }
                }
            }
        }

        // desktop file could not be found
        // launch via command line
        gboolean launched = g_spawn_command_line_async(appname.c_str(), &error);

        if (error) {
            g_warning("Lauch via command line: Error (%s) %s \n", appname.c_str(), error->message);
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
