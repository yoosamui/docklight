#pragma once

#include <glibmm/main.h>

#include <any>
#include <memory>

namespace docklight
{
    namespace factory
    {
        // TODO: lo message in case of error
        template <typename T>
        bool any_cast(std::any a, T& target)
        {
            if (!a.has_value()) return false;

            if (a.type() == typeid(T)) {
                target = std::any_cast<T>(a);  // we not catch any excepion because of speed...

                return target ? true : false;
            }
            return false;
        }

    }  // namespace factory
}  // namespace docklight
