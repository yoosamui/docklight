// clang-format off
#include <glibmm/i18n.h>

#include "components/panel.h"
#include "translations.h"




//#include <glibmm/timer.h>
//#include <gtkmm/drawingarea.h>
// clang-format on

namespace docklight
{

    Panel::Panel()
    {
        //    m_appupdater.signal_update().connect(sigc::mem_fun(this, &Panel::on_update));
        //    Glib::signal_timeout().connect(sigc::mem_fun(*this, &Panel::on_timeout), 1000 / 12);
        //  size(1o00, 100);
        //   Glib::signal_timeout().connect(sigc::mem_fun(this, &Panel::on_timeout_draw), 1000 /
        //   30);
    }

    Panel::~Panel()
    {
        // std::cout << MSG_FREE_OBJECT << "\n" << std::endl;
        g_print(MSG_FREE_OBJECT, "Panel");
        g_print("\n");
    }

    // bool Panel::on_timeout_draw()
    //{
    // g_message("TIMELLLLL");
    // Gtk::Widget::queue_draw();
    ////
    // return false;
    //}

    bool Panel::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
    {
        g_message("PANELLLLLLLLLLLLLLLLLLLLLLLLL");

        // Setting the compositing operator
        // Replace destination layer (bounded)
        cr->set_operator(Cairo::Operator::OPERATOR_SOURCE);
        cr->set_source_rgba(0.133, 0.153, 0.0, 1.0);
        cr->paint();
        return false;
    }
}  // namespace docklight

