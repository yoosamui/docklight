#pragma once

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

#define DOCKLIGHT_INSTANCENAME "docklight"

#define DL_NS_BEGIN     \
    namespace docklight \
    {
#define DL_NS_END }

using namespace std;

enum class dock_location_t : unsigned int {
    bottom = 0,
    left,
    top,
    right,
};

enum class dock_alignment_t : unsigned int {
    center = 0,
    start,
    end,
    fill,
};

enum class struts_position_t : unsigned int {
    Left = 0,
    Right = 1,
    Top = 2,
    Bottom = 3,
    LeftStart = 4,
    LeftEnd = 5,
    RightStart = 6,
    RightEnd = 7,
    TopStart = 8,
    TopEnd = 9,
    BottomStart = 10,
    BottomEnd = 11
};

enum class dock_item_type_t : unsigned int {
    launcher = 0,
    group,
    separator

};

enum class dock_autohide_type_t : int {
    none = -1,
    autohide = 0,
    intelihide = 1,
};
