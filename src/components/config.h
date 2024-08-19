#pragma once

#include <gmodule.h>
#include <gtkmm/enums.h>

#include "constants.h"

namespace docklight
{
    namespace Config
    {
        GKeyFile* load_file();

        void AddArgs(const std::vector<std::tuple<gchar, int, Glib::ustring>>& args);
        void set_dock_location(dock_location_t location);
        void set_icon_size(int value);
        void set_separator_line(bool value);
        void set_dock_alignment(dock_alignment_t value);
        void set_default_style();

        int get_anchor_margin();
        int get_dock_area();
        int get_separator_margin();
        int get_window_start_end_margin();
        int get_icon_size();
        int get_separator_size();

        dock_indicator_type_t get_indicator_type();
        dock_alignment_t get_dock_alignment();

        bool is_autohide_none();
        bool is_autohide();
        bool is_intelihide();
        bool is_show_title();
        bool is_separator_line();

        float get_animation_delay();
        float get_hide_delay();
        //    Config* m_config = Config::getInstance();
    }  // namespace Config
}  // namespace docklight
