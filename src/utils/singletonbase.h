#pragma once

#include <memory>
#include <mutex>

namespace docklight
{
    // Summary
    // guarantees thread safety for static local variables,
    // it's not always suitable:
    template <typename T>
    class SingletonBaseSlim
    {
      public:
        static T& getInstance()
        {
            static T instance;
            return instance;
        }
    };

    // Summary
    // This implementation provides a thread-safe, lazy-initialized singleton base class template in
    // C++11. It leverages modern C++ features to ensure thread safety without explicit locks,
    // offering both a simple instantiation method and a flexible constructor argument passing
    // method for creating instances of any type.
    template <typename T>
    class SingletonBase
    {
      protected:
        SingletonBase(const SingletonBase&) = delete;
        SingletonBase& operator=(const SingletonBase&) = delete;

        SingletonBase() = default;

        static std::unique_ptr<T> m_instance;
        static std::once_flag m_once;

      public:
        virtual ~SingletonBase() = default;

        // Ensure getInstance returns a pointer to T
        static T* getInstance()
        {
            std::call_once(m_once, []() { m_instance.reset(new T); });
            return m_instance.get();  // This line ensures a pointer is returned
        }

        template <typename... Args>
        static T* getInstance2nd(Args&&... args)
        {
            std::call_once(m_once, [&]() { m_instance.reset(new T(std::forward<Args>(args)...)); });
            return m_instance.get();  // Ensure a pointer is returned here as well
        }
    };

    template <typename T>
    std::unique_ptr<T> SingletonBase<T>::m_instance;

    template <typename T>
    std::once_flag SingletonBase<T>::m_once;

}  // namespace docklight

