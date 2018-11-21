//*****************************************************************
//
//  Copyright (C) 2015 Juan R. Gonzalez
//  Created on January  2017
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
// 
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
// 
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//****************************************************************

#include <string.h>
#include "Launcher.h"
#include "DockItem.h"
#include "gio/gdesktopappinfo.h"
#include <map>
#include <glibmm/i18n.h>
#include "MoReader.h"
#include <vector>
#include "Configuration.h"

namespace Launcher
{


    std::map<std::string, std::string> dictionary = {
        {"Gpk-update-viewer", _("Gpk-update-viewer")},
        {"Gpk-application", _("Gpk-application")}

    };

    /**
     * Launch an application from a desktop file or from bash.
     * @param applicationName
     * @return true the application has been launched,
     * false (application not found or other error)
     */
    bool Launch(const std::string& appname)
    {
        return Launch(appname.c_str(), NULL);
    }

    /**
     * Launch an application from a desktop file or from bash.
     * @param applicationName
     * @return true the application has been launched,
     * false (application not found or other error)
     */

    bool Launch(const char* appname)
    {
        return Launch(appname, NULL);
    }

    /**
     * Runs a command line in the background.
     * @param const char* applicationName
     * @param const char* parameters
     * @return TRUE on success, FALSE if error is set
     */
    inline bool LauchAsync(const char* applicationName, const char* parameters)
    {
        GError *error = NULL;
        char command[PATH_MAX];
        *command = 0;

        std::string lowerappname(Utilities::stringToLower(applicationName));
        if (parameters != NULL && (int) strlen(parameters) > 0) {
            sprintf(command, "\"%s\" %s", lowerappname.c_str(), parameters);
        } else {
            sprintf(command, "\"%s\"", lowerappname.c_str());
        }

        g_print("LauchAsync attempt to launch: %s \n", command);
        gboolean launched = g_spawn_command_line_async(command, &error);
        if (error) {
            g_warning("LauchAsync: Error (%s) %s \n", lowerappname.c_str(), error->message);
            g_error_free(error);
            error = NULL;
        }

        bool result = launched && error == NULL;
        if (!result)
            g_warning("LauchAsync: Fails) %s \n", lowerappname.c_str());

        return result;
    }

    /**
     * Launch an application from a desktop file or from command line.
     * @param applicationName
     * @param parameters
     * @return true the application has been launched,
     * false (application not found or other error)
     */
    bool Launch(const char* applicationName, const char* parameters)
    {
        if (applicationName == NULL || strlen(applicationName) < 1)
            return false;

        std::string appname(applicationName);
        std::replace(appname.begin(), appname.end(), ' ', '-');

        GError *error = NULL;
        GAppLaunchContext *context = NULL;
        GAppInfo *app_info = NULL;
        GKeyFile *key_file = g_key_file_new();

        if (getDesktopFile(key_file, appname.c_str())) {
            app_info = (GAppInfo*) g_desktop_app_info_new_from_keyfile(key_file);
            if (app_info != NULL) {
                char* uri = NULL;
                GFile* file = NULL;
                GList *glist_parameters = NULL;

                if (parameters != NULL && (int) strlen(parameters) > 0 && g_app_info_supports_uris(app_info)) {
                    bool validDesktopfile = false;
                    char* execprog = g_key_file_get_string(key_file, "Desktop Entry", "Exec", &error);
                    if (error) {
                        g_error_free(error);
                        error = NULL;
                    }

                    // checks if the desktop file support parameters
                    validDesktopfile = execprog != NULL && error == NULL && strstr(execprog, "%") != NULL;

                    if (validDesktopfile) {
                        file = g_file_new_for_commandline_arg(parameters);
                        uri = g_file_get_uri(file);
                        glist_parameters = g_list_append(glist_parameters, uri);
                    } else {
                        if (LauchAsync(appname.c_str(), parameters)) {
                            g_print("Launch via command line %s\n", appname.c_str());
                            return true;
                        }
                    }
                }

                GdkDisplay *display = gdk_display_get_default();
                context = (GAppLaunchContext*) gdk_display_get_app_launch_context(display);

                gboolean launched = g_app_info_launch_uris(app_info, glist_parameters, context, &error);

                if (error) {
                    g_warning("Launcher: Error %s %s \n", appname.c_str(), error->message);
                    g_error_free(error);
                    error = NULL;
                }

                g_object_unref(app_info);
                g_object_unref(context);

                if (uri != NULL)
                    g_free(uri);

                if (file != NULL)
                    g_object_unref(file);

                g_key_file_free(key_file);
                g_print("Launch via Desktop file %s\n", appname.c_str());
                return launched;
            }
        }

        if (LauchAsync(appname.c_str(), parameters)) {
            g_print("Launch via command line %s\n", appname.c_str());
            return true;
        }

        return false;
    }

    /**
     * Get the Application names
     * @param window
     * @param std::string& the_appname
     * @param std::string& the_instancename
     * @param std::string& the_groupname
     * @param std::string& the_titlename
     * @return TRUE if the window is not a docklight instance.
     */
    gboolean getAppNameByWindow(WnckWindow* window,
            std::string& the_appname,
            std::string& the_instancename,
            std::string& the_groupname,
            std::string& the_titlename)
    {
        const std::string extensions[] = {".py", ".exe", ".sh"};
        // Checks whether or not the window has a name. wnck_window_get_name() 
        // will always return some value, even if window has no name set;
        const char* _appname = wnck_window_get_name(window);

        std::string appname(_appname);
        // Returns the class instance name for window , or NULL if window has no class instance.
        const char* _instancename = wnck_window_get_class_instance_name(window);
        if (_instancename == NULL) {
            _instancename = _appname;
        }

        std::string instancename(_instancename);
        instancename = Utilities::removeExtension(instancename, extensions);

        // Returns the class group name for window , or NULL if window belongs to no class group.
        const char* _realgroupname = wnck_window_get_class_group_name(window);
        if (_realgroupname == NULL) {
            _realgroupname = _appname;
        }

        std::string realgroupname(_realgroupname);
        realgroupname = Utilities::removeExtension(realgroupname, extensions);
        std::replace(realgroupname.begin(), realgroupname.end(), ' ', '-');

        if (realgroupname == "Wine")
            realgroupname = instancename;

        if (instancename == DOCKLIGHT_INSTANCENAME)
            return FALSE;

        the_appname = appname;
        the_instancename = instancename;
        the_groupname = realgroupname;
        the_titlename = Launcher::getTitleNameFromDesktopFile(realgroupname, instancename);

        return TRUE;
    }

    /**
     * Get the Application names
     * @param window
     * @return TRUE if the window is not a docklight instance.
     */
    std::string getAppNameByWindow(WnckWindow* window)
    {

        std::string the_appname;
        std::string the_instancename;
        std::string the_groupname;
        std::string the_titlename;


        return getAppNameByWindow(window, the_appname,
                the_instancename, the_groupname, the_titlename) == TRUE ?
                the_groupname : "";
    }

    /**
     * read a desktop file by Application Name
     * @param key_file
     * @param appname
     * @return true desktop file found, false desktop could not be found.
     */
    gboolean getDesktopFile(GKeyFile *key_file, const char* appname)
    {

        if (appname == NULL)
            return FALSE;

        if (appname == "Untitled window")
            return FALSE;

        if (appname == "Wine")
            return FALSE;


        GError *error = NULL;

        std::string name(appname);
        std::string lowername = Utilities::stringToLower(appname);

        const std::string desktopfilename[] = {
            {appname},
            {lowername},
            {"org.gnome." + name},
            {"org.gnome." + lowername}
        };

        const std::string filelocaltion[] = {
            {"/usr/share/applications/"},
            {"/usr/local/share/applications/"}
        };

        char filepath[PATH_MAX];
        for (auto p : filelocaltion) {
            for (auto f : desktopfilename) {
                sprintf(filepath, "%s%s.desktop", p.c_str(), f.c_str());

                gboolean found = g_key_file_load_from_file(key_file,
                        filepath, GKeyFileFlags::G_KEY_FILE_NONE, &error);

                if (error) {
                    g_error_free(error);
                    error = NULL;
                    continue;
                }

                if (found)
                    return TRUE;

            }
        }

        return true;
    }

    /**
     * gets the Application Name from a Desktop file
     * @param appname
     * @return the Application Name (Translated)
     */
    std::string getTitleNameFromDesktopFile(const std::string appname)
    {

        std::string theappname(appname);
        std::replace(theappname.begin(), theappname.end(), ' ', '-');

        if (dictionary.count(theappname) == 1) {
            std::string value = dictionary[theappname];
            if (value.empty())
                return theappname;

            return gettext(value.c_str());
        }

        GError *error = NULL;
        GKeyFile *key_file = g_key_file_new();
        const char* _appname = theappname.c_str();
        if (!getDesktopFile(key_file, _appname)) {
            dictionary[theappname] = theappname;
            g_key_file_free(key_file);
            return theappname;
        }

        char* elang = getenv("LANG");
        if (elang == NULL) {
            dictionary[theappname] = theappname;
            g_key_file_free(key_file);
            return theappname;
        }

        std::string lang(elang);
        lang = lang.substr(0, 2);
        if (lang == "en") {

            char* titlename = g_key_file_get_string(key_file,
                    "Desktop Entry", "Name", &error);
            if (titlename != NULL) {
                dictionary[theappname] = titlename;
                g_key_file_free(key_file);
                return titlename;
            }

            if (error) {
                g_error_free(error);
                error = NULL;
            }

            dictionary[theappname] = theappname;
            g_key_file_free(key_file);
            return theappname;
        }


        // check if the GenericName Entry exists
        std::string langNameKey = "GenericName[" + lang + "]";
        char* titlename = g_key_file_get_string(key_file,
                "Desktop Entry", langNameKey.c_str(), &error);
        if (titlename != NULL) {
            dictionary[theappname] = titlename;
            g_key_file_free(key_file);
            return titlename;
        }

        if (error) {
            g_error_free(error);
            error = NULL;
        }

        // check if the Name Entry exists
        langNameKey = "Name[" + lang + "]";
        titlename = g_key_file_get_string(key_file,
                "Desktop Entry", langNameKey.c_str(), &error);
        if (titlename != NULL) {
            dictionary[theappname] = titlename;
            g_key_file_free(key_file);
            return titlename;
        }

        if (error) {
            g_error_free(error);
            error = NULL;
        }


        // check if the dektop file is a Domain file 
        langNameKey = "X-Ubuntu-Gettext-Domain";
        char* mofile = g_key_file_get_string(key_file,
                "Desktop Entry", langNameKey.c_str(), &error);
        if (mofile == NULL) {
            if (error) {
                g_error_free(error);
                error = NULL;
            }

            dictionary[theappname] = theappname;
            g_key_file_free(key_file);
            return theappname;
        }

        langNameKey = "X-GNOME-FullName";
        char* xFullname = g_key_file_get_string(key_file,
                "Desktop Entry", langNameKey.c_str(), &error);
        if (xFullname == NULL) {
            if (error) {
                g_error_free(error);
                error = NULL;
            }
        }

        langNameKey = "GenericName";
        char* xGenericName = g_key_file_get_string(key_file,
                "Desktop Entry", langNameKey.c_str(), &error);
        if (xGenericName == NULL) {
            if (error) {
                g_error_free(error);
                error = NULL;
            }
        }

        langNameKey = "Name";
        char* xName = g_key_file_get_string(key_file,
                "Desktop Entry", langNameKey.c_str(), &error);
        if (xName == NULL) {
            if (error) {
                g_error_free(error);
                error = NULL;
            }
        }

        MoReader* mo = new MoReader();
        std::string moFilePath("/usr/share/locale-langpack/" +
                lang + "/LC_MESSAGES/" + mofile + ".mo");

        if (mo->Load(moFilePath.c_str()) < 0) {

            dictionary[theappname] = theappname;
            g_key_file_free(key_file);

            delete mo;
            return theappname;

        }

        std::string xFullnameTranslatedText = "";
        std::string xGenericNameTranslatedText = "";
        std::string xNameTranslatedText = "";


        if (xFullname != NULL)
            xFullnameTranslatedText = mo->getText(xFullname);

        if (xGenericName != NULL)
            xGenericNameTranslatedText = mo->getText(xGenericName);

        if (xName != NULL)
            xNameTranslatedText = mo->getText(xName);


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

        if (!xNameTranslatedText.empty() &&
                xNameTranslatedText != theappname) {

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

    std::string getTitleNameFromDesktopFile(std::string desktopfile, std::string desktopfile2)
    {
        std::string result = getTitleNameFromDesktopFile(desktopfile);
        if (result == "")
            result = getTitleNameFromDesktopFile(desktopfile2);

        return result;
    }

    void LaunchSessionGroup(const char* sessiongrpName)
    {
        char filename[PATH_MAX];
        std::string thispath = Utilities::getExecPath();
        sprintf(filename, "%s/%s/%s.bin", thispath.c_str(),
                DEF_ATTACHMENTDIR, sessiongrpName);

        FILE* f;
        size_t result;
        f = fopen(filename, "rb");
        if (!f)
            return;

        struct sessionGrpData st;
        while (1) {
            result = fread(&st, sizeof (st), 1, f);
            if (feof(f) != 0)
                break;

            if (result == 0)
                g_critical("LaunchSessionGroup: Error reading file> fread\n");

            Launch(st.appname, st.parameters);
        }


        fclose(f);

    }

    int getSessionGroupData(std::vector<sessionGrpData>& data, const char* sessiongrpName)
    {
        char filename[PATH_MAX];
        std::string thispath = Utilities::getExecPath();
        sprintf(filename, "%s/%s/%s.bin", thispath.c_str(),
                DEF_ATTACHMENTDIR, sessiongrpName);

        size_t result;
        FILE* f;
        f = fopen(filename, "rb");
        if (!f)
            return 0;


        struct sessionGrpData st;
        while (1) {
            result = fread(&st, sizeof (st), 1, f);
            if (feof(f) != 0)
                break;

            if (result == 0)
                g_critical("getSessionGroupData: Error reading file> fread\n");

            data.push_back(st);
        }


        fclose(f);
        return data.size();
    }

}