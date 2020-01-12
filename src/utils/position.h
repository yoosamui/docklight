#pragma once

#include <gdkmm/rectangle.h>
#include <gtkmm/enums.h>
#include <gtkmm/window.h>
#include "appwindow.h"
#include "common.h"

DL_NS_BEGIN
namespace position_util
{
    typedef enum struts_enum {
        left = 0,
        right = 1,
        top = 2,
        bottom = 3,
        left_start = 4,
        left_end = 5,
        right_start = 6,
        right_end = 7,
        top_start = 8,
        top_end = 9,
        bottom_start = 10,
        bottom_end = 11
    } struts_position_t;

    void set_strut(bool reset);
    void reset_strut();
    void hide();
    void hide_full();
    bool is_visible();
    int get_area();
    void init(AppWindow *window);
    void set_window_position();
    const Gdk::Rectangle get_appwindow_geometry();
    void get_center_screen_position(int targetwidth, int targetheight, int &posx, int &posy);
    Gdk::Rectangle get_workarea();
    int get_workarea_max_size();
    bool get_center_position(int index, int &x, int &y, int width, int height);

}  // namespace position_util
DL_NS_END

