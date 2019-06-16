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


#ifndef DOCKPREVIEW_H
#define DOCKPREVIEW_H

#include <gtkmm.h>
#include "Configuration.h"
#include "DockItem.h"
#include <thread>
#include <gdkmm/pixbuf.h>


#define PREVIEW_HV_OFFSET 20
#define PREVIEW_FRAMES_PERSECOND  8
#define PREVIEW_TITLE_OFFSET 28
#define PREVIEW_MAX_ELAPSED_FRAMES PREVIEW_FRAMES_PERSECOND * 3


class DockPreview : public Gtk::Window
{
    public:
        DockPreview();
        ~DockPreview();
        void Show(const std::vector<DockItem*>& items, const guint index, const guint cellSize);
        bool Update();
        // https://developer.gnome.org/gtkmm-tutorial/stable/chapter-custom-signals-example.html.en
        // signal accessor:
        //typedef sigc::signal<void, WnckWindow*, int> type_signal_update;
        //type_signal_update signal_update();

        static GdkPixbuf* GetPreviewImage(DockItem* item, guint& scaleWidth, guint& scaleHeight);
        static Glib::RefPtr<Gdk::Pixbuf> LoadPreviewImage(DockItem* item);

        static guint m_cellWidth;
        static guint m_cellHeight;

    private:

    //    static type_signal_update m_signal_update;

        static bool m_updateRequired;
        guint m_dockItemIndex;
        guint m_dockItemCellSize;

        guint m_closeSymbolX = 0;
        guint m_closeSymbolY = 0;
        bool m_closeSymbolMouseOver = false;

        int m_currentIndex = -1;
        bool m_mouseIn = false;
        bool m_isMutterWindowManager = false;

        //std::thread* m_thread = nullptr;
       // static bool threadRunning;
       // static void MovementDetector();
       // static bool m_detectMovement;
        static bool m_allowDraw;

        Glib::RefPtr<Pango::Layout> m_refLayout;
        Pango::FontDescription m_font;

        //Glib::Timer m_detectMovementTimer;
        static guint get_CountItems();
        //bool RemoveCurrentPreviewItem();
        void set_ItemsDynamic();
        void set_CurrentItemDynamic();

        static void Close();
        bool on_leave_notify_event(GdkEventCrossing* crossing_event);
        bool on_enter_notify_event(GdkEventCrossing* crossing_event);

       // bool scanSet = false;
        bool m_mouseDown = false;

        int get_Index(const int& mouseX, const int& mouseY);
        bool on_button_press_event(GdkEventButton *event);
        bool on_button_release_event(GdkEventButton *event);
        bool on_motion_notify_event(GdkEventMotion*event);

        bool ComparePixels (const Glib::RefPtr<Gdk::Pixbuf>& first_pixbuf, const Glib::RefPtr<Gdk::Pixbuf>& current_pixbuf);
        bool ComparePixels(const guint8* pixels, const GdkPixbuf* current_pixbuf);
        bool ComparePixels (const GdkPixbuf* pixbuf, const guint8* pixels_first, const guint8* pixels_current);
        Configuration::Style::Theme m_Theme = Configuration::get_Theme();

        bool on_timeoutDraw();
        static std::vector<DockItem*> m_previewItems;
        static void on_window_opened(WnckScreen *screen, WnckWindow *window, gpointer data);
        static void on_window_closed(WnckScreen *screen, WnckWindow *window, gpointer data);
        bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;
};

#endif /* DOCKPREVIEW_H */
