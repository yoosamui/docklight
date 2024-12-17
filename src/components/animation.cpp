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

#include "components/animation.h"

namespace docklight
{

    Glib::RefPtr<AnimationManager> m_animation_manager;
    Glib::RefPtr<AnimationManager> create_animation_manager()
    {
        if (!m_animation_manager) {
            m_animation_manager = Glib::RefPtr<AnimationManager>(new AnimationManager());
        }
        return m_animation_manager;
    }

    Glib::RefPtr<AnimationManager> Animation()
    {
        if (!m_animation_manager) create_animation_manager();
        return m_animation_manager;
    }

    AnimationManager::AnimationManager()
    {
        m_bck_thread =
            std::shared_ptr<std::thread>(new std::thread(&AnimationManager::thread_func, this));
    }

    void AnimationManager::start(Glib::ustring name)
    {
        //
        m_name = name;
        m_run = true;
    }

    void AnimationManager::thread_func()
    {
        auto anim = Glib::RefPtr<AnimBoomWindow>(new AnimBoomWindow());
        while (true) {
            if (m_run) {
                m_run = false;

                if (anim) {
                    int x = 0;
                    int y = 0;
                    system::get_mouse_position(x, y);

                    anim->show_at(x, y);
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
}  // namespace docklight
