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

#ifndef DOCKPANEL_H
#define	DOCKPANEL_H

#define WNCK_I_KNOW_THIS_IS_UNSTABLE  1

#include <libwnck/libwnck.h>
#include <glibmm/timer.h>
#include <gtkmm.h>

#include "Configuration.h"
#include "DockItem.h"
#include "LauncherWindow.h"
#include "AppUpdater.h"
#include "TitleWindow.h"
#include "DockMenu.h"
#include "DockPreview.h"
#include "About.h"
#include <thread>
class DockPanel : public Gtk::DrawingArea, DockMenu {

    public:

        static guint m_ItemsWidth;
        static guint m_ItemsHeight;

        DockPanel();
        virtual ~DockPanel();
        //SessionWindow* m_sessionWindow;
        int Init(Gtk::Window* window);
        static void update();
        bool get_AutohideAllow();
        static int get_CurrentIndex() {
            return m_currentMoveIndex;
        };

        static guint get_WidthDecrement(){
            return m_widthDecrement;
        }

        static guint get_HeightDecrement(){
            return m_heightDecrement;
        }

        static void PreviewClose();

        static AppUpdater& get_AppUpdater()  {
            return *m_AppUpdater;
        }

        static bool is_mouseIn(){
            return m_mouseIn;
        }
    private:
    static Glib::RefPtr<Gdk::Pixbuf> m_AnimationImage;
    guint m_animationCounter = 0;
    bool m_animationStart = false;
    guint m_animationEndValue = 1;
    Configuration::Style::Theme m_Theme = Configuration::get_Theme();
        static int m_previewIndex ;
        LauncherWindow* m_launcherWindow = nullptr;
        About m_about;
        static guint m_widthDecrement;
        static guint m_heightDecrement;

        static void set_SelectorForActiveWindow(WnckScreen* screen);
        Gtk::Window* m_AppWindow = nullptr;
        static DockPreview* m_dockPreview;
        static bool m_popupMenuOn;
        static bool m_forceDraw;

        TitleWindow m_titlewindow;
        TitleWindow m_infowindow;

        static AppUpdater*  m_AppUpdater;
        std::string m_homeiconFilePath;
        std::string m_separatorFilePath;
        static int m_currentMoveIndex;
        static int m_popupMenuIndex;
        int get_Index(const int& mouseX, const int& mouseY);
        int m_previousCellwidth;
        int m_iconsize;
        bool m_mouseLeftButtonDown = false;
        bool m_mouseRightButtonDown = false;
        float m_mouseclickEventTime = 0.0f;

        DockItem* get_CurrentItem();

        // Timer for showing the title window
        Glib::Timer m_titleTimer;

        gdouble m_titleElapsedSeconds;
        int m_titleItemOldindex = 0;
        bool m_titleShow = false;
        void get_ItemPosition(const DockItemType dockType, int& x, int& y, int& width, int& height);

        void ExecuteApp(GdkEventButton* event);
        void AnimateItem(Glib::RefPtr<Gdk::Pixbuf> image);

        //static void on_window_opened(WnckScreen* screen, WnckWindow* window, gpointer data);
        //static void on_window_closed(WnckScreen* screen, WnckWindow* window, gpointer data);
        static void on_active_window_changed_callback(WnckScreen* screen, WnckWindow* previously_active_window, gpointer user_data);

        // Mouse handlers
        // http://www.horde3d.org/wiki/index.php5?title=Tutorial_-_Setup_Horde_with_Gtkmm
        virtual bool on_button_press_event(GdkEventButton *event);
        virtual bool on_button_release_event(GdkEventButton *event);
        virtual bool on_motion_notify_event(GdkEventMotion* event);
        virtual bool on_scroll_event(GdkEventScroll* e);

        bool on_timeoutDraw();

        //Override default signal handler:
        bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;

//        void RoundedRectangle(const Cairo::RefPtr<Cairo::Context>& cr, double x, double y, double width, double height, double radius);

        virtual bool on_enter_notify_event(GdkEventCrossing* crossing_event);
        virtual bool on_leave_notify_event(GdkEventCrossing* crossing_event);

        float m_time = 0;
        float position = 0;
        float initTime;
        float endPosition;
        static bool m_mouseIn;
        bool m_animate = false;
        bool m_timerStoped = true;
        bool m_visible = true;
        float atime = 0.f;
        Glib::Timer m_Timer;

        void draw_Panel(const Cairo::RefPtr<Cairo::Context>& cr);
        void draw_Items(const Cairo::RefPtr<Cairo::Context>& cr);
        void show_Title();

        // Animation thread related
        std::thread* m_AppRunThreadLauncher = nullptr;
        static void AppRunAnimation();
        static bool m_AppThreadRunning;

        // Menu events
        void on_popup_homemenu_position(int& x, int& y, bool& push_in);
        void on_popup_itemmenu_position(int& x, int& y, bool& push_in);

        void on_QuitMenu_event();
        void on_HideMenu_event();
        void on_NewMenu_event();
        void on_DettachMenu_event();
        void on_AttachMenu_event();
        void on_AutohideToggled_event();
        void on_HomeMinimizeAllWindowsExceptActive_event();
        void on_HomeUnMinimizeAllWindows_event();
        void on_AboutMenu_event();
        void on_HomeMinimizeAllWindows_event();
        void on_HelpMenu_event();
        void on_HomeCloseAllWindowsExceptActive_event();
        void on_HomeAddSessionGrp_event();
        void on_HomePreferences_event();
        void on_HomeCloseAllWindows_event();
        void on_CloseAll_event();
        void on_CloseAllExceptActive_event();
        void on_MinimieAll_event();
        void on_MinimieAllExceptActive_event();
        void on_UnMinimieAll_event();
        void on_AddSeparator_event();

            // Drag & Drop
            //sigc::connection m_signalDragDrop;

        void DragDropEnds();
        DockItem* m_dragDropItem = nullptr;
        Glib::Timer m_dragdropTimer;
        bool  m_DragDropBegin = false;
        Gdk::Point m_dragdropMousePoint;
        guint m_DragDropSourceIndex = 0;
};
#endif	/* DOCKPANEL_H */

