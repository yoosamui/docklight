#pragma once

//#include <glibmm/timer.h>
//#include <gtkmm/drawingarea.h>
//#include <sigc++/sigc++.h>
//#include <thread>
#include "common.h"
#include "components/appupdater.h"
//#include "components/appupdater.h"
//#include "components/autohide.h"
#include "components/config.h"
//#include "components/dockmenu.h"

//#include "components/titlewindow.h"

DL_NS_BEGIN

class Panel_base  //: public Gtk::DrawingArea
{
  public:
    Panel_base();

  protected:
    static int m_current_index;
    AppUpdater m_appupdater;
};

DL_NS_END
