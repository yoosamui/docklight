#pragma once
//  Copyright (c) 2018-2024 Juan R. González
//
//  This file is part of Docklight.
//
//  Docklight is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  Docklight is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
#include <stdint.h>

// Version:
#define DL_VERSION_MAJOR 0
#define DL_VERSION_MINOR 5
#define Dl_VERSION_PATCH 0
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

enum class dock_icon_alignment_t : unsigned int {
    center = 0,
    start,
    end,
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
