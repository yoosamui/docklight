#pragma once
//  Copyright (c) 2018-2024 Juan R. González
//
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
//  public Glib::Object GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  identification number, along with this program.  If not, see <http://www.gnu.org/licenses/>.

// clang-format off
#include <cairomm/context.h>
#include <gdkmm/rectangle.h>
#include <glibmm/main.h>

#include "components/config.h"
// clang-format on

namespace docklight
{

    namespace cairo
    {
        //       using namespace config::style;

        // class ColorWindow;
        void rounded_rectangle(const Cairo::RefPtr<Cairo::Context>& cr, double x, double y,
                               double width, double height, double radius);

        /*void stroke(const Cairo::RefPtr<Cairo::Context>& cr, const ColorWindow& solid_style,
                    const Gdk::Rectangle& rect);

        void fill(const Cairo::RefPtr<Cairo::Context>& cr, const ColorWindow& solid_style,
                  const Gdk::Rectangle& rect);

        void fill(const Cairo::RefPtr<Cairo::Context>& cr, const ColorWindow& solid_style,
                  const ColorWindow& gradient_style, const Gdk::Rectangle& rect);*/

    }  // namespace cairo

}  // namespace docklight
