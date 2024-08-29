//  Copyright (c) 2018-2024 Juan R. González
//
//  This file is part of Docklight.
//
//  Docklight is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  Docklight is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.

// clang-format off
#include "components/TransparentWindow.h"
// clang-format on

TransparentWindow::TransparentWindow()
{
    GdkScreen* screen;
    GdkVisual* visual;

    gtk_widget_set_app_paintable(GTK_WIDGET(gobj()), TRUE);
    screen = gdk_screen_get_default();
    visual = gdk_screen_get_rgba_visual(screen);

    if (visual != nullptr && gdk_screen_is_composited(screen)) {
        gtk_widget_set_visual(GTK_WIDGET(gobj()), visual);
    }

    set_decorated(false);
    set_resizable(false);
    set_keep_above(true);
}

// bool TransparentWindow::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
//{
// cr->save();
////  Clear destination layer (bounded)
// cr->set_operator(Cairo::Operator::OPERATOR_CLEAR);
////   cr->set_operator(Cairo::Operator::OPERATOR_SOURCE);
// cr->paint();
// cr->restore();

// return false;
//}
