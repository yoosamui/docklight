/* 
 * File:   DockPanel.cpp
 * Author: yoo
 * 
 * Created on November 6, 2018, 7:31 PM
 */

#include "DockItem.h"


#include "DockPanel.h"
#include "DockWindowHandler.h"
#include "Configuration.h"
#include "Launcher.h"
#include "Animations.h"
#include "AppWindow.h"

#include <glibmm/i18n.h>
#include <gtkmm/window.h>
#include <gtkmm/messagedialog.h>
#include <gdkmm/cursor.h>
#include <limits.h>
#include <math.h>
#include <fstream>
#include <iostream>

// static members
std::vector<DockItem*> DockPanel::m_dockitems;
int DockPanel::m_currentMoveIndex;
Gtk::Window* DockPanel::m_AppWindow;

DockPanel::DockPanel():

//m_previousCellwidth(DEF_CELLWIDTH),
m_homeiconFilePath(Utilities::getExecPath(DEF_ICONNAME))
{

    // Set masks for mouse events
    add_events(Gdk::BUTTON_PRESS_MASK |
               Gdk::BUTTON_RELEASE_MASK |
               Gdk::SCROLL_MASK |
               Gdk::SMOOTH_SCROLL_MASK |
               Gdk::POINTER_MOTION_HINT_MASK |
               Gdk::FOCUS_CHANGE_MASK |
               Gdk::ENTER_NOTIFY_MASK |
               Gdk::LEAVE_NOTIFY_MASK |
               Gdk::POINTER_MOTION_MASK);

    //    set_app_paintable(true);
    //
    //    GdkScreen *screen;
    //    GdkVisual *visual;
    //
    //    gtk_widget_set_app_paintable(GTK_WIDGET(gobj()), TRUE);
    //    screen = gdk_screen_get_default();
    //    visual = gdk_screen_get_rgba_visual(screen);
    //
    //    if (visual != NULL && gdk_screen_is_composited(screen)) {
    //        gtk_widget_set_visual(GTK_WIDGET(gobj()), visual);
    //    }
    //    




    DockPanel::m_currentMoveIndex = -1;
    m_location = Configuration::get_dockWindowLocation();

    initTime = 0;
    endPosition = 200.0;
    DockPanel::m_AppWindow = nullptr;







    // Gets the default WnckScreen on the default display.
    WnckScreen *wnckscreen = wnck_screen_get_default();
    g_signal_connect(G_OBJECT(wnckscreen), "window-opened",
                     G_CALLBACK(DockPanel::on_window_opened), NULL);

    g_signal_connect(wnckscreen, "window_closed",
                     G_CALLBACK(DockPanel::on_window_closed), NULL);

    //    g_signal_connect(wnckscreen, "active_window_changed",
    //                     G_CALLBACK(DockPanel::on_active_window_changed_callback), NULL);


    Glib::signal_timeout().connect(sigc::mem_fun(*this,
                                                 &DockPanel::on_timeoutDraw), DEF_FRAMERATE);

    // Launch timer every second
    Glib::signal_timeout().connect(sigc::mem_fun(*this,
                                                 &DockPanel::on_timeoutDraw), 1000);

}

/**
 * preInit load the attached icons and initializes variables, 
 * and create the popup menus
 * @param Gtk::Window*  window
 * @return return 0 is success or -1 is an error found
 */
int DockPanel::preInit(Gtk::Window* window)
{
    this->m_AppWindow = window;



    m_cellwidth = Configuration::get_CellWidth();
    m_cellheight = Configuration::get_CellHeight();


    const char* filename = m_homeiconFilePath.c_str();
    DockItem* dockItem = new DockItem();
    try {

        int m_iconsize = m_cellwidth - ICON_CELL_WIDTH_MARGIN;
        dockItem->m_image = Gdk::Pixbuf::create_from_file(filename,
                                                          m_iconsize, m_iconsize, true);
    }
    catch (Glib::FileError fex) {
        g_critical("preInit: file %s could not be found!\n", filename);
        return -1;

    }
    catch (Gdk::PixbufError bex) {
        g_critical("preInit: file %s PixbufError!\n", filename);
        return -1;
    }

    dockItem->m_appname = _("Desktop");
    dockItem->m_realgroupname = _("Desktop");
    dockItem->m_dockitemtype = DockItemType::SingleDock;

    m_dockitems.insert(m_dockitems.begin(), dockItem);


    // testt separator

    dockItem = new DockItem();
    dockItem->m_dockitemtype = DockItemType::Separator;
    dockItem->m_width = 12;
    m_dockitems.push_back(dockItem);


    if (m_AppWindow != nullptr) {
        ((AppWindow*)m_AppWindow)->update();
    }

}

DockPanel::~DockPanel(){ }

bool DockPanel::on_timeoutEasing()
{
    m_time++;
}

bool DockPanel::on_timeoutDraw()
{
    Gtk::Widget::queue_draw();
    return true;
}

/**
 * Emitted when a new Wnck.Window is opened on screen.
 * @param screen
 * @param window
 * @param data
 */
void DockPanel::on_window_opened(WnckScreen *screen, WnckWindow* window, gpointer data)
{
    Update(window, Window_action::OPEN);
}

/**
 * Emitted when a Wnck.Window is closed on screen.
 * @param screen
 * @param window
 * @param data
 */
void DockPanel::on_window_closed(WnckScreen *screen, WnckWindow *window, gpointer data)
{

    // updateSessioWindow(window);
    Update(window, Window_action::CLOSE);
}

/**
 * void DockPanel::Update(WnckWindow* window, bool mode)
 * Update the items and handle the X Window events window_close and window_open.
 * @param window
 * @param actiontype
 */
void DockPanel::Update(WnckWindow* window, Window_action actiontype)
{
    WnckWindowType wt = wnck_window_get_window_type(window);

    if (wt == WNCK_WINDOW_DESKTOP ||
        wt == WNCK_WINDOW_DOCK ||
        wt == WNCK_WINDOW_MENU ||
        wt == WNCK_WINDOW_SPLASHSCREEN) {
        return;
    }

    if (actiontype == Window_action::OPEN) {
        std::string the_appname;
        std::string the_instancename;
        std::string the_groupname;
        std::string the_titlename;

        if (Launcher::getAppNameByWindow(window,
                                         the_appname,
                                         the_instancename,
                                         the_groupname,
                                         the_titlename) == FALSE) {
            return;
        }

        //DEBUG
        g_print("...appnames: %s, %s, %s title:%s\n",
                the_appname.c_str(),
                the_instancename.c_str(),
                the_groupname.c_str(),
                the_titlename.c_str());

        /*
   
                if (m_currentsessionItem != nullptr && m_sessiondata.size() > 0) {
                    for (int i = m_sessiondata.size() - 1; i >= 0; i--) {
                        std::string appname(m_sessiondata[i].appname);
                        if (appname == the_groupname) {
                            if (attachToSessiongrp(window)) {
                                m_sessiondata.erase(m_sessiondata.begin() + i);
                                break;
                            }
                        }
                    }
                }
         */


        // handle DockItems groups
        for (auto item:m_dockitems) {
            if (the_groupname == item->m_realgroupname) {
                DockItem* childItem = new DockItem();
                childItem->m_realgroupname = the_groupname;
                childItem->m_appname = the_appname;
                childItem->m_titlename = the_titlename;
                childItem->m_instancename = the_instancename;
                childItem->m_isDirty = false;
                childItem->m_window = window;
                childItem->m_xid = wnck_window_get_xid(window);
                childItem->m_image = NULLPB;

                if (item->m_isAttached && item->m_isDirty) {
                    item->m_window = window;
                    item->m_xid = childItem->m_xid;
                    item->m_isDirty = false;

                    for (auto itemtorelease:item->m_items)
                        delete(itemtorelease);

                    item->m_items.clear();
                }

                item->m_items.push_back(childItem);
                return;
            }
        }


        Glib::RefPtr<Gdk::Pixbuf> appIcon = NULLPB;

        std::string theme_iconname = "";
        appIcon = IconLoader::GetWindowIcon(window, theme_iconname);
        appIcon = appIcon->scale_simple(DEF_ICONMAXSIZE,
                                        DEF_ICONMAXSIZE, Gdk::INTERP_BILINEAR);

        // Create a new Item
        DockItem* dockItem = new DockItem();
        dockItem->m_appname = the_appname;
        dockItem->m_titlename = the_titlename;
        dockItem->m_realgroupname = the_groupname;
        dockItem->m_instancename = the_instancename;
        dockItem->m_window = window;
        dockItem->m_xid = wnck_window_get_xid(window);
        dockItem->m_image = appIcon;
        dockItem->m_theme_iconname = theme_iconname;

        // Create a child items
        DockItem* childItem = new DockItem();
        childItem->m_appname = dockItem->m_appname;
        childItem->m_titlename = the_titlename;
        childItem->m_realgroupname = dockItem->m_realgroupname;
        childItem->m_instancename = dockItem->m_instancename;
        childItem->m_window = dockItem->m_window;
        childItem->m_xid = dockItem->m_xid;
        childItem->m_image = NULLPB;

        dockItem->m_items.push_back(childItem);
        m_dockitems.push_back(std::move(dockItem));

        if (m_AppWindow != nullptr) {
            ((AppWindow*)m_AppWindow)->update();
        }

    }
    else {
        // find the item to remove;
        int xid = wnck_window_get_xid(window);
        int i = 1;
        bool found = false;
        for (; i < (int)m_dockitems.size(); i++) {
            for (auto ci:m_dockitems.at(i)->m_items) {
                if (ci->m_xid == xid) {
                    found = true;
                    break;
                }
            }
            if (found)
                break;
        }
        if (xid > 0 && found) {
            auto item = m_dockitems.at(i);
            if (item->m_items.size() == 1) {
                // if is attached delete the child item.
                if (item->m_isAttached) {
                    item->m_isDirty = true;

                    delete( item->m_items.at(0));
                    item->m_items.erase(item->m_items.begin());

                    if (m_AppWindow != nullptr) {
                        ((AppWindow*)m_AppWindow)->update();
                    }
                    return;
                }

                // remove this item
                delete( item->m_items.at(0));
                m_dockitems.erase(m_dockitems.begin() + i);

                // if is not attached then it is at the end on the list.
                // we need to reset the index.
                m_currentMoveIndex = -1;

                if (m_AppWindow != nullptr) {
                    ((AppWindow*)m_AppWindow)->update();
                }
                return;
            }
            else {
                // search for the xid and remove the item
                int idx = 0;
                for (auto c:item->m_items) {
                    if (c->m_xid == xid) {
                        delete(c);
                        item->m_items.erase(item->m_items.begin() + idx);
                        if (m_AppWindow != nullptr) {
                            ((AppWindow*)m_AppWindow)->update();
                        }
                        return;
                    }
                    idx++;
                }
            }
        }
    }


}

/**
 * get the item index from mouse coordinates.
 * @param x
 * @param y
 * @return 
 */
int DockPanel::getIndex(int x, int y)
{
    int center = DockWindow::get_geometry().width / 2;
    Gdk::Point mouse(x, y);
    int result = -1;
    int idx = 0;
    int col = 0;


    switch (m_location)
    {
        case panel_locationType::TOP:
        case panel_locationType::BOTTOM:
        {
            if (Configuration::is_panelMode()) {
                col = center - (m_dockitems.size() * m_cellwidth) / 2;
                col -= (Configuration::get_separatorMargin() + m_cellwidth);
            }
            else {
                col = (m_cellwidth / 2) + (Configuration::get_separatorMargin() / 2);
            }

            for (auto item:m_dockitems) {
                if (item->m_dockitemtype == DockItemType::Separator) {
                    idx++;
                    continue;
                }

                if (mouse.get_x() >= col && mouse.get_x() <= col + m_cellwidth) {
                    result = idx;
                    break;
                }
                col += item->m_width + (Configuration::get_separatorMargin());
                idx++;
            }
        }
        case panel_locationType::LEFT:
        case panel_locationType::RIGHT:
        {
            if (Configuration::is_panelMode()) {
                col = center - (m_dockitems.size() * m_cellwidth) / 2;
                col -= (Configuration::get_separatorMargin() + m_cellwidth);
            }
            else {
                col = (m_cellwidth / 2) + (Configuration::get_separatorMargin() / 2);
            }

            for (auto item:m_dockitems) {
                if (item->m_dockitemtype == DockItemType::Separator) {
                    idx++;
                    continue;
                }

                if (mouse.get_y() + 6 >= col && mouse.get_y() -6 <= col + m_cellheight) {
                    result = idx;
                    break;
                }
                col += item->m_width + (Configuration::get_separatorMargin());
                idx++;
            }
        }
        break;
    }
    return result;
}

/**
 * handles Mouse motion: process mouse button event 
 * true to stop other handlers from being invoked for the event.
 * false to propagate the event further.
 * @param event
 * @return 
 */
bool DockPanel::on_motion_notify_event(GdkEventMotion*event)
{
    m_currentMoveIndex = this->getIndex(event->x, event->y);
    return false;
}

void DockPanel::RoundedRectangle(const Cairo::RefPtr<Cairo::Context>& cr,
                                 double x, double y, double width, double height, double radius)
{
    // radius can be no larger than half our height or width
    radius = std::min(radius, std::min(width / 2, height / 2));
    cr->move_to(x + radius, y);
    cr->arc(x + width - radius, y + radius, radius, M_PI * 1.5, M_PI * 2);
    cr->arc(x + width - radius, y + height - radius, radius, 0, M_PI * .5);
    cr->arc(x + radius, y + height - radius, radius, M_PI * .5, M_PI);
    cr->arc(x + radius, y + radius, radius, M_PI, M_PI * 1.5);

}

unsigned int DockPanel::get_dockItemsWidthUntilIndex(int idx)
{
    unsigned int size = 0;
    int count = 0;
    for (DockItem* item:m_dockitems) {
        if (count == idx) {
            break;
        }
        size += item->m_width;

        count++;
    }

    return size;
}

unsigned int DockPanel::get_dockItemsHeightUntilIndex(int idx)
{
    unsigned int size = 0;
    int count = 0;
    for (DockItem* item:m_dockitems) {
        if (count == idx) {
            break;
        }
        if (item->m_dockitemtype == DockItemType::Separator) {
            size += item->m_width;
            count++;
            continue;
        }
        size += item->m_height;

        count++;
    }

    return size;
}

unsigned int DockPanel::get_dockItemsWidth()
{

    unsigned int size = 0;
    for (DockItem* item:m_dockitems) {
        size += item->m_width;
    }

    return size + (Configuration::get_separatorMargin() * m_dockitems.size())
            ; //+            Configuration::get_itemSize(); //Margin 
}

unsigned int DockPanel::get_dockItemsHeight()
{

    unsigned int size = 0;
    for (DockItem* item:m_dockitems) {

        if (item->m_dockitemtype == DockItemType::Separator) {
            size += item->m_width;
            continue;
        }
        size += item->m_height;
    }

    return size + (Configuration::get_separatorMargin() * m_dockitems.size());

}

void DockPanel::get_dockItemPosition(DockItem* item, int &x1, int &y1, int &x2, int &y2, int &center, int i)
{
    int start = 0;
    int increment = 0;

    switch (m_location)
    {
        case panel_locationType::TOP:
        case panel_locationType::BOTTOM:
        {
            int itemsWidth = this->get_dockItemsWidth(); //+ (DockWindow::get_dockWindowStartEndMargin()/2);
            center = DockWindow::getDockWindowWidth() / 2;
            start = center - (itemsWidth / 2); // + (DockWindow::get_dockWindowStartEndMargin()/2);

            increment = get_dockItemsWidthUntilIndex(i);
            increment += (i * Configuration::get_separatorMargin()) + Configuration::get_separatorMargin() / 2;

            x1 = start + increment;
            y1 = CELL_TOP_MARGIN;
            x2 = item->m_width;
            y2 = m_cellheight;

        }
            break;
        case panel_locationType::RIGHT:
        case panel_locationType::LEFT:
        {

            int itemsWidth = this->get_dockItemsHeight(); //+ (DockWindow::get_dockWindowStartEndMargin()/2);
            center = DockWindow::getDockWindowHeight() / 2;
            start = center - (itemsWidth / 2);

            increment = get_dockItemsWidthUntilIndex(i);
            increment += (i * Configuration::get_separatorMargin()) + Configuration::get_separatorMargin() / 2;

            x1 = 2;
            y1 = start + increment;
            x2 = item->m_width;
            y2 = m_cellheight;

            if (item->m_dockitemtype == DockItemType::Separator) {
                y2 = item->m_width;
                x2 = m_cellwidth;
            }
        }


            break;


        default:
            break;
    }
    // g_print("%d\n",y1);
}

bool DockPanel::on_enter_notify_event(GdkEventCrossing* crossing_event)
{
    //  g_print("Mp_IN\n");
    //  mouseIn = true;
    return true;
}

bool DockPanel::on_leave_notify_event(GdkEventCrossing* crossing_event)
{
    // g_print("Mp_OUT\n");
    //  mouseIn = false;
    return true;
}

bool DockPanel::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{



    int x1, y1, x2, y2, center;

    // DockWindow Color
    if (Configuration::is_panelMode()) {
        cr->set_source_rgba(0.0, 0.0, 0.8, 0.4); // partially translucent
        cr->paint();
    }
    else {
        cr->set_source_rgba(0.0, 0.0, 175.8, 0.4); // partially translucent
        RoundedRectangle(cr, 0, 0, DockWindow::getDockWindowWidth(), DockWindow::getDockWindowHeight(), 6);
        cr->fill();
    }





    //        RoundedRectangle(cr, x1, y1, x2, y2, 3);
    //        cr->set_source_rgba(0.0, 0.0, 0.8, 1.0); // partially translucent
    //        cr->fill();



    // // DEBUG Show center ----------------------------------



    // int itemsWidth = (itemCount * Configuration::get_itemSize()) + ((itemCount - 1) * Configuration::get_separatorMargin());
    //int itemsHalf = itemsWidth / 2;

    // draw panel rectangle

    cr->set_line_width(1);
    cr->set_source_rgba(1, 1, 1, 1);


    Glib::RefPtr<Gdk::Pixbuf> icon = NULLPB;
    int idx = 0;

    for (auto item:m_dockitems) {


        this->get_dockItemPosition(item, x1, y1, x2, y2, center, idx);


        RoundedRectangle(cr, x1, y1, x2, y2, 3);
        cr->set_source_rgba(0.00, 0.50, 0.66, 1);
        cr->fill();

        RoundedRectangle(cr, x1, y1, x2, y2, 3);
        cr->set_source_rgba(255.0, 255.0, 255.0, 1);
        cr->stroke();

        if (item->m_dockitemtype == DockItemType::Separator) {
            idx++;
            continue;
        }




        // Draw circles
        double radius = 2.0;
        int margin = 4; //TODO: define;
        int iconsize = m_cellwidth - ICON_CELL_WIDTH_MARGIN;
        int center = (m_cellwidth / 2);
        int centerPos = center - (iconsize / 2);
        int circlePos = center;
        int col = x1;

        //      
        //        if (m_iconsize <= 25) {
        //            radius = 1.5;
        //            margin = 3;
        //        }

        cr->set_source_rgb(1.0, 1.0, 1.0);

        int heightPos = y1 + m_cellheight - CELL_BOTTOM_MARGIN - 4;
        if (item->m_items.size() == 1) {
            cr->arc(col + circlePos, heightPos, radius, 0, 2 * M_PI);
        }
        else if (item->m_items.size() > 1) {
            cr->arc(col + circlePos - margin, heightPos, radius, 0, 2 * M_PI);
            cr->arc(col + circlePos + margin, heightPos, radius, 0, 2 * M_PI);
        }
        cr->fill();




        icon = item->m_image->scale_simple(iconsize, iconsize, Gdk::INTERP_BILINEAR);
        Gdk::Cairo::set_source_pixbuf(cr, icon, x1 + centerPos, y1 + ICON_CELL_TOP_MARGIN);
        cr->paint();


        idx++;
    }


    if (m_currentMoveIndex != -1) {

        int xx1, yy1, xx2, yy2;

        g_print("AA %d\n ", m_currentMoveIndex);
        auto item = m_dockitems.at(m_currentMoveIndex);
        this->get_dockItemPosition(item, xx1, yy1, xx2, yy2, center, m_currentMoveIndex);

        RoundedRectangle(cr, xx1, yy1, xx2, yy2, 3);
        cr->set_source_rgba(1.00, 1.50, 1.66, 0.4);
        cr->fill();


        cr->set_line_width(2.5);
        cr->set_source_rgba(1.0, 1.0, 1.0, 1.0);
        RoundedRectangle(cr, xx1, yy1, xx2, yy2, 3);

        cr->stroke();
    }

    // DEBUG
    auto item = m_dockitems.at(0);
    this->get_dockItemPosition(item, x1, y1, x2, y2, center, 0);
    cr->set_line_width(3);
    cr->set_source_rgba(255.0, 245.0, 0.0, 1.0); // partially translucent
    switch (m_location)
    {
        case panel_locationType::TOP:
        case panel_locationType::BOTTOM:
            cr->line_to(center, y1);
            cr->line_to(center, y2);
            break;
        case panel_locationType::RIGHT:
        case panel_locationType::LEFT:
            cr->line_to(x1, center);
            cr->line_to(x2, center);
            break;
    }
    cr->stroke();


    return true;
}

