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

}  // namespace config
DL_NS_END

