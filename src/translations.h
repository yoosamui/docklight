#pragma once
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
//
#include <glibmm/i18n.h>
#include <libintl.h>

// clang-format off
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
#define MSG_THE_DOCK_ALIGNMENT_ SETTINGS_ARE \
    _("The dock alignment settings are start, end, center or fill. Default center."));
#define MSG_PARSED_VALUES \
    _("parsed values")
#define MSG_FREE_OBJECT \
    _("Free the %s object.")
// clang-format on
