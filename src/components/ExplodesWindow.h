#pragma once

// clang-format off
#include <glibmm/main.h>
#include <gtkmm/window.h>

#include "components/CompositedWindow.h"
// clang-format on

namespace docklight
{

    class ExplodesWindow : public CompositedWindow
    {
      public:
        ExplodesWindow();
        ~ExplodesWindow();

        void show_at(int x, int y);
        bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;

      private:
        const int RUN_LENGTH = {300 * 1000};
        long int m_start_time = 0;

        sigc::connection m_timeout_draw;

        void ConnectSignal(bool b);

        long int m_frame_time = 0;
        //        gint m_width = 128;
        //        gint m_height = 1024;
        gfloat m_framerate = {1000 / 30};
        //      GdkPixbuf* m_pixbuf;
        Glib::RefPtr<Gdk::Pixbuf> m_poof_image;

        int m_poof_size = {0};
        int m_poof_frames = {0};

        bool on_timeout_draw();
    };

}  // namespace docklight
