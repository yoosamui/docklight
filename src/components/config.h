#pragma once

#include <gmodule.h>
#include <gtkmm/enums.h>
#include "common.h"

DL_NS_BEGIN
namespace config
{
    void load(const GSList* args_list);
    dock_location_t get_dock_location();
    unsigned int get_dock_area();
    Gtk::Orientation get_dock_orientation();
    unsigned int get_separator_margin();
    unsigned int get_window_start_end_margin();
    unsigned int get_icon_size();

}  // namespace config
DL_NS_END

