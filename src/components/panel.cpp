#include "components/panel.h"
#include "appwindow.h"

DL_NS_BEGIN

Panel::Panel()
{
    m_app_updater.signal_update().connect(
        sigc::mem_fun(this, &Panel::on_appupdater_update));
}
Panel::~Panel() {}
void Panel::on_appupdater_update(bool a, int b)
{
    g_print("Update signal\n");
    AppWindow::update();
}

int Panel::get_required_size()
{
    return 400;
}

bool Panel::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{
    cr->set_source_rgba(1, 0, 0, 1);
    cr->paint();
    return true;
}
DL_NS_END
