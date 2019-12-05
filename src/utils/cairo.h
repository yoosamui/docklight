#pragma once
#include <cairomm/context.h>
#include <gdkmm/rectangle.h>
#include <glibmm/main.h>
#include "common.h"
#include "components/config.h"

DL_NS_BEGIN

namespace cairo_util
{
    using namespace config::style;

    // class ColorWindow;
    void rounded_rectangle(const Cairo::RefPtr<Cairo::Context>& cr, double x, double y,
                           double width, double height, double radius);

    void stroke(const Cairo::RefPtr<Cairo::Context>& cr, const ColorWindow& solid_style,
                const Gdk::Rectangle& rect);

    void fill(const Cairo::RefPtr<Cairo::Context>& cr, const ColorWindow& solid_style,
              const Gdk::Rectangle& rect);

    void fill(const Cairo::RefPtr<Cairo::Context>& cr, const ColorWindow& solid_style,
              const ColorWindow& gradient_style, const Gdk::Rectangle& rect);

}  // namespace cairo_util

DL_NS_END
