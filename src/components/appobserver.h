#pragma once
//  Copyright (c) 2018-2024 Juan R. González
//
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
//  public Glib::Object GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  identification number, along with this program.  If not, see <http://www.gnu.org/licenses/>.

// clang-format off
#include "components/dockitemprovider.h"
#include "utils/pixbuf.h"
#include "utils/wnck.h"
// clang-format on

namespace docklight
{

    class AppObserver : public Glib::Object
    {
      public:
        AppObserver();

      private:
        static void on_actions_changed(WnckWindow* window, WnckWindowActions changed_mask,
                                       WnckWindowActions new_state, gpointer user_data);

        static void on_icon_changed(WnckWindow* window, gpointer user_data);
        static void on_window_closed(WnckScreen* screen, WnckWindow* window, gpointer data);
        static void on_window_opened(WnckScreen* screen, WnckWindow* window, gpointer data);
    };

    Glib::RefPtr<AppObserver> create_observer();
    Glib::RefPtr<AppObserver> Observer();

}  // namespace docklight
