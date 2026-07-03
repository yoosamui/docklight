#pragma once
//  Copyright (c) 2018-2026 yoosamui
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
#include <glibmm/main.h>
#include <gtkmm/window.h>

#include "utils/system.h"

#include "components/animboomwindow.h"
// clang-format on
//
namespace docklight
{

    class AnimationManager : public Glib::Object
    {
      public:
        AnimationManager();
        ~AnimationManager();

        void start(Glib::ustring name);

      private:
        // Runs on the GTK main loop (NOT a worker thread): GTK/GDK is single
        // threaded, so the animation window can only be touched from here.
        bool on_timeout();

      private:
        sigc::connection m_sigc_timer;
        Glib::RefPtr<AnimBoomWindow> m_anim;
        Glib::ustring m_name;
        bool m_run = false;
    };

    Glib::RefPtr<AnimationManager> create_animation_manager();
    Glib::RefPtr<AnimationManager> Animation();
}  // namespace docklight
