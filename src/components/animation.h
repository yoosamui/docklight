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
#include <glibmm/main.h>
#include <gtkmm/window.h>

#include <thread>
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

        void start(Glib::ustring name);

      private:
        void thread_func();

      private:
        // backgound  worker
        std::shared_ptr<std::thread> m_bck_thread;
        Glib::ustring m_name;
        bool m_run = false;
    };

    Glib::RefPtr<AnimationManager> create_animation_manager();
    Glib::RefPtr<AnimationManager> Animation();
}  // namespace docklight
