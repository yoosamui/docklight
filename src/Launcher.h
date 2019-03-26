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

#ifndef LAUNCHER_H
#define LAUNCHER_H

#define WNCK_I_KNOW_THIS_IS_UNSTABLE  1

#include "Utilities.h"
#include <libwnck/libwnck.h>
#include "SessionWindow.h"
#include <thread>
#include <mutex>

    
namespace Launcher
{
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
    std::string getTitleNameFromDesktopFile(std::string desktopfile, std::string desktopfile2);
    std::string getWindowTitle(WnckWindow *window);

    int getSessionGroupData(std::vector<sessionGrpData>& data, const char* sessiongrpName);
}

#endif /* LAUNCHER_H */

