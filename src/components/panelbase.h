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

#include "components/panelpreview.h"
//#include "components/titlewindow.h"

DL_NS_BEGIN

class Panel_base  //: public Gtk::DrawingArea
{
  public:
    Panel_base();
    static int get_current_index();

  protected:
    static int m_current_index;
    AppUpdater m_appupdater;

    unique_ptr<Panel_preview> m_preview;
    int m_preview_index = -1;
};

DL_NS_END
