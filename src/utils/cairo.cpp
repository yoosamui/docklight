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
#include "cairo.h"
#include "blur.c"
// clang-format on
//
namespace docklight
{

    namespace cairo
    {
        void rounded_rectangle(const Cairo::RefPtr<Cairo::Context>& cr, double x, double y,
                               double width, double height, double radius)
        {
            if (radius < 1.0) {
                cr->rectangle(x, y, width, height);
                return;
            }

            // radius can be no larger than half, height or width
            radius = std::min(radius, std::min(width / 2, height / 2));
            cr->move_to(x + radius, y);
            cr->arc(x + width - radius, y + radius, radius, M_PI * 1.5, M_PI * 2);
            cr->arc(x + width - radius, y + height - radius, radius, 0, M_PI * .5);
            cr->arc(x + radius, y + height - radius, radius, M_PI * .5, M_PI);
            cr->arc(x + radius, y + radius, radius, M_PI, M_PI * 1.5);
        }

        void stroke(const Cairo::RefPtr<Cairo::Context>& cr, const ColorWindow& solid_style,
                    const Gdk::Rectangle& rect)
        {
            if (solid_style.Stroke().Color::alpha != 0.0) {
                rounded_rectangle(cr, rect.get_x(), rect.get_y(), rect.get_width(),
                                  rect.get_height(), solid_style.Ratio());

                cr->set_source_rgba(
                    solid_style.Stroke().Color::red, solid_style.Stroke().Color::green,
                    solid_style.Stroke().Color::blue, solid_style.Stroke().Color::alpha);

                cr->set_line_width(solid_style.LineWidth());
                cr->stroke();
            }
        }

        void fill(const Cairo::RefPtr<Cairo::Context>& cr, const ColorWindow& solid_style,
                  const Gdk::Rectangle& rect)
        {
            ColorWindow empty(Color(0, 0, 0, 0), Color(0, 0, 0, 0), 0, 0, 0);
            fill(cr, solid_style, empty, rect);
        }

        void fill(const Cairo::RefPtr<Cairo::Context>& cr, const ColorWindow& solid_style,
                  const ColorWindow& gradient_style, const Gdk::Rectangle& rect)
        {
            if (solid_style.Fill().Color::alpha <= 0.f) {
                return;
            }

            rounded_rectangle(cr, rect.get_x(), rect.get_y(), rect.get_width(), rect.get_height(),
                              solid_style.Ratio());
            // instance
            cairo_pattern_t* pattern = nullptr;

            // defines the offset for the solid color
            double base_offset = gradient_style.Ratio();
            if (base_offset > 0.f) {
                // get dock location
                auto const location = Config()->get_dock_location();

                // gradient directions
                // 0 = left to right, 1 = right to left, 2 = top to bottom, 3 = bottom to top
                auto const gradient_direction = (int)gradient_style.LineWidth();
                // handle dock orientation
                //
                if (Config()->get_dock_orientation() == Gtk::ORIENTATION_HORIZONTAL) {
                    switch (gradient_direction) {
                        case 1:
                            pattern = cairo_pattern_create_linear(rect.get_width(), rect.get_y(),
                                                                  rect.get_x(), rect.get_height());
                            break;

                        case 2:
                            if (location == dock_location_t::top) {
                                pattern = cairo_pattern_create_linear(0, rect.get_height(), 0,
                                                                      rect.get_y());
                            } else {
                                pattern = cairo_pattern_create_linear(0, rect.get_y(), 0,
                                                                      rect.get_height());
                            }

                            break;

                        case 3:
                            pattern =
                                cairo_pattern_create_linear(0, rect.get_height(), 0, rect.get_y());
                            break;
                        default:
                            pattern = cairo_pattern_create_linear(
                                rect.get_y(), rect.get_x(), rect.get_width(), rect.get_height());
                            break;
                    }
                } else {
                    switch (gradient_direction) {
                        case 1:
                            pattern =
                                cairo_pattern_create_linear(0, rect.get_height(), 0, rect.get_y());
                            break;

                        case 2:
                            if (location == dock_location_t::left) {
                                pattern = cairo_pattern_create_linear(rect.get_width(), 0,
                                                                      rect.get_x(), 0);
                            } else {
                                pattern = cairo_pattern_create_linear(rect.get_x(), 0,
                                                                      rect.get_width(), 0);
                            }
                            break;

                        case 3:
                            if (location == dock_location_t::left) {
                                pattern = cairo_pattern_create_linear(rect.get_x(), 0,
                                                                      rect.get_width(), 0);
                            } else {
                                pattern = cairo_pattern_create_linear(rect.get_width(), 0,
                                                                      rect.get_x(), 0);
                            }
                            break;
                        default:
                            pattern =
                                cairo_pattern_create_linear(0, rect.get_y(), 0, rect.get_height());
                            break;
                    }
                }

                // mandatory start color
                cairo_pattern_add_color_stop_rgb(
                    pattern, gradient_style.Fill().Color::alpha, gradient_style.Fill().Color::red,
                    gradient_style.Fill().Color::green, gradient_style.Fill().Color::blue);

                // mandatory base end from solid color
                cairo_pattern_add_color_stop_rgb(
                    pattern, base_offset, solid_style.Fill().Color::red,
                    solid_style.Fill().Color::green, solid_style.Fill().Color::blue);

                // optional last end get it from stroke colors make a 3D effect.
                if (gradient_style.Stroke().Color::alpha > 0.0) {
                    cairo_pattern_add_color_stop_rgb(pattern, gradient_style.Stroke().Color::alpha,
                                                     gradient_style.Stroke().Color::red,
                                                     gradient_style.Stroke().Color::green,
                                                     gradient_style.Stroke().Color::blue);
                }

                // set the pattern as source
                if (pattern) cairo_set_source(cr->cobj(), pattern);
            } else {
                if (solid_style.Fill().Color::alpha != 0.f) {
                    cr->set_source_rgba(
                        solid_style.Fill().Color::red, solid_style.Fill().Color::green,
                        solid_style.Fill().Color::blue, solid_style.Fill().Color::alpha);
                }
            }

            cr->fill();

            if (pattern) {
                cairo_pattern_destroy(pattern);
            }
        }

    }  // namespace cairo
}  // namespace docklight

