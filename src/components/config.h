#pragma once

#include <gmodule.h>
#include <gtkmm/enums.h>
#include "common.h"

DL_NS_BEGIN

namespace config
{
    void load(const GSList* args_list);
    dock_location_t get_dock_location();
    int get_dock_area();
    Gtk::Orientation get_dock_orientation();
    int get_separator_margin();
    int get_window_start_end_margin();
    int get_icon_size();
    bool is_autohide();
    bool is_intelihide();
    bool is_show_title();

}  // namespace config

DL_NS_END

