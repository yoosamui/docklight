#pragma once

// clang-format off
//
//#include <cairomm/context.h>
//#include <gdkmm.h>
#include <glibmm/main.h>
#include <gtkmm/drawingarea.h>

//#include <cmath>
//#include <ctime>

// clang-format on
//
//

namespace docklight
{

    class Panel : public Gtk::DrawingArea
    {
      public:
        Panel();
        ~Panel();
    };
}  // namespace docklight

