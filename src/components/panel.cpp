#include "components/panel.h"
DL_NS_BEGIN

bool panel::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{
    cr->set_source_rgba(1, 0, 0, 1);
    cr->paint();
    return true;
}
DL_NS_END
