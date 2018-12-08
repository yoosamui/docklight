/* 
 * File:   DockPanel.cpp
 * Author: yoo
 * 
 * Created on November 6, 2018, 7:31 PM
 */

#include "DockPanel.h"
#include "DockWindowHandler.h"
#include "Configuration.h"
#include "Launcher.h"
#include "Animations.h"
#include "AppWindow.h"

// static members
std::vector<DockItem*> DockPanel::m_dockitems;
int DockPanel::m_currentMoveIndex;
Gtk::Window* DockPanel::m_AppWindow;

DockPanel::DockPanel()
{
    DockPanel::m_currentMoveIndex = -1;
    m_location = Configuration::get_dockWindowLocation();
    // Gets the default WnckScreen on the default display.
    WnckScreen *wnckscreen = wnck_screen_get_default();
    initTime = 0;
    endPosition = 200.0;
    DockPanel::m_AppWindow = nullptr;



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

unsigned int get_center(){ }

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

unsigned int DockPanel::get_dockItemsWidth()
{

    unsigned int size = 0;
    for (DockItem* item:m_dockitems) {
        size += item->m_width;
    }

    return size + (Configuration::get_separatorMargin() * m_dockitems.size()) +
            Configuration::get_itemSize(); //Margin 
}

void DockPanel::get_dockItemPosition(int &x1, int &y1, int &x2, int &y2, int &center, int i)
{


    int itemSize = Configuration::get_itemSize() + Configuration::get_separatorMargin();

    int start = 0;
    int increment = 0;



    //g_print("%d\n",DockWindow::getDockItemSize());

    switch (m_location)
    {
        case panel_locationType::TOP:
        case panel_locationType::BOTTOM:

            center = this->get_width() / 2;
            start = center - (m_dockitems.size() * itemSize / 2) + Configuration::get_separatorMargin() / 2;
            increment = i * itemSize;
            x1 = start + increment;
            y1 = Configuration::get_topMargin();
            x2 = Configuration::get_itemSize();
            y2 = DockWindow::getDockItemHeight() - Configuration::get_bottomMargin();

            break;
        case panel_locationType::RIGHT:
        case panel_locationType::LEFT:
            center = this->get_height() / 2;
            start = center - (m_dockitems.size() * itemSize / 2) + Configuration::get_separatorMargin() / 2;
            increment = i * itemSize;
            x1 = Configuration::get_topMargin();
            y1 = start + increment;
            x2 = Configuration::get_itemSize();
            y2 = DockWindow::getDockItemWidth() - Configuration::get_bottomMargin();
            break;


        default:
            break;
    }
    // g_print("%d\n",y1);
}


//var box = document.getElementById("box");

float compute(float t, float b, float c, float d)
{
    if (t == 0) return b;
    if ((t /= d) == 1) return b + c;
    float p = d * .3f;
    float a = c;
    float s = p / 4;
    return (a * pow(2, -10 * t) * sin((t * d - s)*(2 * M_PI) / p) + c + b);
}

float linear(int time, int begin, int change, int duration)
{
    return change * (time / duration) + begin;
}

/*
bool DockPanel::on_draw(const Cairo::RefPtr<Cairo::Context>& cr) {
  // DockWindow Color
    cr->set_source_rgba(0.0, 0.0, 0.8, 1.0); // partially translucent
    cr->paint();
    
    
    
static int fps           = 30;
static int duration	  = 2;                                   // seconds
static int iterations	  = fps * duration;                      // 120 frames
static int startPosition = 0;                                   // left end of the screen
static int endPosition	  = 200;//window.innerWidth - box.clientWidth; // right end of the screen
static int distance      = endPosition - startPosition;         // total distance
static float timeIncrement = duration / iterations;
static int position      = 0;
//static int time          = 0;
     


    // int endPosition	  = 200;//window.innerWidth - box.clientWidth; // right end of the screen
    //timeIncrement = 30;//2 / (30*2);
 //   m_time += 0.00001; //timeIncrement;
   // m_time += 1.1f;
    //int position = compute(m_time, 10, 10, 10);

     auto duration =  50.f;
    auto endTime = initTime + duration;
    auto now = m_time; //ofGetElapsedTimef();
    this->position = ofxeasing::map_clamp(now, initTime, endTime, 0, endPosition, &ofxeasing::cubic::easeIn) ;

 //   g_print("%f - %f\n", m_time, position);


    if (m_time < duration){ 
        m_time++;
    }
    
    
        if (position >= endPosition) {
                //clearInterval(handler);
                //box.style.left = endPosition + "px";
            m_time=0;
                return true;
        }


    RoundedRectangle(cr, 0, position, 100, 100, 0);
    cr->fill();
    
   
}
 */


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


    if (!DockWindow::is_visible()) {
        //return true;
    }

    int x1, y1, x2, y2, center;

    // DockWindow Color
    if (Configuration::is_panelMode()) {
        cr->set_source_rgba(0.0, 0.0, 0.8, 1.0); // partially translucent
        cr->paint();
    }
    else {
        cr->set_source_rgba(0.0, 0.0, 0.8, 0.0); // partially translucent
        cr->paint();
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

        this->get_dockItemPosition(x1, y1, x2, y2, center, idx);
        RoundedRectangle(cr, x1, y1, x2, y2, 3);
        cr->set_source_rgba(0.00, 0.50, 0.66, 1);
        cr->fill();

        RoundedRectangle(cr, x1, y1, x2, y2, 3);
        cr->set_source_rgba(255.0, 255.0, 255.0, 1);
        cr->stroke();




        // Draw circles
        double radius = 2.0;
        int margin = 4;

        int m_iconsize = DEF_ICONSIZE;
        int col = x1;
        int m_cellwidth = DEF_CELLWIDTH;
        int m_cellheight = DEF_CELLHIGHT;
        if (m_iconsize <= 25) {
            radius = 1.5;
            margin = 3;
        }

        cr->set_source_rgb(1.0, 1.0, 1.0);

        if (item->m_items.size() == 1) {
            cr->arc(col + (m_cellwidth / 2), y1 + m_cellheight - 4, radius, 0, 2 * M_PI);
        }
        else if (item->m_items.size() > 1) {
            cr->arc((col + (m_cellwidth / 2)) - margin, y1 + m_cellheight - 4, radius, 0, 2 * M_PI);
            cr->arc((col + (m_cellwidth / 2)) + margin, y1 + m_cellheight - 4, radius, 0, 2 * M_PI);
        }
        cr->fill();



        icon = item->m_image->scale_simple(DEF_ICONSIZE, DEF_ICONSIZE, Gdk::INTERP_BILINEAR);
        Gdk::Cairo::set_source_pixbuf(cr, icon, x1 + 6, y1 + 2);

        //        switch (m_location) {
        //            case panel_locationType::TOP:
        //            case panel_locationType::BOTTOM:
        //                Gdk::Cairo::set_source_pixbuf(cr, icon, x1 + 6, 6);
        //                break;
        //            case panel_locationType::RIGHT:
        //            case panel_locationType::LEFT:
        //                Gdk::Cairo::set_source_pixbuf(cr, icon, 10, y1 + 2);
        //                break;
        //        }

        cr->paint();

        idx++;
    }

    // DEBUG
    this->get_dockItemPosition(x1, y1, x2, y2, center, 0);
    cr->set_line_width(1);
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

