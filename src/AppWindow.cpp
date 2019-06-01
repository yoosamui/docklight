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

#include <glibmm-2.4/glibmm/timer.h>
#include "AppWindow.h"
#include "../config.h"
#include "DockWindow.h"
#include "Animations.h"
#include "Utilities.h"
#include "WnckHandler.h"

// static members
bool AppWindow::m_isfullscreenSet;
bool AppWindow::m_isfullscreen;
RootScreen AppWindow::m_screen;

/*
   static const gchar* type_name(WnckWindowType type)
   {
   switch (type)
   {
   case WNCK_WINDOW_NORMAL: return "normal";
   case WNCK_WINDOW_DESKTOP: return "desktop";
   case WNCK_WINDOW_DOCK: return "dock";
   case WNCK_WINDOW_DIALOG: return "dialog";
   case WNCK_WINDOW_TOOLBAR: return "toolbar";
   case WNCK_WINDOW_MENU: return "menu";
   case WNCK_WINDOW_UTILITY: return "utility";
   case WNCK_WINDOW_SPLASHSCREEN: return "splashscreen";
   default: return "UNKNOWN";
   }
   }
   */

AppWindow::AppWindow()
{
    GdkScreen *screen;
    GdkVisual *visual;

    gtk_widget_set_app_paintable(GTK_WIDGET(gobj()), TRUE);
    screen = gdk_screen_get_default();
    visual = gdk_screen_get_rgba_visual(screen);

    if (visual != NULL && gdk_screen_is_composited(screen)) {
        gtk_widget_set_visual(GTK_WIDGET(gobj()), visual);
    }

    /*
       This function returns the position you need to pass to move() to keep window in its current position.
       This means that the meaning of the returned value varies with window gravity. See move() for more details.

       The reliability of this function depends on the windowing system currently in use. Some windowing systems, such as Wayland,
       do not support a global coordinate system, and thus the position of the window will always be (0, 0). Others, like X11,
       do not have a reliable way to obtain the geometry of the decorations of a window if they are provided by the window manager.
       Additionally, on X11, window manager have been known to mismanage window gravity, which result in windows moving even if
       you use the coordinates of the current position as returned by this function.

       If you haven’t changed the window gravity, its gravity will be Gdk::GRAVITY_NORTH_WEST. This means that get_position()
       gets the position of the top-left corner of the window manager frame for the window. move() sets the position of this same top-left corner.

       If a window has gravity Gdk::GRAVITY_STATIC the window manager frame is not relevant, and thus get_position() will always produce accurate results.
       However you can’t use static gravity to do things like place a window in a corner of the screen, because static gravity ignores the window manager decorations.

       Ideally, this function should return appropriate values if the window has client side decorations, assuming that the windowing system supports global coordinates.

       In practice, saving the window position should not be left to applications, as they lack enough knowledge of the windowing system and the
       window manager state to effectively do so. The appropriate way to implement saving the window position is to use a platform-specific protocol, wherever that is available.

       Parameters
       root_x	Return location for X coordinate of gravity-determined reference point.
       root_y	Return location for Y coordinate of gravity-determined reference point.

     */
    this->set_gravity(Gdk::Gravity::GRAVITY_STATIC);

    // A window to implement a docking bar used for creating the dock panel.
    this->set_skip_taskbar_hint(true);
    this->set_skip_pager_hint(true);
    this->set_type_hint(Gdk::WindowTypeHint::WINDOW_TYPE_HINT_DOCK);
}

/**
 * Initialize
 */
int AppWindow::init()
{
    this->set_title(PACKAGE_NAME);

    // Initialize the monitor geometry.
    this->show_all();
    this->m_screen.init(this);
    g_print("Monitor geometry ready. Monitor count = %d\n",this->m_screen.get_MonitorsCount());

    // Load  configuration file
    Configuration::Load();

    // Seting the docklight logo
    std::string iconFile = Utilities::getExecPath(DEF_LOGONAME);
    this->set_icon_from_file(iconFile);

    // A window to implement a docking bar. Used for creating the dock panel.
    this->set_skip_taskbar_hint(true);
    this->set_skip_pager_hint(true);
    this->set_decorated(false);
    this->set_type_hint(Gdk::WindowTypeHint::WINDOW_TYPE_HINT_DOCK);

    //https://developer.gnome.org/gtk-tutorial/stable/x2431.html
    this->add_events(
            Gdk::PROPERTY_CHANGE_MASK |
            Gdk::STRUCTURE_MASK );

    GdkScreen *screen = gdk_screen_get_default();
    WnckScreen *wnckscreen = wnck_screen_get(0);

    // The monitors-changed signal is emitted when the number,
    // size or position of the monitors attached to the screen change.
    g_signal_connect(G_OBJECT(screen), "monitors-changed", G_CALLBACK(monitor_changed_callback),  (gpointer) this);
    g_signal_connect(G_OBJECT(screen), "size-changed", G_CALLBACK(monitor_size_changed_callback), (gpointer) this);
    g_signal_connect(G_OBJECT(wnckscreen), "window-opened", G_CALLBACK(AppWindow::on_window_opened), NULL);
    g_signal_connect(wnckscreen, "window_closed", G_CALLBACK(AppWindow::on_window_closed), NULL);

    // Set timer handlers
    Glib::signal_timeout().connect(sigc::mem_fun(*this, &AppWindow::fullScreenTimer), 100);
    Glib::signal_timeout().connect(sigc::mem_fun(*this, &AppWindow::autohideTimer), 1000 / 30);

    // Initialize the DockWindow namespace
    if (DockWindow::init(this) != 0) {
        return -1;
    }

    // Create the DockPanel instance.
    m_dockpanel = new DockPanel();
    this->add(*m_dockpanel);

    this->show_all();

    return  m_dockpanel->Init(this);
}

/**
 * Auto hide this window if auto hide property is enabled.
 * @return true
 */
bool AppWindow::autohideTimer()
{
    if (!Configuration::is_autoHide()) {
        return true;
    }
    if (/*!m_animate && */!m_mouseIn && m_visible && m_timerStoped) {
        m_Timer.start();
        m_timerStoped = false;
    }
    if (!m_animate && m_mouseIn && !m_timerStoped) {
        m_timerStoped = true;
    }
    if ((m_mouseIn && !m_visible && !m_isfullscreenSet) /*|| (!m_animate && !m_isfullscreenSet)*/) {
        m_animate = true;
        m_easing_duration = 6.f;
    }
    if ((m_mouseIn && m_visible && !m_timerStoped) /*|| (!m_animate && !m_isfullscreenSet)*/) {
        m_animate = false;
        m_visible = false;
    }
    if (this->m_dockpanel->get_AutohideAllow() && !m_animate && m_visible && !m_mouseIn && abs(m_Timer.elapsed()) > 1.5f) {
        m_Timer.reset();
        m_Timer.stop();
        m_timerStoped = true;
        m_animate = true;
        m_easing_duration = 12.f;
    }

    if (m_animate) {
        auto endTime = m_initTime + m_easing_duration;
        auto now = atime;
        int currentPositionX = 0;
        int currentPositionY = 0;
        float startPosition = 0.f;
        float endPosition = 0.f;

        panel_locationType location = Configuration::get_dockWindowLocation();

        int xpos = 0;
        int ypos = 0;
        int mouseX, mouseY;
        this->get_position(xpos, ypos);

        // set start and  end position according to location
        switch (location)
        {
            case panel_locationType::TOP:
                {
                    // avoid hidden if the mouse is in the margin area
                    int margin = Configuration::get_WindowDockMonitorMargin_Top();
                    if (m_visible && margin > 0 && !this->m_mouseIn) {
                        Utilities::getMousePosition(mouseX, mouseY);
                        if (mouseY <= margin &&
                                mouseX > xpos && mouseX < xpos + this->get_width()) {
                            m_timerStoped = true;
                            return true;
                        }
                    }
                    // set start and end position for hide/show
                    if (m_visible) { // Hide
                        startPosition = margin;
                        endPosition -= this->get_height() - 1;
                    }
                    else { // Show
                        startPosition -= this->get_height() - 1;
                        endPosition = margin;
                    }
                    break;
                }
            case panel_locationType::BOTTOM:
                {
                    // avoid hidden if the mouse is in the margin area
                    int margin = Configuration::get_WindowDockMonitorMargin_Bottom();
                    if (m_visible && margin > 0 && !this->m_mouseIn) {
                        Utilities::getMousePosition(mouseX, mouseY);
                        if (mouseY > DockWindow::Monitor::get_geometry().height - margin
                                && mouseX > xpos && mouseX < xpos + this->get_width()) {
                            m_timerStoped = true;
                            return true;
                        }
                    }
                    // set start and end position for hide/show
                    if (m_visible) { // Hide
                        startPosition = DockWindow::Monitor::get_geometry().height - ( margin + this->get_height() );
                        endPosition = startPosition + this->get_height() + margin - 1;
                    }
                    else { // Show
                        startPosition =  DockWindow::Monitor::get_geometry().height  + this->get_height() + margin - 1;
                        endPosition = DockWindow::Monitor::get_geometry().height - ( margin + this->get_height() );
                    }
                    break;
                }
            case panel_locationType::LEFT:
                {
                    // avoid hidden if the mouse is in the margin area
                    int margin = Configuration::get_WindowDockMonitorMargin_Left();
                    if (m_visible && margin > 0 && !this->m_mouseIn) {
                        Utilities::getMousePosition(mouseX, mouseY);
                        if (mouseX <= margin &&  mouseY > ypos && mouseY < ypos + this->get_height()) {
                            m_timerStoped = true;
                            return true;
                        }
                    }
                    // set start and end position for hide/show
                    if (m_visible) { // Hide
                        startPosition = margin;
                        endPosition -= this->get_width() - 1;
                    }
                    else { // Show
                        startPosition -= this->get_width() - 1;
                        endPosition = margin;
                    }
                    break;
                }
            case panel_locationType::RIGHT:
                {
                    // avoid hidden if the mouse in the margin area
                    int margin = Configuration::get_WindowDockMonitorMargin_Right();
                    if (m_visible && margin > 0 && !this->m_mouseIn) {
                        Utilities::getMousePosition(mouseX, mouseY);
                        if (mouseX > DockWindow::Monitor::get_geometry().width - margin
                            && mouseY > ypos && mouseY < ypos + this->get_height()) {
                            m_timerStoped = true;
                            return true;
                        }
                    }
                    // set start and end position for hide/show
                    if (m_visible) { // Hide
                        startPosition = DockWindow::Monitor::get_geometry().width - ( margin + this->get_width() );
                        endPosition = startPosition + this->get_width() + margin - 1;
                    }
                    else { // Show
                        startPosition =  DockWindow::Monitor::get_geometry().width  + this->get_width() + margin - 1;
                        endPosition = DockWindow::Monitor::get_geometry().width - ( margin + this->get_width() );
                    }
                    break;
                }
        }

        float position = ofxeasing::map_clamp(now,
                m_initTime,
                endTime,
                startPosition,
                endPosition,
                &ofxeasing::linear::easeIn);

        switch (location)
        {
            case panel_locationType::LEFT:
            case panel_locationType::RIGHT:
                currentPositionY = ypos;
                currentPositionX = position;
                break;
            case panel_locationType::TOP:
            case panel_locationType::BOTTOM:
                currentPositionY = position;
                currentPositionX = xpos;
                break;
        }

        //  ease move
        this->move(currentPositionX, currentPositionY);

        if (atime < m_easing_duration) {
            atime++;
        }

        // check the end of animation
        if ((int)position == (int)endPosition) {
            m_initTime = 0;
            atime = 0;
            m_animate = false;
            m_visible = !m_visible;
            this->m_Timer.reset();
            this->m_Timer.stop();

            DockWindow::set_Visibility(this->m_visible);
        }
    }

    return true;
}

/**
 * Hide the window if the active is in full screen mode.
 * @return true
 */
bool AppWindow::fullScreenTimer()
{
    m_isfullscreen = WnckHandler::FullscreenActive();
    if (m_isfullscreen && !m_isfullscreenSet) {
        if (!Configuration::is_autoHide()) {
            Configuration::set_allowDraw(false);
            DockWindow::hide();
            DockWindow::Monitor::removeStrut();
            this->m_visible = false;
        }

        m_isfullscreenSet = true;
        return true;
    }

    if (!m_isfullscreen && m_isfullscreenSet) {

        m_isfullscreenSet = false;
        if (!Configuration::is_autoHide()) {
            Configuration::set_allowDraw(true);
            DockWindow::update(true);//reSize();
            DockWindow::Monitor::updateStrut();
            this->m_visible = true;
        }

        m_isfullscreenSet = false;
    }
    return true;
}

/**
 * Emitted when a new Wnck.Window is opened on screen.
 * @param screen
 * @param window
 * @param data
 */
void AppWindow::on_window_opened(WnckScreen *screen, WnckWindow* window, gpointer data)
{
    DockWindow::update();
    DockPanel::update();
}

/**
 * Emitted when a Wnck.Window is closed on screen.
 * @param screen
 * @param window
 * @param data
 */
void AppWindow::on_window_closed(WnckScreen *screen, WnckWindow *window, gpointer data)
{
    DockWindow::update();
    DockPanel::update();
}

void AppWindow::monitor_size_changed_callback(GdkScreen *screen, gpointer gtkwindow){ }

void AppWindow::on_active_window_changed_callback(WnckScreen *screen,
        WnckWindow *previously_active_window, gpointer user_data){ }

void AppWindow::geometry_changed(WnckWindow *window){ }

void AppWindow::window_opened_callback(WnckScreen* screen, WnckWindow* window, gpointer data){ }

void AppWindow::monitor_changed_callback(GdkScreen *screen, gpointer gtkwindow)
{
    /*
    // Sleep for 2 seconds to give the monitor time to update the geometry
    using namespace std::chrono_literals;
    auto start = std::chrono::high_resolution_clock::now();
    std::this_thread::sleep_for(2s);

*/
    DockWindow::Monitor::removeStrut();
    DockWindow::Monitor::updateGeometry();
    DockWindow::reSize();

    if (Configuration::is_autoHide() == false) {

        DockWindow::Monitor::updateStrut();
    }
}


void AppWindow::window_geometry_changed_callback(WnckWindow *window, gpointer user_data){
}

bool AppWindow::on_enter_notify_event(GdkEventCrossing* crossing_event)
{
    m_mouseIn = true;
    return true;
}

bool AppWindow::on_leave_notify_event(GdkEventCrossing* crossing_event)
{
    m_mouseIn = false;
    return true;
}

AppWindow::~AppWindow()
{
    g_print("AppWindow destroy\n");

}
