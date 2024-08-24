#include "ExplodesWindow.h"

#include <iostream>

namespace docklight
{

    ExplodesWindow::ExplodesWindow()
    {
        set_type_hint(Gdk::WindowTypeHint::WINDOW_TYPE_HINT_DOCK);
        // TODO: relative path
        std::string filename =
            "/media/yoo/data/devs/DockLightDev/sources/docklight_5.0.orig/data/images/explodes.svg";

        m_poof_image = Gdk::Pixbuf::create_from_file(filename);
        m_poof_size = m_poof_image->get_width();
        m_poof_frames = std::floor(m_poof_image->get_height() / m_poof_size);

        set_size_request(m_poof_size, m_poof_size);

        /*m_timeout_draw =
            Glib::SignalTimeout::connect(sigc::mem_fun(this, &ExplodesWindow::on_timeout_draw),
           1000);*/
    }

    ExplodesWindow::~ExplodesWindow()
    {
        std::cout << "FREEEEEEEEEEEEEEEE\n" << std::endl;
    }

    void ExplodesWindow::ConnectSignal(bool b)
    {
        if (b) {
            m_timeout_draw = Glib::signal_timeout().connect(
                sigc::mem_fun(this, &ExplodesWindow::on_timeout_draw), m_frame_time);
            g_print("Connected\n");
        } else {
            m_timeout_draw.disconnect();
            g_print("Disconnected\n");
        }
        //
    }
    bool ExplodesWindow::on_timeout_draw()
    {
        m_frame_time = g_get_real_time();
        if (m_frame_time - m_start_time <= RUN_LENGTH) {
            Gtk::Widget::queue_draw();
            return true;
        }

        hide();
        ConnectSignal(false);
        return true;
    }
    void ExplodesWindow::show_at(int x, int y)
    {
        g_print("%dx%d\n ", x, y);
        m_start_time = g_get_real_time();
        m_frame_time = m_start_time;
        ConnectSignal(true);
        show();

        move(x - (m_poof_size / 2), y - (m_poof_size / 2));
        // GdkPixbufAnimationIter* iter = gdk_pixbuf_animation_get_iter(anim, start_time);
        // auto b = iter->

        // uint animation_timer_id = 0U;
        // animation_timer_id = Gdk.threads_add_timeout(30);
        //
        // std::thread t([]() {
        ////
        // std::cout << "thread function\n" << std::endl;
        //});
    }

    bool ExplodesWindow::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
    {
        // Replace destination layer (bounded)
        cr->set_operator(Cairo::Operator::OPERATOR_SOURCE);
        // Gdk.cairo_set_source_pixbuf (cr, poof_image, 0, -poof_size * (int) (poof_frames *
        // (frame_time
        // - start_time) / (double) RUN_LENGTH));
        int ypos = -m_poof_size *
                   (int)(m_poof_frames * (m_frame_time - m_start_time) / (double)RUN_LENGTH);
        Gdk::Cairo::set_source_pixbuf(cr, m_poof_image, 0, ypos);
        g_print("%d\n", ypos);
        //    cr->set_source_rgba(1, 1, 1, 0.9);
        // cr->fill();
        //    			Gdk.cairo_set_source_pixbuf (cr, poof_image, 0, -poof_size * (int)
        //    (poof_frames * (frame_time - start_time) / (double) RUN_LENGTH));
        // cr->set_source_pixbuf(m_pixbuf, 0, 0);
        cr->paint();
        //
        return false;
    }
}  // namespace docklight
