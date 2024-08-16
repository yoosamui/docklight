#include "components/panel.h"

#include <glibmm/i18n.h>
namespace docklight
{

    Panel::Panel()
    {
        //    m_appupdater.signal_update().connect(sigc::mem_fun(this, &Panel::on_update));
        //    Glib::signal_timeout().connect(sigc::mem_fun(*this, &Panel::on_timeout), 1000 / 12);
    }

    Panel::~Panel()
    {
        g_print("Free the Panel object.");
    }
}  // namespace docklight

