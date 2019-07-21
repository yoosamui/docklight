#include "cairo.h"

DL_NS_BEGIN
namespace cairo_util
{
    void rounded_rectangle(const Cairo::RefPtr<Cairo::Context>& cr, double x,
                           double y, double width, double height, double radius)
    {
        // radius can be no larger than half our height or width
        radius = std::min(radius, std::min(width / 2, height / 2));
        cr->move_to(x + radius, y);
        cr->arc(x + width - radius, y + radius, radius, M_PI * 1.5, M_PI * 2);
        cr->arc(x + width - radius, y + height - radius, radius, 0, M_PI * .5);
        cr->arc(x + radius, y + height - radius, radius, M_PI * .5, M_PI);
        cr->arc(x + radius, y + radius, radius, M_PI, M_PI * 1.5);
    }

}  // namespace cairo_util
DL_NS_END
