#pragma once
//  This file is part of Docklight.
//
//  Docklight is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  Docklight is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.

//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.

// Translation messages
//
// #define _(String) gettext(String)
// redefined in /usr/include/glib-2.0/glib/gi18n.h
//
// Macro Expansion instead of using String literals:
//
// #define ready_to_go _("Ready to go")
//
// When the preprocessor encounters a macro definition like #define ready_to_go _("Ready to go"),
// it replaces all occurrences of ready_to_go with the text _("Ready to go") before compilation.
// This means that the gettext function is called during the preprocessing stage, and the
// translation is performed.

// clang-format off
#include <glibmm/i18n.h>
#include <libintl.h>

#define MSG_APPLICATION_START \
    _("Application start.")
#define MSG_APPLICATION_ACTIVATED \
    _("Application activated.")
#define MSG_INITIALIZE_PANEL \
    _("Initialize panel.")
#define MSG_SETS_THE_PRIMARY_MONITOR \
    _("Sets the primary Monitor.")
#define MSG_SETS_THE_PANEL_LOCATION \
    _("Sets the Panel location: top, right, bottom or left. Default bottom")
#define MSG_SETS_THE_DOCK_ALIGNMENT \
    _("The dock alignment settings are start, end, center or fill. Default center.")
#define MSG_SETS_THE_ICON_ALIGNMENT \
    _("The icon alignment settings are start, endi or center. Default center.")
#define MSG_PARSED_VALUES \
    _("parsed values")
#define MSG_FREE_OBJECT \
    _("Free the %s object.")
#define MSG_DISPLAY_DETECTED_MONITORS \
    _("Display detected monitors")
#define MSG_DEFAULT_MONITOR \
    _("Default monitor")
#define MSG_MONITOR \
    _("monitor")
#define MSG_GEOMETRY \
    _("Geometry")
#define MSG_WORKAREA \
    _("Workarea")
#define MSG_WIDTH \
    _("Width")
#define MSG_HEIGHT \
    _("Height")
#define MSG_MODEL \
    _("Model")
#define MSG_QUIT \
    _("Quit")
#define MSG_CLOSE_ALL \
    _("Close all")
#define MSG_SHOW_ALL \
    _("Show all")
#define MSG_MINIMIZE_ALL \
    _("Minimize all")
#define MSG_CLOSE_ALL_WINDOWS \
    _("Close all windows")
#define MSG_OPEN_NEW \
    _("Open new")
#define MSG_ATTACH \
    _("Attach")
#define MSG_MINIMIZE_ALL_WINDOWS \
    _("Minimize all windows")
#define MSG_MAXIMIZE_ALL_WINDOWS \
    _("Maximinize all windows")
// clang-format on
