#pragma once

#include <gtkmm/enums.h>
#include <gtkmm/window.h>
#include "appwindow.h"
#include "common.h"

DL_NS_BEGIN
namespace position_util
{
    void set_window_position();
    void init(AppWindow* window);
}  // namespace position_util
DL_NS_END

