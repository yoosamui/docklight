#include "components/config.h"

#include <iostream>

namespace docklight
{
    namespace Config
    {
        // parsed and optimized at compile time
        static constexpr const int DEF_MIN_ITEM_SIZE = 26;
        static constexpr const int DEF_DOCKAREA_MARGIN = 12;
        static constexpr const char* DEF_CONFIG_FILENAME = "docklight.config";
        static constexpr const int DEF_ICON_SIZE = 36;
        static constexpr const int DEF_ICON_MAXSIZE = 128;
        static constexpr const int DEF_SEPARATOR_MARGIN = 12;
        static constexpr const int DEF_SEPARATOR_SIZE = 8;
        static constexpr const double DEF_AUTOHIDE_ANIMATION_DELAY = 5.0;
        static constexpr const int DEF_AUTOHIDE_ANCHORT_MARGIN = 20;
        static constexpr const double DEF_AUTOHIDE_HIDE_DELAY = 0.5;

        // Initializers
        int m_icon_size = DEF_ICON_SIZE;
        int m_separator_margin = DEF_SEPARATOR_MARGIN;
        int m_separator_size = DEF_SEPARATOR_SIZE;
        int m_anchor_margin = DEF_AUTOHIDE_ANCHORT_MARGIN;

        Gtk::Orientation get_dock_orientation();
        dock_location_t get_dock_location();
        dock_alignment_t get_dock_alignment();
        dock_indicator_type_t get_indicator_type();
        dock_alignment_t m_alignment = dock_alignment_t::center;
        dock_autohide_type_t m_autohide_type = dock_autohide_type_t::intelihide;
        dock_location_t m_location = dock_location_t::bottom;
        dock_indicator_type_t m_indicator_type = dock_indicator_type_t::dots;

        bool m_show_title = true;
        bool m_show_separator_line = true;

        float m_animation_delay = DEF_AUTOHIDE_ANIMATION_DELAY;
        float m_hide_delay = DEF_AUTOHIDE_HIDE_DELAY;

        void AddArgs(const std::vector<std::tuple<gchar, int, Glib::ustring>>& args)
        {
            for (auto&& t : args) {
                std::cout << "R:" << std::get<0>(t) << ", " << std::get<1>(t) << ", "
                          << std::get<2>(t) << std::endl;
            }
        }

        // get/set location
        dock_location_t get_dock_location()
        {
            //
            return m_location;
        }

        void set_dock_location(dock_location_t location)
        {
            //
            m_location = location;
        }

        int get_dock_area()
        {
            int area = m_icon_size + DEF_DOCKAREA_MARGIN;
            return area;
        }

        Gtk::Orientation get_dock_orientation()
        {
            switch (m_location) {
                case dock_location_t::top:
                case dock_location_t::bottom:
                    return Gtk::ORIENTATION_HORIZONTAL;

                case dock_location_t::left:
                case dock_location_t::right:
                    return Gtk::ORIENTATION_VERTICAL;

                default:
                    return Gtk::ORIENTATION_HORIZONTAL;
            }
        }

        dock_alignment_t get_dock_alignment()
        {
            return m_alignment;
        }

        dock_indicator_type_t get_indicator_type()
        {
            return m_indicator_type;
        }

        void set_dock_alignment(dock_alignment_t value)
        {
            m_alignment = value;
        }

        int get_separator_margin()
        {
            //
            return m_separator_margin;
        }

        int get_window_start_end_margin()
        {
            if (get_dock_alignment() == dock_alignment_t::fill) {
                return 10;  // TODO: constant
            }

            // TODO: constant
            return 20;
        }

        int get_icon_size()
        {
            int icon_size = m_icon_size;
            return icon_size;
        }

        void set_icon_size(int value)
        {
            m_icon_size = value;

            if (value > DEF_ICON_MAXSIZE) {
                m_icon_size = DEF_ICON_MAXSIZE;
            }
        }

        bool is_autohide_none()
        {
            return m_autohide_type == dock_autohide_type_t::none;
        }

        bool is_autohide()
        {
            return m_autohide_type == dock_autohide_type_t::autohide;
        }

        bool is_intelihide()
        {
            return m_autohide_type == dock_autohide_type_t::intelihide;
        }

        bool is_separator_line()
        {
            return m_show_separator_line;
        }

        void set_separator_line(bool value)
        {
            m_show_separator_line = value;
        }

        float get_animation_delay()
        {
            //
            return m_animation_delay;
        }

        float get_hide_delay()
        {
            //
            return m_hide_delay;
        }

        int get_anchor_margin()
        {
            //
            return m_anchor_margin;
        }

    }  // namespace Config
}  // namespace docklight
