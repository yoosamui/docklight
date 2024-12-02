#pragma once
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
#include <gdkmm/general.h>
#include <glibmm/main.h>
#include <gtkmm/window.h>
// clang-format on

class CompositeWindow : public Gtk::Window
{
  public:
    CompositeWindow();

    // protected:
    //// Override default signal handler:
    //  virtual bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr);
    // bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;
};

