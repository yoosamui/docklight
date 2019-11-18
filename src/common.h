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

enum class dock_item_type_t : unsigned int {
    launcher = 0,
    group,
    separator,
    expander

};

enum class dock_autohide_type_t : int {
    none = -1,
    autohide = 0,
    intelihide = 1,
};

enum class dock_indicator_type_t : unsigned int {
    dots = 0,
    lines = 1,
};
