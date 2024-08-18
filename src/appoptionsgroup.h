#pragma once
// https://gitlab.gnome.org/GNOME/glibmm/-/blob/master/examples/options/main.cc

#include <glibmm.h>

#include <iomanip>
#include <iostream>

#include "components/device.h"

// Commandline Option Parser
// https://gitlab.gnome.org/GNOME/glibmm/-/blob/master/examples/options/main.cc
//
// The OptionGroup commandline parser is intended to be a simpler replacement
// for the popt library. It supports short and long commandline options, as shown
// in the following example:
//
// main -r 1 --foo=34 --max-size 20 --rand --display=:1.0 -vb -- file1 file2
namespace docklight
{

    class AppOptionsResult
    {
      public:
        gchar short_name;
        Glib::ustring long_name;
    };  // namespace AppOptionsResult

    class AppOptionsGroup : public Glib::OptionGroup
    {
      private:
        std::vector<std::tuple<gchar, int, Glib::ustring>> m_list;

        Glib::OptionEntry m_entry_monitor;
        Glib::OptionEntry m_entry_location;
        Glib::OptionEntry m_entry_alignment;

        void validate_location();
        void validate_monitor();
        void validate_alignment();

        int m_arg_monitor;

        Glib::ustring m_arg_location;
        Glib::ustring m_arg_alignment;

      public:
        AppOptionsGroup();

        const std::vector<std::tuple<gchar, int, Glib::ustring>>& getList();
        void validate();
        Glib::ustring toString() const;
    };
}  // namespace docklight

