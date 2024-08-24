#pragma once

// clang-format off
#include <gdkmm/general.h>
#include <glibmm/main.h>
#include <gtkmm/window.h>


#include <gdkmm/pixbufanimation.h>


#include <gdk-pixbuf/gdk-pixbuf.h>
#include <gdk/gdkx.h>
// clang-format on
class CompositedWindow : public Gtk::Window
{
  public:
    CompositedWindow();

    bool virtual on_draw(const Cairo::RefPtr<Cairo::Context>& cr);
};

/////////////////////////
class PlankTest : public CompositedWindow
{
  private:
    const int RUN_LENGTH = 300 * 1000;
    long int m_start_time = {0};

    sigc::connection m_timeout_draw;

    void ConnectSignal(bool b);

    long int m_frame_time = {0};
    gint m_width = 128;
    gint m_height = 1024;
    gfloat m_rate = 1000 / 30;
    GdkPixbuf* m_pixbuf;
    Glib::RefPtr<Gdk::Pixbuf> m_poof_image;

    int m_poof_size = {0};
    int m_poof_frames = {0};
    // Glib::RefPtr<Gdk::PixbufAnimation> m_animation;
    //
    // Glib::RefPtr<Gdk::PixbufAnimationIter> m_iter;
    //     GdkPixbufAnimation*  anim = nullptr;
    GdkPixbufSimpleAnim* anim = nullptr;
    bool on_timeout_draw();

  public:
    PlankTest();
    ~PlankTest();
    void show_at(int x, int y);
    bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;
};

