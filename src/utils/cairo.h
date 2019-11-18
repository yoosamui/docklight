#pragma once

#include <cairomm/context.h>
#include <glibmm/main.h>
#include "common.h"

DL_NS_BEGIN
namespace cairo_util
{
    void rounded_rectangle(const Cairo::RefPtr<Cairo::Context>& cr, double x,
                           double y, double width, double height,
                           double radius);
}
DL_NS_END
