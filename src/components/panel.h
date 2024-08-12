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

#include "common.h"

DL_NS_BEGIN
using namespace std;

class Panel : public Gtk::DrawingArea
{
  public:
    Panel();
    ~Panel();
};

DL_NS_END
