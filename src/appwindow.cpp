#include "appwindow.h"

DL_NS_BEGIN

AppWindow::AppWindow()
{
    /*m_panel.signal_update().connect(sigc::mem_fun(this, &AppWindow::on_update));

    Gdk::Screen::get_default()->signal_monitors_changed().connect(
        sigc::mem_fun(this, &AppWindow::on_monitors_changed));

    Gdk::Display::get_default()->signal_monitor_added().connect(
        sigc::mem_fun(this, &AppWindow::on_monitor_added));

    this->createWindow();*/
}

int AppWindow::init(Glib::RefPtr<Gtk::Application>& app)
{
    //
    return 0;  //
}

void AppWindow::on_monitors_changed()
{
    //
    this->update_position();
}

void AppWindow::on_monitor_added(const Glib::RefPtr<Gdk::Monitor>& monitor)
{
    //
    this->update_position();
}

void AppWindow::update_position()
{
    /*

        // int area = 64 + 20;  // config::get_dock_area();
        int area = config::get_dock_area();
        auto const workarea = get_workarea();
        //  auto const alignment = config::get_dock_alignment();
        // auto const panel = m_window->get_panel();
        int xpos = 0, ypos = 0, center = 0;

        int width = m_panel.get_required_size();
        //   g_print("---------- %d\n", width);
        center = workarea.get_width() / 2 - width / 2;
        xpos = workarea.get_x() + center;

        ypos = workarea.get_y() + workarea.get_height() - area;

        this->resize(width, area);
        this->move(xpos, ypos);
        */
}
DL_NS_END
