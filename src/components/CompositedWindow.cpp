#include "components/CompositedWindow.h"

#include <iostream>
#include <thread>
//#include <cairomm/context.h>
//#include <cairomm/enums.h>
//#include "cairo.h"
CompositedWindow::CompositedWindow()
{
    GdkScreen* screen;
    GdkVisual* visual;

    gtk_widget_set_app_paintable(GTK_WIDGET(gobj()), TRUE);
    screen = gdk_screen_get_default();
    visual = gdk_screen_get_rgba_visual(screen);

    if (visual != nullptr && gdk_screen_is_composited(screen)) {
        gtk_widget_set_visual(GTK_WIDGET(gobj()), visual);
    }

    set_keep_above(true);
    set_decorated(false);
    set_resizable(false);
}

bool CompositedWindow::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{
    cr->save();
    // Clear destination layer (bounded)
    cr->set_operator(Cairo::Operator::OPERATOR_CLEAR);
    cr->paint();
    cr->restore();

    return false;  // Gdk::EVENT_STOP;
}
//
/////////////////////////////////////////////////////////////
///

PlankTest::PlankTest()
{
    // Connect to timeout
    // m_connectionStopWatch = Glib::SignalTimeout::connect(
    // sigc::mem_fun(*this, &CStopWatchClass::OneSecondElapsed), 1000);

    set_type_hint(Gdk::WindowTypeHint::WINDOW_TYPE_HINT_DOCK);

    // set_type_hint(Gdk::WindowTypeHint::WINDOW_TYPE_HINT_DOCK);
    // set_type(Gdk::WindowType::WINDOW_TOPLEVEL);
    std::string filename =
        "/media/yoo/data/devs/DockLightDev/sources/docklight_5.0.orig/data/images/explodes.svg";

    // GError** error = nullptr;
    // anim = gdk_pixbuf_animation_new_from_file(filename.c_str(), error);
    anim = gdk_pixbuf_simple_anim_new(m_width, m_height, m_rate);
    // m_pixbuf =
    m_poof_image = Gdk::Pixbuf::create_from_file(filename);
    m_poof_size = m_poof_image->get_width();
    m_poof_frames = std::floor(m_poof_image->get_height() / m_poof_size);

    set_size_request(m_poof_size, m_poof_size);
    // m_pixbuf = get_from_file(filename.c_str(), m_width, m_height);
    // m_pixbuf = gdk_pixbuf_new_from_file(filename.c_str(), error);
    // Glib::wrap(poof_image);
    //   m_animation = Gdk::PixbufAnimation::create_from_file(filename);
    //      Glib::RefPtr<PixbufAnimationIter> get_iter(gint64 start_time = g_get_real_time())
    //   i.. _poof_image = m_animation->get_pixbuf();
    //    gint64 start_time = g_get_real_time();
    //   gint64 frame_time = -1LL;
    //   get_iter (gint64 start_time=g_get_real_time())i
    //
    //   auto iter = Gdk::PixbufAnimation::get_iter();
    //      m_animation->get_iter();
    //   Gdk::PixbufAnimation::get_iter();
    //      Gdk::PixbufAnimation::get_iter();
    //      gdk_pixbuf_animation_get_iter(m_animation.c_obj());

    /*m_timeout_draw =
        Glib::SignalTimeout::connect(sigc::mem_fun(this, &PlankTest::on_timeout_draw), 1000);*/
}

PlankTest::~PlankTest()
{
    std::cout << "FREEEEEEEEEEEEEEEE\n" << std::endl;
}

void PlankTest::ConnectSignal(bool b)
{
    if (b) {
        m_timeout_draw = Glib::signal_timeout().connect(
            sigc::mem_fun(this, &PlankTest::on_timeout_draw), 1000 / 30);
        g_print("Connected\n");
    } else {
        m_timeout_draw.disconnect();
        g_print("Disconnected\n");
    }
    //
}
bool PlankTest::on_timeout_draw()
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
void PlankTest::show_at(int x, int y)
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

bool PlankTest::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{
    // Replace destination layer (bounded)
    cr->set_operator(Cairo::Operator::OPERATOR_SOURCE);
    // Gdk.cairo_set_source_pixbuf (cr, poof_image, 0, -poof_size * (int) (poof_frames * (frame_time
    // - start_time) / (double) RUN_LENGTH));
    int ypos =
        -m_poof_size * (int)(m_poof_frames * (m_frame_time - m_start_time) / (double)RUN_LENGTH);
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
