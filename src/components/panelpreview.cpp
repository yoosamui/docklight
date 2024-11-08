//  Copyright (c) 2018-2024 Juan R. González
//
//  This file is part of Docklight.
//
//  Docklight is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  Docklight is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.

// clang-format off
#include <iostream>

#include "panelpreview.h"

#include "utils/system.h"
#include "translations.h"
// clang-format on

namespace docklight
{

    PanelPreview::PanelPreview() : Gtk::Window(Gtk::WindowType::WINDOW_POPUP)
    {
        // set_skip_taskbar_hint(true);
        // set_skip_pager_hint(true);
        //// set_resizable(false);
        // set_can_focus(false);
        //// set_type_hint(Gdk::WindowTypeHint::WINDOW_TYPE_HINT_DOCK);
        //        Gtk::Window(Gtk::WindowType::WINDOW_POPUP);
        set_resizable(true);

        // set_decorated(true);

        set_resizable(true);
        set_skip_taskbar_hint(true);
        set_skip_pager_hint(true);
        set_keep_above(true);
        // auto filename = "data/images/explodes.svg";

        // try {
        // m_image = Gdk::Pixbuf::create_from_file(filename);
        // m_size = m_image->get_width();
        // m_frames = std::floor(m_image->get_height() / m_size);
        //} catch (const std::exception& ex) {
        // g_critical("Exception 'PanelPreview'. :%s", ex.what());
        //} catch (...) {
        // std::exception_ptr p = std::current_exception();
        // auto message =
        //"Exception 'PanelPreview'. Unable to load the desired image: %s, type: %s";
        // g_critical(message, filename, p ? p.__cxa_exception_type()->name() : "unknown type");
        //}

        m_size = Config()->get_preview_image_size();  // m_image->get_width();
        // set_size_request(m_size * 3, m_size);
        // resize(512 * 3, 512);
    }

    PanelPreview::~PanelPreview()
    {
        connect_signal(false);
        g_message(MSG_FREE_OBJECT, "PanelPreview");
    }

    void PanelPreview::connect_signal(bool connect)
    {
        if (connect) {
            m_sigc_connection = Glib::signal_timeout().connect(
                sigc::mem_fun(this, &PanelPreview::on_timeout_draw), 1000 / 4);
        } else {
            m_sigc_connection.disconnect();
        }
    }

    //  hide();  cause  GLib-GObject-CRITICAL **: 09:33:13.256: g_object_unref: assertion
    //  'G_IS_OBJECT (object)' failed
    // TODO Work around
    /*
     Asks to keep window above, so that it stays on top.
     Note that you shouldn’t assume the window is definitely
     above afterward, because other entities (e.g. the user or
     [window manager][gtk-X11-arch]) could not keep it above,
     and not all window managers support keeping windows above.
     But normally the window will end kept above.

     Just don’t write code that crashes if not.
     */
    // set_keep_above();
    bool PanelPreview::on_timeout_draw()

    {
        m_frame_time = g_get_real_time();
        //    if (m_frame_time - m_start_time <= DF_EXPLODES_TIMEMAX) {
        Gtk::Widget::queue_draw();
        return true;
        //    }

        // hide();
        return true;
    }

    guint PanelPreview::get_width()
    {
        return m_size;
    }

    void PanelPreview::show_at(int x, int y, std::shared_ptr<DockItemIcon> dockitem)
    {
        m_dockitem = dockitem;
        m_start_time = g_get_real_time();
        m_frame_time = m_start_time;

        connect_signal(true);
        // move(x - (m_size / 2), y - (m_size / 2));

        if (!system::is_mutter_window_manager()) {
            /* for (auto& it : m_dockitem->get_childmap()) {*/
            ////
            // auto child = it.second;
            // auto xid = it.first;  // get_xid();

            // WnckWindow* window = it.second->get_wnckwindow();
            ////                wnck::move_window_to_workspace(window);

            // if (window && wnck_window_is_minimized(window)) {
            // wnck::unminimize(window);
            //}

            ////              if (pixbuf::get_window_image(xid, m_image)) {
            ////              child->set_image(m_image);
            ////            }
            /*}*/
        }
        auto size = dockitem->get_childmap().size();

        show();
        resize(m_size * size, m_size);
        move(x, y);
        m_set = 0;
        m_visible = true;
    }

    bool PanelPreview::get_visible() const
    {
        return m_visible;
    }
    void PanelPreview::hide_now()
    {
        connect_signal(false);
        hide();
        m_visible = false;
        m_set = 0;
    }
    bool PanelPreview::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
    {
        cr->set_source_rgba(0.266, 0.309, 0.361, 1.0);
        cairo::rounded_rectangle(cr, 0, 0, m_size, m_size, 4.0);
        cr->fill();

        int startX = 0;
        int startY = 0;

        for (auto& it : m_dockitem->get_childmap()) {
            //
            auto child = it.second;
            auto xid = it.first;  // get_xid();

            WnckWindow* window = it.second->get_wnckwindow();

            // if (window && wnck_window_is_minimized(window)) {
            //// int event_time = gtk_get_current_event_time();
            ////   wnck::bring_above_window(window);
            //// g_message("on_draw unminimize");
            // wnck::unminimize(window);
            ////  wnck::minimize(window);
            //// wnck_window_make_below(window);
            ////   unsigned int milli = 1000;
            ////   usleep(100 * milli);
            /////wnck::select_window(window);
            //}

            std::string wstring = "";

            /*if (Provider()->get_dockitem_by_xid(xid, child)) {
                m_image = child->get_image();
            }

            if (wnck::count_in_workspace(window, wstring)) {
                std::shared_ptr<DockItemIcon> child;
                //} else {
                // if (pixbuf::get_window_image(xid, m_image, 512)) {
                // child->set_image(m_image);
                //}
            }*/
            /*if (m_set == 0) {
                wnck::move_window_to_workspace(window);
                wnck::bring_above_window(window);
                pixbuf::get_window_image(xid, m_image);
                //   m_image = child->get_image();
                //    if (pixbuf::get_window_image(xid, m_image)) {
                if (m_image) {
                    Gdk::Cairo::set_source_pixbuf(cr, m_image, startX, startY);
                    child->set_image(m_image);
                    cr->paint();
                }
            } else {
                m_image = child->get_image();
                if (m_image) {
                    Gdk::Cairo::set_source_pixbuf(cr, m_image, startX, startY);
                    cr->paint();
                }
            }*/
            //    }
#define PREV
#ifdef PREV

            if (!system::is_mutter_window_manager()) {
                if (wnck::count_in_workspace(window, wstring)) {
                    std::shared_ptr<DockItemIcon> dockitem;

                    for (auto& item : Provider()->data()) {
                        if (wnck::is_window_on_current_desktop(window)) {
                            continue;
                        }

                        //   dockitem->get_group_name().c_str());
                        // if (item->get_hash() == child->get_hash()) {
                        ////  g_print("%lu == %lu \n", item->get_hash(), child->get_hash());
                        // continue;
                        //}

                        // wnck::
                        for (auto& it : item->get_childmap()) {
                            auto source = it.second;

                            //  if (child->get_hash() == source->get_hash()) continue;
                            if (it.first != xid) {
                                //  g_print("%lu == %lu \n", it.first, xid);
                                continue;
                            }

                            //  g_print("%lu == %lu \n", it.first, xid);
                            // if (it.firstsource->get_xid() != xid) continue;
                            //
                            if (m_set == 0) {
                                wnck::move_window_to_workspace(window);
                            }

                            wnck::unminimize(window);
                            pixbuf::get_window_image(xid, m_image);

                            //   auto image = source->get_image();
                            if (!m_image) {
                                m_image = source->get_image();
                                //  g_print("FOUND %lu %s \n", it.first,
                                //  source->get_window_name().c_str());

                                // char filename[512];
                                // sprintf(filename, "/home/yoo/TEMP/docklight_icons/%lu_%s",
                                // source->get_xid(), source->get_instance_name().c_str());

                                // m_image->save(filename, "png");
                            }
                            break;
                        }
                    }
                } else {
                    //                    m_image = child->get_image();

                    // if (m_set == 0) {
                    wnck::move_window_to_workspace(window);
                    //  }
                    // pixbuf::get_window_image(xid, m_image);
                    //  if (!wnck_window_is_minimized(window)) {
                    pixbuf::get_window_image(xid, m_image);
                    //} else {
                    // m_image = child->get_image();

                    // if (!m_image) {
                    // wnck::unminimize(window);
                    // pixbuf::get_window_image(xid, m_image);
                    //}
                    //}

                    if (m_image) {
                        child->set_image(m_image);
                    }
                    //                }

                    if (m_image) {
                        Gdk::Cairo::set_source_pixbuf(cr, m_image, startX, startY);
                        cr->paint();
                    }
                    m_set = true;
                }
            } else {
                if (pixbuf::get_window_image(xid, m_image)) {
                    Gdk::Cairo::set_source_pixbuf(cr, m_image, startX, startY);
                    cr->paint();
                }
            }
#endif
            startX += m_size;
            startY = 0;
        }
        m_set = 1;
        // Replace destination layer (bounded)
        // cr->set_operator(Cairo::Operator::OPERATOR_SOURCE);

        // int ypos = -m_size * (int)(m_frames * (m_frame_time - m_start_time) /
        // DF_EXPLODES_TIMEMAX); Gdk::Cairo::set_source_pixbuf(cr, m_image, 0, ypos);
        // cr->paint();
        // exit(1);
        return false;
    }
}  // namespace docklight
