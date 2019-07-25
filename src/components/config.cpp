
#include "components/config.h"
#include "components/arguments.h"
#include "components/device.h"

#define MARGIN 8

DL_NS_BEGIN
namespace config
{
    dock_location_t m_location = dock_location_t::bottom;
    int m_icon_size = 48;
    int m_separator_margin = 10;
    bool m_autohidde = true;

    void load(const GSList *args_list)
    {
        // TODO Load config

        if (args_list) {
            for (const GSList *l = args_list; l; l = l->next) {
                cli::result_t *data = static_cast<cli::result_t *>(l->data);
                g_print("arguments: --%c %d %s\n", data->arg, data->int_value,
                        data->string_value);

                if (data->arg == (char)'m') {
                    device::monitor::get_current()->set_current_monitor(
                        data->int_value);
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

                    continue;
                }
            }
        }
    }

    bool is_autohidde() { return m_autohidde; }
    int get_window_start_end_margin() { return 20; }

    int get_separator_margin() { return m_separator_margin; }

    dock_location_t get_dock_location() { return m_location; }

    int get_dock_area() { return m_icon_size + MARGIN; }

    int get_icon_size() { return m_icon_size; }

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
