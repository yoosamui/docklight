// clang-format off
#include <glibmm/i18n.h>

#include "components/panel.h"
#include "translations.h"
// clang-format on

namespace docklight
{

    Panel::Panel()
    {
        //    m_appupdater.signal_update().connect(sigc::mem_fun(this, &Panel::on_update));
        //    Glib::signal_timeout().connect(sigc::mem_fun(*this, &Panel::on_timeout), 1000 / 12);
    }

    Panel::~Panel()
    {
        // std::cout << MSG_FREE_OBJECT << "\n" << std::endl;
        g_print(MSG_FREE_OBJECT, "Panel");
        g_print("\n");
    }
}  // namespace docklight

