#pragma once

#include <gdkmm/rectangle.h>
#include <gtkmm/enums.h>
#include <gtkmm/window.h>
#include "appwindow.h"
#include "common.h"

DL_NS_BEGIN
namespace position_util
{
    void init(AppWindow* window);
    void set_window_position();
    const Gdk::Rectangle get_appwindow_geometry();

}  // namespace position_util
DL_NS_END

