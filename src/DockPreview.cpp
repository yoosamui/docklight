//*****************************************************************
//
//  Copyright © 2018 Juan R. González
//  j-gonzalez@email.de
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//*****************************************************************

#include "DockPreview.h"
#include "DockPanel.h"
#include "DockItemPositions.h"
#include "DockWindow.h"
//#include <gdk/gdkx.h>

std::vector<DockItem*> DockPreview::m_previewItems;
DockPreview::DockPreview():Gtk::Window(Gtk::WindowType::WINDOW_POPUP){

    // Set up the top-level window.
    set_title("DockPreview");
    set_decorated(false);
    set_app_paintable(true);

    // Set masks for mouse events
    add_events(Gdk::BUTTON_PRESS_MASK |
            Gdk::BUTTON_RELEASE_MASK |
            Gdk::BUTTON_PRESS_MASK |
            Gdk::SCROLL_MASK |
            Gdk::SMOOTH_SCROLL_MASK |
            Gdk::POINTER_MOTION_HINT_MASK |
            Gdk::FOCUS_CHANGE_MASK |
            Gdk::ENTER_NOTIFY_MASK |
            Gdk::LEAVE_NOTIFY_MASK |
            Gdk::POINTER_MOTION_MASK);


    GdkScreen *screen;
    GdkVisual *visual;

    gtk_widget_set_app_paintable(GTK_WIDGET(gobj()), TRUE);
    screen = gdk_screen_get_default();
    visual = gdk_screen_get_rgba_visual(screen);

    if (visual != NULL && gdk_screen_is_composited(screen)) {
        gtk_widget_set_visual(GTK_WIDGET(gobj()), visual);
    }


    m_font.set_family("System");
    m_font.set_size(8 * PANGO_SCALE);
    m_font.set_weight(Pango::WEIGHT_NORMAL);

    WnckScreen *wnckscreen = wnck_screen_get_default();

    Glib::signal_timeout().connect(sigc::mem_fun(*this, &DockPreview::on_timeoutDraw), 1000 / 30);

    g_signal_connect(G_OBJECT(wnckscreen), "window-opened", G_CALLBACK(DockPreview::on_window_opened), NULL);
    g_signal_connect(wnckscreen, "window_closed", G_CALLBACK(DockPreview::on_window_closed), NULL);

    //m_timer.start();
}

DockPreview::~DockPreview()
{
    // g_object_unref static items
    for (DockItem* item : m_previewItems) {
        if (item->m_scaledPixbuf) {
            g_object_unref(item->m_scaledPixbuf);
        }
    }

    g_print("Preview destroy.\n");
}

void DockPreview::init(const std::vector<DockItem*>& items, const guint index)
{
    // Using assignment operator to copy the items vector
    DockPreview::m_previewItems = items;

    int x = 0;
    int y = 0;
    guint hv_diff = 20;
    this->cellWidth = 200;
    this->cellHeight= this->cellWidth - hv_diff;
    guint separatorsSize = Configuration::get_separatorMargin() * (items.size() - 1);

    if (DockWindow::is_Horizontal()){

        int width = this->cellWidth  *  items.size() + DockWindow::get_dockWindowStartEndMargin() + separatorsSize;

        if (width  > DockWindow::Monitor::get_geometry().width){
            width = DockWindow::Monitor::get_geometry().width;
            this->cellWidth = (DockWindow::Monitor::get_geometry().width - DockWindow::get_dockWindowStartEndMargin() - separatorsSize ) / items.size();
            this->cellHeight = this->cellWidth - hv_diff;
        }

        this->set_size_request(width, this->cellHeight);
        DockItemPositions::get_CenterPosition(index, x, y, width, this->cellHeight );

    } else {

        // make the cell height smaller looks better
        this->cellHeight -= hv_diff;
        int height = this->cellHeight  *  items.size() + DockWindow::get_dockWindowStartEndMargin() + separatorsSize;

        if (height  > DockWindow::Monitor::get_geometry().height){
            height = DockWindow::Monitor::get_geometry().height;
            this->cellHeight =   (DockWindow::Monitor::get_geometry().height - DockWindow::get_dockWindowStartEndMargin() - separatorsSize ) / items.size();
        }

        this->set_size_request(this->cellWidth, height);
        DockItemPositions::get_CenterPosition(index, x, y, this->cellWidth,  height);
    }

    this->move(x, y);
}
/**
 * handles on_enter_notify_event
 * Event triggered by pointer enter widget area.
 * The signal will be emitted when the pointer enters the widget's window.
 * This signal will be sent to the grab widget if there is one.
 *
 * true to stop other handlers from being invoked for the event.
 * false to propagate the event further.
 * @param crossing_event
 * @return true to stop other handlers from being invoked for the event. false to propagate the event further
 */
bool DockPreview::on_enter_notify_event(GdkEventCrossing* crossing_event)
{
    m_canLeave = true;
    m_mouseIn = true;
    //    A
    //    m_dockpanelReference->m_previewWindowActive = true;
    return true;
}

/**
 * handles on_leave_notify_event
 * Event triggered by pointer leaving widget area.
 * The signal_leave_notify_event() will be emitted when the pointer leaves the widget's window.
 * To receive this signal, the Gdk::Window associated to the widget needs to enable the Gdk::LEAVE_NOTIFY_MASK mask.
 * This signal will be sent to the grab widget if there is one.
 *
 * @param crossing_event
 * @return true to stop other handlers from being invoked for the event. false to propagate the event further
 */
bool DockPreview::on_leave_notify_event(GdkEventCrossing* crossing_event)
{
    if (!m_canLeave) {
        return true;
    }

    this->hideMe();

    // tell the caller that we are leaving...
    //  m_dockpanelReference->previewWindowClosed();
    return true;
}

/**
 * Hide the window and reset values;
 */
void DockPreview::hideMe()
{
    //  hide();

    m_canLeave = true;
    //  m_isActive = false;
    //  m_isVisible = false;
    //    m_mouseIn = false;
    //m_dockpanelReference->m_previewWindowActive = false;



    //this->m_previewItems.clear();
    DockPanel::PreviewClose();
}

bool DockPreview::on_timeoutDraw(){

    Gtk::Widget::queue_draw();
    return true;
}
void DockPreview::on_window_opened(WnckScreen *screen, WnckWindow *window, gpointer data){

}
void DockPreview::on_window_closed(WnckScreen *screen, WnckWindow *window, gpointer data){

}
bool DockPreview::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{

    int  width = this->get_width();
    int  height = this->get_height();


    cr->set_source_rgba(
            this->m_Theme.Panel().Fill().Color::red,
            this->m_Theme.Panel().Fill().Color::green,
            this->m_Theme.Panel().Fill().Color::blue,
            this->m_Theme.Panel().Fill().Color::alpha);

    int x,y;

    if (DockWindow::is_Horizontal()){

        Utilities::RoundedRectangle(cr, 0, 0, width, this->cellHeight ,6);
        cr->fill();
        y = 8; //margin
        x =  DockWindow::get_dockWindowStartEndMargin() / 2 ;
    }
    else
    {

        Utilities::RoundedRectangle(cr, 0, 0, this->cellWidth, height, 6);
        cr->fill();
        x = 8; //margin
        y = DockWindow::get_dockWindowStartEndMargin() / 2 ;

    }


    //        g_print("Items %d\n",(int)this->m_previewItems.size());
    int idx = 0;
    for (DockItem* item : this->m_previewItems)
    {

        cr->set_source_rgba(
                this->m_Theme.PanelCell().Stroke().Color::red,
                this->m_Theme.PanelCell().Stroke().Color::green,
                this->m_Theme.PanelCell().Stroke().Color::blue,
                this->m_Theme.PanelCell().Stroke().Color::alpha);

        if (DockWindow::is_Horizontal()){

            Utilities::RoundedRectangle(cr, x, y, this->cellWidth  , this->cellHeight - 16  ,6);
            cr->stroke();
            x +=  this->cellWidth + Configuration::get_separatorMargin();
        }
        else
        {
            Utilities::RoundedRectangle(cr, x, y, this->cellWidth - 16  , this->cellHeight  ,6);
            cr->stroke();

            y +=  this->cellHeight + Configuration::get_separatorMargin();
        }
    }
    return true;

}
