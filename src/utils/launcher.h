#pragma once
#define WNCK_I_KNOW_THIS_IS_UNSTABLE 1

#include <libwnck/libwnck.h>
#include "common.h"
#include "components/dockitem.h"

DL_NS_BEGIN
namespace launcher_util
{
    bool get_app_info(WnckWindow* window, appinfo_t& info);
    bool get_desktop_file(GKeyFile* key_file, const char* appname, string& desktop_file);
    string get_name_from_desktopfile(const char* appname);
    bool launch(const string& appname, const string& desktop_filename);
    int get_desktop_files(vector<string>& desktop_file_list);
    bool get_info_from_desktopfile(GKeyFile* key_file, const char* desktop_file_name,
                                   appinfo_t& info);
    /*
         bool Launch(const std::string& appname);
         bool Launch(const char* appname);
         bool Launch(const char* applicationName, const char* parameters);
         bool LauchAsync(const char* applicationName, const char* parameters);

         void LaunchSessionGroup(const char* sessiongrpName);

         gboolean getDesktopFile(GKeyFile *key_file, const char* appname);

         std::string getAppNameByWindow(WnckWindow* window);
         gboolean getAppNameByWindow(WnckWindow* window,
                 std::string& the_appname,
                 std::string& the_instancename,
                 std::string& the_groupname,
                 std::string& the_titlename);

         std::string getTitleNameFromDesktopFile(std::string desktopfile);
         std::string getTitleNameFromDesktopFile(std::string desktopfile,
        std::string desktopfile2); std::string getWindowTitle(WnckWindow
        *window);

         int getSessionGroupData(std::vector<sessionGrpData>& data, const char*
        sessiongrpName);*/
}  // namespace launcher_util

DL_NS_END

