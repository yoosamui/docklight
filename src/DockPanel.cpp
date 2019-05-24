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

#include "DockItem.h"
#include "DockPanel.h"
#include "DockWindow.h"
#include "Configuration.h"
#include "DockItemPositions.h"
#include "Launcher.h"
#include "Animations.h"
#include "AppWindow.h"
#include "DockWindow.h"
#include "WnckHandler.h"
#include "About.h"
#include <iostream>
#include <thread>


// static members
int DockPanel::m_currentMoveIndex;
bool DockPanel::m_forceDraw;
Glib::RefPtr<Gdk::Pixbuf> DockPanel::m_AppRunImage;
bool DockPanel::m_AppThreadRunning;
guint DockPanel::m_widthDecrement;
guint DockPanel::m_heightDecrement;

guint DockPanel::m_ItemsWidth;
guint DockPanel::m_ItemsHeight;


DockPreview* DockPanel::m_dockPreview;
AppUpdater*  DockPanel::m_AppUpdater;
int DockPanel::m_previewIndex;

/**
 * This class is the main dock renderer.
 */
DockPanel::DockPanel():
    m_homeiconFilePath(Utilities::getExecPath(DEF_ICONNAME)),
    m_separatorFilePath(Utilities::getExecPath(DEF_SEPARATOR))

{
    DockPanel::m_AppUpdater = nullptr;
    DockPanel::m_dockPreview = nullptr;
    DockPanel::m_previewIndex = -1;

    // Set event masks
    add_events(Gdk::BUTTON_PRESS_MASK |
            Gdk::BUTTON_RELEASE_MASK |
            Gdk::SCROLL_MASK |
            Gdk::SMOOTH_SCROLL_MASK |
            Gdk::POINTER_MOTION_HINT_MASK |
            Gdk::FOCUS_CHANGE_MASK |
            Gdk::ENTER_NOTIFY_MASK |
            Gdk::LEAVE_NOTIFY_MASK |
            Gdk::POINTER_MOTION_MASK);

    // set the static member to the default values
    DockPanel::m_currentMoveIndex = -1;
    DockPanel::m_forceDraw = false;
    DockPanel::m_AppThreadRunning = true;
    DockPanel::m_widthDecrement = 0;
    DockPanel::m_heightDecrement = 0;

    // Gets the default WnckScreen on the default display.
    WnckScreen *wnckscreen = wnck_screen_get_default();

    // set the required signals handlers
    g_signal_connect(G_OBJECT(wnckscreen), "window-opened", G_CALLBACK(DockPanel::on_window_opened), NULL);
    g_signal_connect(wnckscreen, "window_closed", G_CALLBACK(DockPanel::on_window_closed), NULL);
    g_signal_connect(wnckscreen, "active_window_changed", G_CALLBACK(DockPanel::on_active_window_changed_callback), NULL);
    Glib::signal_timeout().connect(sigc::mem_fun(*this, &DockPanel::on_timeoutDraw),1000 / 8);

    // Menus
    m_HomeMenu.attach_to_widget(*this);
    m_HomeMenu.accelerate(*this);
    m_HomeMenu.signal_hide().connect(sigc::mem_fun(*this, &DockPanel::on_HideMenu_event));

    m_AutohideMenuItem.set_active(Configuration::is_autoHide());
    m_AutohideMenuItem.signal_toggled().connect(sigc::mem_fun(*this, &DockPanel::on_AutohideToggled_event));
    m_HomeUnMinimizeAllWindowsMenuItem.signal_activate().connect(sigc::mem_fun(*this, &DockPanel::on_HomeUnMinimizeAllWindows_event));
    m_HomeMinimizeAllWindowsExceptActiveMenuItem.signal_activate().connect(sigc::mem_fun(*this, &DockPanel::on_HomeMinimizeAllWindowsExceptActive_event));
    m_HomeMinimizeAllWindowsMenuItem.signal_activate().connect(sigc::mem_fun(*this, &DockPanel::on_HomeMinimizeAllWindows_event));
    m_HomeCloseAllWindowsExceptActiveMenuItem.signal_activate().connect(sigc::mem_fun(*this, &DockPanel::on_HomeCloseAllWindowsExceptActive_event));
    m_HomeCloseAllWindowsMenuItem.signal_activate().connect(sigc::mem_fun(*this, &DockPanel::on_HomeCloseAllWindows_event));
    m_HelpMenuItem.signal_activate().connect(sigc::mem_fun(*this, &DockPanel::on_HelpMenu_event));
    m_homeSessionGrp.signal_activate().connect(sigc::mem_fun(*this, &DockPanel::on_HomeAddSessionGrp_event));
    m_preferencesMenuItem.signal_activate().connect(sigc::mem_fun(*this, &DockPanel::on_HomePreferences_event));
    m_QuitMenuItem.signal_activate().connect(sigc::mem_fun(*this, &DockPanel::on_QuitMenu_event));
    m_HomeAddSeparatotMenuItem.signal_activate().connect(sigc::mem_fun(*this, &DockPanel::on_AddSeparator_event));
    m_AboutMenuItem.signal_activate().connect(sigc::mem_fun(*this, &DockPanel::on_AboutMenu_event));


    m_ItemMenu.attach_to_widget(*this);
    m_ItemMenu.accelerate(*this);
    m_ItemMenu.signal_hide().connect(sigc::mem_fun(*this, &DockPanel::on_HideMenu_event));
    m_MenuItemNewApp.signal_activate().connect(sigc::mem_fun(*this, &DockPanel::on_NewMenu_event));
    m_MenuItemAttach.signal_activate().connect(sigc::mem_fun(*this, &DockPanel::on_AttachMenu_event));
    m_MenuItemDetach.signal_activate().connect(sigc::mem_fun(*this, &DockPanel::on_DettachMenu_event));
    m_MenuItemCloseAll.signal_activate().connect(sigc::mem_fun(*this, &DockPanel::on_CloseAll_event));
    m_MenuItemCloseAllExceptActive.signal_activate().connect(sigc::mem_fun(*this, &DockPanel::on_CloseAllExceptActive_event));
    m_MenuItemMinimizedAll.signal_activate().connect(sigc::mem_fun(*this, &DockPanel::on_MinimieAll_event));
    m_MenuItemMinimizedAllExceptActive.signal_activate().connect(sigc::mem_fun(*this, &DockPanel::on_MinimieAllExceptActive_event));
    m_MenuItemUnMinimizedAll.signal_activate().connect(sigc::mem_fun(*this, &DockPanel::on_UnMinimieAll_event));

}

/**
 * preInit load the attached icons and initializes variables.
 * @param Gtk::Window*  window
 * @return return 0 is success or -1 is an error found
 */
int DockPanel::Init(Gtk::Window* window)
{
    m_AppWindow = window;
    m_AppUpdater = new AppUpdater();


    const char* filename = m_homeiconFilePath.c_str();
    DockItem* dockItem = new DockItem();
    try {
        int iconsize = Configuration::get_CellWidth() - ICON_CELL_WIDTH_MARGIN;
        dockItem->m_image = Gdk::Pixbuf::create_from_file(filename, iconsize, iconsize, true);
    }
    catch (Glib::FileError fex) {
        g_critical("preInit: file %s could not be found!\n", filename);
        return -1;

    }
    catch (Gdk::PixbufError bex) {
        g_critical("preInit: file %s PixbufError!\n", filename);
        return -1;
    }

    // add the home icon
    dockItem->m_appname = _("Desktop");
    dockItem->m_realgroupname = _("Desktop");
    dockItem->m_dockitemtype = DockItemType::SingleDock;
    dockItem->m_index = 0;
    m_AppUpdater->m_dockitems.insert(this->m_AppUpdater->m_dockitems.begin(), dockItem);

    // Start the background thread for application start animation
    m_AppRunThreadLauncher = new std::thread(AppRunAnimation);

    return 0;
}

/*
 *  destructor
 */
DockPanel::~DockPanel()
{
    // tell the background thread to terminate.
    m_AppThreadRunning = false;

    // Detach
    m_AppRunThreadLauncher->detach();

    // free memory
    delete m_AppRunThreadLauncher;

    // pointed dangling to ptr NULL
    m_AppRunThreadLauncher = NULL;

    delete m_AppUpdater;
    m_AppUpdater = NULL;

    g_print("DockPanel destroy.\n");
}

/**
 * This method will be call from the background tread to manage the application image animation.
 * The animation consists in invert the colors of the image.
 */
void DockPanel::AppRunAnimation()
{
    gint x, y, i;
    while(m_AppThreadRunning){
        if (m_AppRunImage && m_AppRunImage->get_colorspace() == Gdk::COLORSPACE_RGB && m_AppRunImage->get_bits_per_sample() == 8){
            int w = m_AppRunImage->get_width();
            int h = m_AppRunImage->get_height();
            int channels = m_AppRunImage->get_n_channels();
            gint rowstride = m_AppRunImage->get_rowstride();
            gint pixel_offset;
            for (i = 0; i < 4; i++) {
                for (y = 0; y < h; y++) {
                    for (x = 0; x < w; x++) {
                        pixel_offset = y * rowstride + x * channels;
                        guchar* pixel = &m_AppRunImage->get_pixels()[pixel_offset];

                        pixel[0] = 255 - pixel[0];
                        pixel[1] = 255 - pixel[1];
                        pixel[2] = 255 - pixel[2];
                    }
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
            m_AppRunImage = NULLPB;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
}

/**
 * Emitted when a new Wnck.Window is opened on screen.
 * @param screen
 * @param window
 * @param data
 */
void DockPanel::on_window_opened(WnckScreen *screen, WnckWindow* window, gpointer data)
{
    update();
}

/**
 * Emitted when a Wnck.Window is closed on screen.
 * @param screen
 * @param window
 * @param data
 */
void DockPanel::on_window_closed(WnckScreen *screen, WnckWindow *window, gpointer data)
{
    update();
}

/**
 * Emitted when the active window on screen has changed.
 * @param screen
 * @param previously_active_window
 * @param user_data
 */
void DockPanel::on_active_window_changed_callback(WnckScreen *screen, WnckWindow *previously_active_window, gpointer user_data)
{
}

/**
 * Emitted when the menu is clicked.
 */
void DockPanel::on_NewMenu_event()
{
    DockItem* item = this->get_CurrentItem();
    if (item == nullptr)
        return;

    if (item->m_dockitemSesssionGrpId > 0) {
        //  createSessionWindow();
        return;

    }

    if (!Launcher::Launch(item->m_realgroupname)) {

        //   createLauncher(item);
    }

    // start the animation and launch the application
    m_AppRunImage = item->m_image;
    this->update();
}

/**
 * Close application and release resources.
 */
void DockPanel::on_QuitMenu_event()
{
    m_AppWindow->close();
}

/**
 * Emitted when the menu hides.
 */
void DockPanel::on_HideMenu_event()
{
    m_popupMenuOn = false;
}

/**
 *Emitted when the menu Attach to Dock is selected.
 */
void DockPanel::on_AttachMenu_event()
{
    m_AppUpdater->AttachItem(this->m_currentMoveIndex);
}

void DockPanel::on_DettachMenu_event()
{
    if (!m_AppUpdater->DettachItem(this->m_currentMoveIndex)){
        return;
    }

    this->update();

    // force resize the dock window.
    DockWindow::update();

}
/**
 * tonggle utohide state
 */
void DockPanel::on_AutohideToggled_event()
{
    // discover the new state.
    bool autohide = m_AutohideMenuItem.get_active();
    Configuration::set_autoHide(autohide);
}

/**
 * Unminimize all
 */
void DockPanel::on_HomeUnMinimizeAllWindows_event()
{
    WnckHandler::HomeUnMinimizeAll();
}


/**
 * Minimize all except active
 */
void DockPanel::on_HomeMinimizeAllWindowsExceptActive_event()
{

    WnckHandler::HomeMinimizeAllExceptActive();
}

/**
 * Minimize all windows
 */
void DockPanel::on_HomeMinimizeAllWindows_event()
{
    WnckHandler::HomeMinimizeAll();
}

/**
 * Close all windows except the active
 */
void DockPanel::on_HomeCloseAllWindowsExceptActive_event()
{
    WnckHandler::HomeCloseAllExceptActive();
}

/**
 * Close all windows
 */
void DockPanel::on_HomeCloseAllWindows_event()
{
    WnckHandler::HomeCloseAllWindows();
}

void DockPanel::on_HomePreferences_event()
{

    //   createPreferences();
}

/**
 * Add a separator at the end of the list
 */
void DockPanel::on_AddSeparator_event()
{
    const char* filename = m_separatorFilePath.c_str();
    DockItem* dockItem = new DockItem(8, Configuration::get_CellHeight());

    try {
        int iconsize = Configuration::get_CellWidth() - ICON_CELL_WIDTH_MARGIN;
        dockItem->m_image = Gdk::Pixbuf::create_from_file(filename, iconsize, iconsize, true);
    }
    catch (Glib::FileError fex) {
        g_critical("on_AddSeparator: file %s could not be found!\n", filename);
        return;

    }
    catch (Gdk::PixbufError bex) {
        g_critical("on_AddSeparator: file %s PixbufError!\n", filename);
        return;
    }

    dockItem->m_dockitemtype = DockItemType::Separator;
    dockItem->m_isAttached = true;

    m_AppUpdater->m_dockitems.push_back(dockItem);
    DockWindow::update();
}

void DockPanel::on_HomeAddSessionGrp_event()
{

    //   CreateSessionDockItemGrp();
}

/**
 * show the github web page
 */
void DockPanel::on_HelpMenu_event()
{
    Utilities::system("xdg-open https://github.com/yoosamui/DockLight/wiki");
}

void DockPanel::on_AboutMenu_event()
{
    m_about.show(m_AppWindow);
}

/**
 * Unminimize all
 */
void DockPanel::on_UnMinimieAll_event()
{
    DockItem * dockitem = this->get_CurrentItem();
    if(dockitem == nullptr){
        return;
    }

    WnckHandler::unMinimizeAllByDockItem(dockitem);
}

/**
 * Minimize all except active
 */
void DockPanel::on_MinimieAllExceptActive_event()
{
    DockItem * dockitem = this->get_CurrentItem();
    if(dockitem == nullptr){
        return;
    }


        auto window = dockitem->m_items[0]->m_window;
        wnck_window_close(window, gtk_get_current_event_time());


//    WnckHandler::minimizeAllExceptActiveByDockItem(dockitem);
}

/**
 * Minimize all
 */
void DockPanel::on_MinimieAll_event()
{
    DockItem * dockitem = this->get_CurrentItem();
    if(dockitem == nullptr){
        return;
    }
    WnckHandler::minimizeAllByDockItem(dockitem);
}

/**
 * Closse all except active
 */
void DockPanel::on_CloseAllExceptActive_event()
{
    DockItem * dockitem = this->get_CurrentItem();
    if(dockitem == nullptr){
        return;
    }

    WnckHandler::closeAllExceptActiveByDockItem(dockitem);
}

/**
 * Close all
 */
void DockPanel::on_CloseAll_event()
{
    DockItem * dockitem = this->get_CurrentItem();
    if(dockitem == nullptr){
        return;
    }

    WnckHandler::closeAllByDockItem(dockitem);
}

/**
 * bool DockPanel::on_button_press_event(GdkEventButton *event)
 *
 * handles Mouse button press : process mouse button event true to stop other handlers from being invoked for the event.
 * false to propagate the event further.
 */
bool DockPanel::on_button_press_event(GdkEventButton *event)
{

    if ((event->type == GDK_BUTTON_PRESS)) {
        m_mouseclickEventTime = gtk_get_current_event_time();

        // Check if the event is a left button click.
        if (event->button == 1 && !m_mouseLeftButtonDown) {
            m_mouseLeftButtonDown = true;

            // start drag & drop timer
            if (m_currentMoveIndex > 0) {
                m_dragdropMousePoint.set_x((int) event->x);
                m_dragdropMousePoint.set_y((int) event->y);
                m_dragdropTimer.start();
            }
        }

        // Check if the event is a right button click.
        if (event->button == 3 && !m_mouseRightButtonDown) {
            m_mouseRightButtonDown = true;
            // The event has been handled.

            // Items
            if (m_currentMoveIndex > 0) {
                //DockItem *dockitem = m_AppUpdater->m_dockitems[m_currentMoveIndex];
                //dockitem->m_isAttached = true;
                //dockitem->m_attachedIndex = m_currentMoveIndex;
                //m_AppUpdater->Save();
            }

            return true;
        }
    }
    return false;
}

/**
 * Check if a drop has been made.
 */
void DockPanel::DragDropEnds()
{
    // Check if a item was drop
    if (m_DragDropBegin){
        m_AppUpdater->MoveItem(0);
        m_AppUpdater->Reindex();

        // set as attached and save it
        if (m_DragDropSourceIndex != this->m_currentMoveIndex){
            m_dragDropItem->m_isAttached = true;
            m_AppUpdater->Save();
        }

        // reset the pointer
        m_dragDropItem = nullptr;
        m_DragDropBegin = false;
    }
}

/**
 * Returning TRUE means we handled the event, so the signal emission should be stopped (don’t call any further callbacks
 * that may be connected). Return FALSE to continue invoking callbacks. handles Mouse button released : process mouse button event
 * true to stop other handlers from being invoked for the event. false to propagate the event further.
 */
bool DockPanel::on_button_release_event(GdkEventButton *event)
{

    this->DragDropEnds();

    // Taking Too Long To Release the mouse.  // That is not a valid Click.
    if ((gtk_get_current_event_time() - m_mouseclickEventTime) > 200) {
        m_mouseLeftButtonDown = false;
        m_mouseRightButtonDown = false;
        return true;
    }

    if (!m_mouseIn)
        return true;

    if (m_mouseLeftButtonDown) {
        m_mouseLeftButtonDown = false;
        this->ExecuteApp(event);
        //  SelectWindow(m_currentMoveIndex, event);
        return TRUE;
    }

    // Right mouse click
    if (m_mouseRightButtonDown) {
        // m_preview.hideMe();
        //  m_mouseRightClick = true;
        m_mouseRightButtonDown = false;

        // Menus
        if (m_currentMoveIndex == 0) {
            m_HomeMenu.popup(sigc::mem_fun(*this, &DockPanel::on_popup_homemenu_position), 1, event->time);
            m_popupMenuOn = true;
        }
        else  if (m_currentMoveIndex > 0) {
            m_ItemMenu.popup(sigc::mem_fun(*this,&DockPanel::on_popup_itemmenu_position), 1, event->time);
            m_popupMenuOn = true;
        }

        //Home
        if (m_currentMoveIndex == 0) {

            int wincount = WnckHandler::windowscount();
            int minimizedexitst = WnckHandler::isExistsMinimizedWindows();
            int unminimized = WnckHandler::unMinimizedWindowsCount();

            m_HomeCloseAllWindowsMenuItem.set_sensitive(wincount > 0);
            m_HomeCloseAllWindowsExceptActiveMenuItem.set_sensitive(wincount > 1);
            m_HomeMinimizeAllWindowsMenuItem.set_sensitive(unminimized > 0);
            m_HomeUnMinimizeAllWindowsMenuItem.set_sensitive(wincount > 0);
            m_HomeMinimizeAllWindowsExceptActiveMenuItem.set_sensitive(unminimized > 1);

            return true;
        }

        // items
        if (m_currentMoveIndex > 0) {
            DockItem *item = this->get_CurrentItem();
            if(item != nullptr ){
                bool maximizedexistst = WnckHandler::isExistsUnMaximizedWindowsByDockItem(item);
                bool isExitstActiveWindow = WnckHandler::isExitsActivetWindowByDockItem(item);
                int isExitstWindows =  WnckHandler::isExitstWindowsByDockItem(item);

                m_MenuItemAttach.set_sensitive(item->m_isAttached == false);
                m_MenuItemDetach.set_sensitive(item->m_isAttached /*&& item->m_items.size() == 0*/);
                m_MenuItemMinimizedAll.set_sensitive(isExitstWindows > 0 && maximizedexistst);
                m_MenuItemMinimizedAllExceptActive.set_sensitive(isExitstWindows > 1  && maximizedexistst && isExitstActiveWindow);
                m_MenuItemUnMinimizedAll.set_sensitive(isExitstWindows);
                m_MenuItemCloseAllExceptActive.set_sensitive(isExitstWindows > 1 && isExitstActiveWindow);
                m_MenuItemCloseAll.set_sensitive(isExitstWindows);

            }
        }
    }

    return TRUE;
}
/**
 * The mouse scroll event. activate the next and or current window.
 */
bool DockPanel::on_scroll_event(GdkEventScroll* e)
{
    int index = m_currentMoveIndex;
    if (index == -1 || index == 0)
        return true;

    DockItem* item = m_AppUpdater->m_dockitems[index]->get_next();
    if (item == nullptr)
        return true;

    WnckWindow* window = item->m_window;
    if ((int)e->delta_y == (int)1) {
        WnckHandler::ActivateWindow(window);
    }
    else if ((int)e->delta_y == (int) - 1) {
        WnckHandler::ActivateWindow(window);
    }

    // Event has been handled
    return true;
}

/**
 * Handle the home menu popup position.
 * @param x
 * @param y
 * @param push_in
 */
void DockPanel::on_popup_homemenu_position(int& x, int& y, bool& push_in)
{

    DockItemPositions::get_CenterPosition(m_currentMoveIndex, x, y, m_HomeMenu.get_width(), m_HomeMenu.get_height());
}

void DockPanel::on_popup_itemmenu_position(int& x, int& y, bool& push_in)
{
    DockItemPositions::get_CenterPosition(m_currentMoveIndex, x, y, m_ItemMenu.get_width(), m_ItemMenu.get_height());
}

void DockPanel::ExecuteApp(GdkEventButton* event)
{
    DockItem* item = this->get_CurrentItem();
    if (item == nullptr){
        return;
    }

    if (item->m_dockitemtype == DockItemType::Separator) {
        return;
    }

    if (item->m_items.size() == 0) {
        on_NewMenu_event();
        return;
    }

    if (item->m_items.size() == 1) {
        WnckWindow *window = nullptr;
        window = WnckHandler::get_ActiveWindowIfAny(item);
        if (window == nullptr) {
            DockItem* firstChild = item->m_items[0];
            window = firstChild->m_window;
        }

        WnckHandler::ActivateWindow(window);
        return;
    }

    // check if preview already open.
    if( m_previewIndex == m_currentMoveIndex && this->m_dockPreview){
        return;
    }

    // skeep home
    if (m_previewIndex == 0){
        return;
    }

    this->PreviewClose();

    m_dockPreview = new DockPreview();
    m_previewIndex = this->m_currentMoveIndex;
    m_dockPreview->Show(item->m_items, this->m_currentMoveIndex, AppUpdater::m_dockitems[0]->get_height());

    m_dockPreview->show();
}

/**
 * Gets the current dock Item.
 */
inline DockItem* DockPanel::get_CurrentItem()
{
    if (m_currentMoveIndex < 0 || m_currentMoveIndex > m_AppUpdater->m_dockitems.size()) {
        return nullptr;
    }

    return m_AppUpdater->m_dockitems[m_currentMoveIndex];
}



bool DockPanel::get_AutohideAllow()
{
    return m_popupMenuOn == false && this->m_dockPreview == nullptr;
}

/**
 * Force redraw.
 */
void DockPanel::update()
{
    m_forceDraw = true;
}

/**
 * Run at defined FRAMERATE
 */
bool DockPanel::on_timeoutDraw()
{
    // initiate the drag
    if (!m_DragDropBegin && m_currentMoveIndex > 0 &&  m_mouseLeftButtonDown && m_dragdropTimer.elapsed() >= 0.25 ){
        m_dragdropTimer.stop();
        m_dragdropTimer.reset();

        m_dragDropItem =  this->get_CurrentItem();
        if ( m_dragDropItem != nullptr){

            m_AppRunImage = m_dragDropItem->m_image;
            m_DragDropBegin = true;
            m_DragDropSourceIndex = this->m_currentMoveIndex;
        }
    }

    if (m_mouseIn || m_forceDraw || m_AppRunImage) {
        Gtk::Widget::queue_draw();
        m_forceDraw = false;
    }
    return true;
}

/**
 * A
 * calculate the item index from mouse coordinates.
 * @param x
 * @param y
 * @return the item index or -1 if the item could not be found.
 */
inline int DockPanel::get_Index(const int& mouseX, const int& mouseY)
{
    Gdk::Point mouse(mouseX, mouseY);
    int idx = 0;
    int x = DockWindow::get_dockWindowStartEndMargin() / 2;
    int y = x;

    if (DockWindow::is_Horizontal()) {
        for (auto item : m_AppUpdater->m_dockitems) {
            if (mouse.get_x() >= x && mouse.get_x() <= x + item->get_width()) {
                return idx;
            }

            x += item->get_width() + Configuration::get_separatorMargin();
            idx++;
        }
    }
    else
    {
        int height;
        for (DockItem* item : m_AppUpdater->m_dockitems) {
            height = item->m_dockitemtype == DockItemType::Separator ? item->get_width() : item->get_height();

            if (mouse.get_y() >= y && mouse.get_y() <= y + height) {
                return idx;
            }

            y += height + Configuration::get_separatorMargin();
            idx++;
        }
    }

    return -1;
}

/**
 * handles Mouse motion: process mouse button event
 * true to stop other handlers from being invoked for the event.
 * false to propagate the event further.
 * @param event
 * @return false
 */
bool DockPanel::on_motion_notify_event(GdkEventMotion * event)
{
    m_currentMoveIndex = this->get_Index(event->x, event->y);

    if(m_DragDropBegin){

        m_AppUpdater->MoveItem(this->m_currentMoveIndex);
        m_dragdropMousePoint.set_x((int) event->x);
        m_dragdropMousePoint.set_y((int) event->y);


    }

    if(DockPanel::m_previewIndex != m_currentMoveIndex && DockPanel::m_dockPreview != nullptr){
        DockPanel::PreviewClose();
    }

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



bool DockPanel::on_enter_notify_event(GdkEventCrossing * crossing_event)
{
    //  g_print("Mp_IN\n");
    m_mouseIn = true;

    m_HomeMenu.hide();
    m_ItemMenu.hide();
    //    Gtk::Menu::signal_hide(
    return true;
}

bool DockPanel::on_leave_notify_event(GdkEventCrossing * crossing_event)
{
    // g_print("Mp_OUT\n");
    m_mouseIn = false;

    m_titlewindow.hide();
    m_infowindow.hide();

    return true;
}

/**
 * Calculate the draw position for all items.
 */
inline void DockPanel::get_ItemPosition(const DockItemType dockType, int& x, int& y, int& width, int& height)
{
    static int nextsize = 0;

    if (DockWindow::is_Horizontal())
    {
        if (x == 0 ) {
            y = (DockWindow::get_DockWindowHeight() / 2) - Configuration::get_CellHeight() / 2;
            x = DockWindow::get_dockWindowStartEndMargin() / 2;
            nextsize = width;
            return;
        }

        // if the item is a separator the width is probably not equal.
        // in this case wie remeber the size for use it in the next item.
        if( dockType == DockItemType::Separator){
            x +=  nextsize + Configuration::get_separatorMargin();
            nextsize = width;
            return;
        }

        x +=  nextsize + Configuration::get_separatorMargin();
        nextsize = width;
    }
    else
    {
        if (y == 0 ){
            x = (DockWindow::get_DockWindowWidth() / 2) - Configuration::get_CellWidth() / 2;
            y = DockWindow::get_dockWindowStartEndMargin() / 2;
            nextsize = height;
            return;
        }

        // if the item is a separator the wisth is probably not equal.
        // in this case wie remeber the size for use it in the next item.
        if( dockType == DockItemType::Separator){
            y +=  nextsize + Configuration::get_separatorMargin();
            height = nextsize = width;
            width = Configuration::get_CellWidth();
            return;
        }

        y +=  nextsize + Configuration::get_separatorMargin();
        nextsize = height;
    }
}
/**
 * The cairo main renderer
 * @param the cairo contex.
 */
bool DockPanel::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{


    if (Configuration::is_allowDraw() == false) {
        return true;
    }

    this->draw_Panel(cr);
    this->draw_Items(cr);
    this->show_Title();

    return true;
}

/**
 * Draw the panel in all locations
 * @param cr cairo context
 * @param x current position x
 * @param y current position y
 */
void DockPanel::draw_Panel(const Cairo::RefPtr<Cairo::Context>& cr)
{
        cr->set_source_rgba(
            m_Theme.Panel().Fill().Color::red,
            m_Theme.Panel().Fill().Color::green,
            m_Theme.Panel().Fill().Color::blue,
            m_Theme.Panel().Fill().Color::alpha);

    if (Configuration::is_panelMode()) {
        cr->paint();
    }
    else {

        RoundedRectangle(cr, 0, 0, DockWindow::get_DockWindowWidth(), DockWindow::get_DockWindowHeight(), m_Theme.Panel().Ratio());
        cr->fill();

        cr->set_source_rgba(
            m_Theme.Panel().Stroke().Color::red,
            m_Theme.Panel().Stroke().Color::green,
            m_Theme.Panel().Stroke().Color::blue,
            m_Theme.Panel().Stroke().Color::alpha);
        RoundedRectangle(cr, 0, 0, DockWindow::get_DockWindowWidth(), DockWindow::get_DockWindowHeight(), m_Theme.Panel().Ratio());
        cr->stroke();
    }
}

/**
 * Render all dock items
 * @param cr cairo context
 */
void DockPanel::draw_Items(const Cairo::RefPtr<Cairo::Context>& cr)
{
    static guint widthDecrement = 0;
    static guint heightDecrement = 0;

    int idx = 0;
    int center = 0;
    int iconsizeWidth = 0;
    int iconsizeHeight = 0;

    int y = 0;
    int x = 0;
    int width = 0;
    int height = 0;

    int itemsCount = m_AppUpdater->m_dockitems.size();

    m_widthDecrement = 0;
    m_heightDecrement = 0;

    m_ItemsWidth = 0;
    m_ItemsHeight = 0;

    if (DockWindow::is_Horizontal()){

        m_widthDecrement = DockItemPositions::get_ResizeWidthDecrement();
        if (widthDecrement != m_widthDecrement){

            widthDecrement = m_widthDecrement;
            DockWindow::update();
        }

    }else{

        m_heightDecrement = DockItemPositions::get_ResizeHeightDecrement();
        if (heightDecrement != m_heightDecrement){

            heightDecrement = m_heightDecrement;
            DockWindow::update();
        }
    }



    // Draw all items with cairo
    for (idx = 0; idx < itemsCount; idx++) {

        DockItem* item = m_AppUpdater->m_dockitems[idx];
        width = item->get_width();
        height = item->get_height();

        this->get_ItemPosition(item->m_dockitemtype, x, y, width, height);
        // y += height + Configuration::get_separatorMargin();
        //g_print("idx: %d y:%d  h:%d  sepa: %d \n",idx,y, height, Configuration::get_separatorMargin());

        item->m_index = idx;

        item->m_posX = x;
        item->m_posY = y;

        m_ItemsWidth  += width;
        m_ItemsHeight += height;

        // Draw cells
        if (item->m_dockitemtype != DockItemType::Separator ){

            cr->set_source_rgba(
            m_Theme.PanelCell().Fill().Color::red,
            m_Theme.PanelCell().Fill().Color::green,
            m_Theme.PanelCell().Fill().Color::blue,
            m_Theme.PanelCell().Fill().Color::alpha);

            RoundedRectangle(cr, x, y, width, height, m_Theme.PanelCell().Ratio());
            cr->fill();
            cr->set_source_rgba(
            m_Theme.PanelCell().Stroke().Color::red,
            m_Theme.PanelCell().Stroke().Color::green,
            m_Theme.PanelCell().Stroke().Color::blue,
            m_Theme.PanelCell().Stroke().Color::alpha);
            cr->set_line_width(m_Theme.PanelCell().LineWidth());

            if (m_Theme.PanelCell().Mask() == 0){
                RoundedRectangle(cr, x, y, width, height, m_Theme.PanelCell().Ratio());
            }
            else{
                // bottom
                cr->move_to(x, y + height);
                cr->line_to(x, y + height);
                cr->line_to(x + width, y + height);
            }
            cr->stroke();
        }

        // Draw dots and icons if the item is not a user separator
        if (item->m_dockitemtype != DockItemType::Separator) {
            // Dots
            center = (width / 2);
            if (item->m_items.size() > 0) {
                cr->set_source_rgb(1.0, 1.0, 1.0);
                if (item->m_items.size() == 1) {
                    cr->arc(x + center, y + height - 4, 1.7, 0, 2 * M_PI);
                }
                else if (item->m_items.size() > 1) {
                    cr->arc(x + center - 4, y + height - 4, 1.7, 0, 2 * M_PI);
                    cr->arc(x + center + 4, y + height - 4, 1.7, 0, 2 * M_PI);
                }
                cr->fill();
            }


            // icons
            if (item->m_image != NULLPB) {
                iconsizeWidth = width - CELL_MARGIN;
                iconsizeHeight = height - CELL_MARGIN;
                auto icon = item->m_image->scale_simple(iconsizeWidth, iconsizeHeight, Gdk::INTERP_BILINEAR);
                Gdk::Cairo::set_source_pixbuf(cr, icon, x + center - iconsizeWidth / 2, y + 1);
                cr->paint();
            }

        }

        // Selector
        if (idx == m_currentMoveIndex && (item->m_dockitemtype != DockItemType::Separator || m_DragDropBegin )) {

            cr->set_source_rgba(
            m_Theme.Selector().Fill().Color::red,
            m_Theme.Selector().Fill().Color::green,
            m_Theme.Selector().Fill().Color::blue,
            m_Theme.Selector().Fill().Color::alpha);

            RoundedRectangle(cr, x, y, width, height, m_Theme.Selector().Ratio());
            cr->fill();

            cr->set_source_rgba(
            m_Theme.Selector().Stroke().Color::red,
            m_Theme.Selector().Stroke().Color::green,
            m_Theme.Selector().Stroke().Color::blue,
            m_Theme.Selector().Stroke().Color::alpha);
            cr->set_line_width(m_Theme.Selector().LineWidth());
            RoundedRectangle(cr, x, y, width, height, m_Theme.Selector().Ratio());
            cr->stroke();

        }
    }
}

/**
 * Show the application title.
 */
void DockPanel::show_Title()
{
    if (Configuration::is_autoHide() && !DockWindow::is_Visible() || !m_mouseIn || m_dockPreview != nullptr){

        m_titlewindow.hide();
        m_infowindow.hide();
        m_titleShow = false;
        return;
    }


    DockItem* item = this->get_CurrentItem();
    if (item == nullptr){
        m_titlewindow.hide();
        m_infowindow.hide();
        m_titleShow = false;
        return;
    }

    // title window
    if (m_titleItemOldindex != m_currentMoveIndex) {

        m_titleItemOldindex = m_currentMoveIndex;
        m_titleElapsedSeconds = 0;
        m_titleTimer.start();
        m_titleShow = false;

        m_titlewindow.hide();
        m_infowindow.hide();
        //            if (m_previewWindowActive)
        //                m_preview.hideMe();
        //
        //
        return;
    }


    if (m_titleItemOldindex == m_currentMoveIndex && m_currentMoveIndex != -1 &&
            DockWindow::is_Visible() && item->m_dockitemtype != DockItemType::Separator ) {
        if (m_titleElapsedSeconds > 0.3 && m_titleShow == false /* && !m_previewWindowActive*/) {

            std::string title = item->get_title();
            if (item->m_items.size() > 1) {
                char buff[NAME_MAX];
                sprintf(buff, "%s (%d)", title.c_str(), (int)item->m_items.size());
                title = buff;
            }

            m_titlewindow.setText(title);

            int x, y;
            int height = m_titlewindow.get_height();
            int width = m_titlewindow.get_width();

            DockItemPositions::get_CenterPosition(m_currentMoveIndex, x, y, width, height);
            m_titlewindow.move(x, y);
            m_titleShow = true;
        }

        m_titleElapsedSeconds = m_titleTimer.elapsed();
    }
}


// REFERENCES
// https://developer.gnome.org/gtkmm-tutorial/stable/index.html.en
