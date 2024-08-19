#pragma once
#include <stdint.h>

// Version:
#define OF_VERSION_MAJOR 0
#define OF_VERSION_MINOR 11
#define OF_VERSION_PATCH 2
#define DL_VERSION_PRE_RELEASE "master"

// application
#define DOCKLIGHT_INSTANCENAME "docklight"
#define DOCKLIGHT_APPNAME "Docklight-5"

// core:
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

// enums:
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
