#pragma once

#include <glibmm/main.h>
#include <glibmm/object.h>
#include <gtkmm/application.h>
#include <gtkmm/drawingarea.h>
#include <libwnck/libwnck.h>

//#include "components/panelhide.h"

//  class Panelbase;

// class PanelBase;
// bool PanelBase::m_lock_render;
namespace docklight
{

    class PanelBase : public Gtk::DrawingArea
    {
      public:
        PanelBase();

        //        PanelHide m_panel_hide;

      protected:
        Glib::RefPtr<Gtk::Application> m_app;

        static WnckWindow* m_active_window;
        static bool m_lock_render;
        static bool m_last_fullscreen;

        bool m_context_menu_active = false;
        bool m_mouse_enter = false;
        bool m_drag_drop_starts = false;

        guint m_dockitem_index = 0;
        guint m_drag_drop_item_index = 0;
        guint m_drag_drop_item_move_index = 0;
        gulong m_dockitem_active_index = 0;
    };

}  // namespace docklight

