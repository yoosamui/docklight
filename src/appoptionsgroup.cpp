
// clang-format off
#include <glibmm/i18n.h>

#include "appoptionsgroup.h"
#include "translations.h"

// clang-format on
//
namespace docklight
{

    AppOptionsGroup::AppOptionsGroup()
        : Glib::OptionGroup("Docklight",
                            "Supports short and long commandline options for the group.",
                            "help - Options can be single letters, prefixed by a single dash."),
          m_arg_monitor(0),
          m_arg_location("bottom"),
          m_arg_alignment("center")
    {
        m_entry_monitor.set_long_name("monitor");
        m_entry_monitor.set_short_name('m');
        m_entry_monitor.set_description(MSG_SETS_THE_PRIMARY_MONITOR);
        add_entry(m_entry_monitor, m_arg_monitor);

        m_entry_location.set_long_name("location");
        m_entry_location.set_short_name('l');
        m_entry_location.set_description(MSG_SETS_THE_PANEL_LOCATION);
        add_entry(m_entry_location, m_arg_location);

        m_entry_alignment.set_long_name("alignment");
        m_entry_alignment.set_short_name('r');
        m_entry_alignment.set_description(
            _("The dock alignment settings are start, end, center or fill. Default center."));
        add_entry(m_entry_alignment, m_arg_alignment);
    }

    void AppOptionsGroup::validate()
    {
        validate_location();
        validate_monitor();
        validate_alignment();
    }

    Glib::ustring AppOptionsGroup::toString() const
    {
        std::stringstream result;
        result << MSG_PARSED_VALUES << " :" << std::endl
               << "  monitor"
               << " :" << m_arg_monitor << std::endl
               << "  location"
               << " :" << m_arg_location << std::endl
               << "  alignment"
               << " :" << m_arg_alignment << std::endl;

        return result.str();
    }

    void AppOptionsGroup::validate_monitor()
    {
        int count = device::monitor::get_monitor_count();

        if (m_arg_monitor < count) {
            return;
        }

        std::stringstream msg;
        msg << "m_arg_monitor: unexpected out of range value:" << m_arg_monitor << std::endl
            << m_entry_monitor.get_description() << std::endl
            << "Valid arguments range from 0 to " << (count - 1) << " monitor count: " << count
            << std::endl;

        throw Glib::OptionError(Glib::OptionError::BAD_VALUE, msg.str());
    }

    void AppOptionsGroup::validate_location()
    {
        if (m_arg_location == "top" || m_arg_location == "right" || m_arg_location == "bottom" ||
            m_arg_location == "left") {
            return;
        }

        std::stringstream msg;
        msg << "m_arg_location: unexpected option name: " << m_arg_location << std::endl
            << m_entry_location.get_description() << std::endl;

        throw Glib::OptionError(Glib::OptionError::UNKNOWN_OPTION, msg.str());
    }

    void AppOptionsGroup::validate_alignment()
    {
        if (m_arg_alignment == "start" || m_arg_alignment == "end" || m_arg_alignment == "center" ||
            m_arg_alignment == "fill") {
            return;
        }

        std::stringstream msg;
        msg << "m_arg_alignment: unexpected option name: " << m_arg_alignment << "\n"
            << m_entry_alignment.get_description() << std::endl;

        throw Glib::OptionError(Glib::OptionError::UNKNOWN_OPTION, msg.str());
    }

    const std::vector<std::tuple<gchar, int, Glib::ustring>>& AppOptionsGroup::getList()
    {
        m_list.push_back(std::make_tuple(m_entry_monitor.get_short_name(), m_arg_monitor, ""));
        m_list.push_back(std::make_tuple(m_entry_location.get_short_name(), 0, m_arg_location));
        m_list.push_back(std::make_tuple(m_entry_alignment.get_short_name(), 0, m_arg_alignment));

        return m_list;
    }
}  // namespace docklight
