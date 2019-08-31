#pragma once

#include <gmodule.h>
#include <gtkmm/enums.h>
#include "common.h"

DL_NS_BEGIN

namespace config
{
#define DEF_MIN_ITEM_SIZE 26

    void load(const GSList* args_list);
    dock_location_t get_dock_location();
    int get_dock_area();
    Gtk::Orientation get_dock_orientation();
    int get_separator_margin();
    int get_window_start_end_margin();
    int get_icon_size();
    void set_icon_size(int value);
    bool is_autohide();
    bool is_intelihide();
    bool is_show_title();
    bool is_separator_line();
    void set_separator_line(bool value);
    dock_alignment_t get_dock_alignment();
    void set_dock_alignment(dock_alignment_t value);

}  // namespace config

DL_NS_END

