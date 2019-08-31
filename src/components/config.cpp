
#include "components/config.h"
#include "components/arguments.h"
#include "components/device.h"

#define MARGIN 8

DL_NS_BEGIN
namespace config
{
    dock_location_t m_location = dock_location_t::bottom;
    int m_icon_size = 38;
    int m_separator_margin = 10;
    bool m_show_title = true;
    bool m_show_separator_line = true;
    dock_alignment_t m_alignment = dock_alignment_t::center;
    dock_autohide_type_t m_autohide_type = dock_autohide_type_t::intelihide;
    //:dock_autohide_type_t m_autohide_type = dock_autohide_type_t::autohide;
    void load(const GSList *args_list)
    {
        // TODO Load config

        if (args_list) {
            for (const GSList *l = args_list; l; l = l->next) {
                cli::result_t *data = static_cast<cli::result_t *>(l->data);
                g_print("arguments: --%c %d %s\n", data->arg, data->int_value, data->string_value);

                if (data->arg == (char)'m') {
                    device::monitor::get_current()->set_current_monitor(data->int_value);
                    continue;
                }

                if (data->arg == (char)'r') {
                    set_icon_size((int)data->int_value);
                    continue;
                }

                if (data->arg == (char)'l') {
                    if (strcmp(data->string_value, "bottom") == 0) {
                        m_location = dock_location_t::bottom;
                        continue;
                    }
                    if (strcmp(data->string_value, "left") == 0) {
                        m_location = dock_location_t::left;
                        continue;
                    }
                    if (strcmp(data->string_value, "top") == 0) {
                        m_location = dock_location_t::top;
                        continue;
                    }
                    if (strcmp(data->string_value, "right") == 0) {
                        m_location = dock_location_t::right;
                        continue;
                    }
                }

                if (data->arg == (char)'a') {
                    if (strcmp(data->string_value, "start") == 0) {
                        m_alignment = dock_alignment_t::start;
                        continue;
                    }
                    if (strcmp(data->string_value, "end") == 0) {
                        m_alignment = dock_alignment_t::end;
                        continue;
                    }
                    if (strcmp(data->string_value, "center") == 0) {
                        m_alignment = dock_alignment_t::center;
                        continue;
                    }
                    if (strcmp(data->string_value, "fill") == 0) {
                        m_alignment = dock_alignment_t::fill;
                        continue;
                    }
                }
            }
        }
    }

    bool is_show_title() { return m_show_title; }
    bool is_autohide() { return m_autohide_type == dock_autohide_type_t::autohide; }

    bool is_intelihide() { return m_autohide_type == dock_autohide_type_t::intelihide; }

    bool is_separator_line() { return m_show_separator_line; }

    void set_separator_line(bool value) { m_show_separator_line = value; }

    int get_window_start_end_margin() { return 20; }

    int get_separator_margin() { return m_separator_margin; }

    dock_location_t get_dock_location() { return m_location; }

    int get_dock_area() { return m_icon_size + MARGIN; }

    int get_icon_size() { return m_icon_size; }

    void set_icon_size(int value)
    {
        m_icon_size = value;

        if (value > 128) {
            m_icon_size = 128;
        }
    }

    dock_alignment_t get_dock_alignment() { return m_alignment; }

    void set_dock_alignment(dock_alignment_t value) { m_alignment = value; }

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

}  // namespace config
DL_NS_END
