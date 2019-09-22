
#include "components/config.h"
#include <string>
#include "components/arguments.h"
#include "components/device.h"
#include "components/panel.h"
#include "utils/string.h"
#include "utils/system.h"

DL_NS_BEGIN
namespace config
{
#define MARGIN 12
#define DEF_CONFIG_FILENAME "docklight.config"
#define DEF_DEFAULT_ICON_SIZE 42
#define DEF_DEFAULT_SEPARATOR_MARGIN 12
#define DEF_DEFAULT_SEPARATOR_SIZE 8

    using namespace style;

    Theme m_theme;
    dock_location_t m_location = dock_location_t::bottom;
    int m_icon_size = DEF_DEFAULT_ICON_SIZE;
    int m_separator_margin = DEF_DEFAULT_SEPARATOR_MARGIN;
    int m_separator_size = DEF_DEFAULT_SEPARATOR_SIZE;
    dock_indicator_type_t m_indicator_type = dock_indicator_type_t::dots;
    bool m_show_title = true;
    bool m_show_separator_line = true;
    dock_alignment_t m_alignment = dock_alignment_t::center;
    dock_autohide_type_t m_autohide_type = dock_autohide_type_t::intelihide;

    void get_color_from_string(const std::string &s, Color &fill, Color &stroke, double &lineWidth,
                               double &ratio, int &mask)
    {
        string currentLocale = setlocale(LC_NUMERIC, NULL);
        setlocale(LC_NUMERIC, "C");

        const int MAXBUFF = 50;
        int maxlength = s.size();
        std::string token = "";
        double values[MAXBUFF] = {0};
        int index = 0;
        for (int i = 0; i < maxlength; i++) {
            if (index < MAXBUFF) {
                char c = s[i];
                if (c != ',' && c != ' ') {
                    token += c;
                }

                if ((c == ',') || i + 1 == maxlength) {
                    double value = 0.0;
                    try {
                        value = std::stod(token);
                        values[index] = value;

                    } catch (...) {
                        g_critical("get_color_from_string: can't convert the token: %s\n",
                                   s.c_str());
                    }

                    token = "";
                    index++;
                }
            }
        }

        fill.red = values[0];
        fill.green = values[1];
        fill.blue = values[2];
        fill.alpha = values[3];

        stroke.red = values[4];
        stroke.green = values[5];
        stroke.blue = values[6];
        stroke.alpha = values[7];

        lineWidth = values[8];
        ratio = values[9];
        mask = (int)values[10];

        setlocale(LC_NUMERIC, currentLocale.c_str());
    }

    string get_filepath()
    {
        string user_name = system_util::get_current_user();

        char config_dir[120];
        sprintf(config_dir, "/home/%s/.config/docklight", user_name.c_str());

        system_util::create_directory_if_not_exitst(config_dir);

        char buff[PATH_MAX];
        sprintf(buff, "%s/%s", config_dir, DEF_CONFIG_FILENAME);

        return buff;
    }

    void set_default_style()
    {
        // clang-format off
        //
        m_theme.set_Panel(new ColorWindow());
        m_theme.set_PanelCell(new ColorWindow(Color(0, 0.50, 0.66, 1),Color(1, 0, 0, 1), 1.5, 1, 0));
        m_theme.set_PanelDrag(new ColorWindow(Color(1, 1, 1, 0.4), Color(1, 1, 1, 1), 2.5, 3, 0));
        m_theme.set_PanelIndicator(new ColorWindow(Color(1, 1, 1, 1), Color(1, 1, 1, 1), 0, 0, 0));
        m_theme.set_PanelSeparator(new ColorWindow(Color(0, 0.50, 0.66, 1),Color(1, 1, 1, 1.0), 1.0, 0, 0));

/*
        m_theme.set_Selector(new ColorWindow(Color(255, 255, 255, 0.3), Color(1, 1, 1, 1), 1.5, 3, 0));
        m_theme.set_PanelTitle(new ColorWindow(Color(0, 0, 0, 1), Color(0, 0, 0, 1), 1, 6, 0));
        m_theme.set_PanelTitleText(new ColorWindow(Color(), Color(1, 1, 1, 1), 1, 0, 0));
        m_theme.set_Preview(new ColorWindow());
        m_theme.set_PreviewCell(new ColorWindow(Color(1, 1, 1, 0.2), Color(1, 1, 1, 0), 1.5, 3, 0));
        m_theme.set_PreviewTitleText(new ColorWindow(Color(), Color(1, 1, 1, 1), 0, 0, 0));
        m_theme.set_PreviewClose(new ColorWindow(Color(1, 0, 0, 1), Color(1, 1, 1, 1), 1.5, 0, 0));
        m_theme.set_Separator(new ColorWindow(Color(), Color(1, 1, 1, 1), 1, 0, 0));
*/
        // clang-format on
    }
    GKeyFile *load_config_file()
    {
        set_default_style();

        string filepath = get_filepath();
        GError *error = nullptr;
        GKeyFile *key_file = g_key_file_new();

        auto found = g_key_file_load_from_file(key_file, filepath.c_str(),
                                               GKeyFileFlags::G_KEY_FILE_NONE, &error);

        if (!found) {
            if (error) {
                g_warning("Load: configuration could not be found. %s\n", error->message);
                g_error_free(error);
                error = nullptr;
            }

            g_key_file_free(key_file);
            return nullptr;
        }

        return key_file;
    }

    int get_icon_size(GKeyFile *key_file)
    {
        GError *error = nullptr;
        int value = g_key_file_get_integer(key_file, "dock", "icon_size", &error);
        if (error) {
            g_error_free(error);
            error = nullptr;

            return m_icon_size;
        }

        return value;
    }

    string get_indicator_type_key(GKeyFile *key_file)
    {
        GError *error = nullptr;
        char *value = g_key_file_get_string(key_file, "dock", "indicator_type", &error);
        if (error) {
            g_error_free(error);
            error = nullptr;

            return string{};
        }

        return value;
    }

    string get_style(GKeyFile *key_file)
    {
        GError *error = nullptr;
        char *value = g_key_file_get_string(key_file, "dock", "style", &error);
        if (error) {
            g_error_free(error);
            error = nullptr;

            return string{};
        }

        // check if exits
        if (g_key_file_get_string(key_file, value, "panel", &error) == nullptr) {
            g_error_free(error);
            error = nullptr;

            return string{};
        }

        return value;
    }

    string get_style_item(GKeyFile *key_file, const string &style_name, const string &item_name)
    {
        GError *error = nullptr;
        char *value =
            g_key_file_get_string(key_file, style_name.c_str(), item_name.c_str(), &error);
        if (!value || error) {
            g_error_free(error);
            error = nullptr;

            return string{};
        }

        return value;
    }

    string get_location(GKeyFile *key_file)
    {
        GError *error = nullptr;
        char *value = g_key_file_get_string(key_file, "dock", "location", &error);
        if (error) {
            g_error_free(error);
            error = nullptr;

            return string{};
        }

        return value;
    }

    string get_autohide(GKeyFile *key_file)
    {
        GError *error = nullptr;
        char *value = g_key_file_get_string(key_file, "dock", "autohide_mode", &error);
        if (error) {
            g_error_free(error);
            error = nullptr;

            return string{};
        }

        return value;
    }

    string get_alignment(GKeyFile *key_file)
    {
        GError *error = nullptr;
        char *value = g_key_file_get_string(key_file, "dock", "alignment", &error);
        if (error) {
            g_error_free(error);
            error = nullptr;

            return string{};
        }

        return value;
    }

    bool get_separator_show_line(GKeyFile *key_file)
    {
        GError *error = nullptr;
        bool result;
        result = g_key_file_get_boolean(key_file, "separator", "show_line", &error);
        if (error) {
            g_error_free(error);
            error = nullptr;

            return false;
        }

        return result;
    }

    int get_separator_size(GKeyFile *key_file)
    {
        GError *error = nullptr;
        int value = g_key_file_get_integer(key_file, "separator", "separator_size", &error);
        if (error) {
            g_error_free(error);
            error = nullptr;

            return m_separator_size;
        }

        return value;
    }

    int get_separator_margin(GKeyFile *key_file)
    {
        GError *error = nullptr;
        int value = g_key_file_get_integer(key_file, "separator", "separator_margin", &error);
        if (error) {
            g_error_free(error);
            error = nullptr;

            return m_separator_margin;
        }

        return value;
    }
    void load(const GSList *args_list)
    {
        GKeyFile *key_file = load_config_file();
        if (key_file != nullptr) {
            m_icon_size = get_icon_size(key_file);
            if (m_icon_size < 24 || m_icon_size > 128) {
                m_icon_size = DEF_DEFAULT_ICON_SIZE;
            }

            // location
            string location = get_location(key_file);
            if (!location.empty()) {
                if (location == "left") {
                    m_location = dock_location_t::left;
                } else if (location == "top") {
                    m_location = dock_location_t::top;
                } else if (location == "right") {
                    m_location = dock_location_t::right;
                } else if (location == "bottom") {
                    m_location = dock_location_t::bottom;
                } else {
                    g_warning("configuration: invalid location. %s\n", location.c_str());
                }
            }

            // alignment
            string alignment = get_alignment(key_file);
            if (!alignment.empty()) {
                if (alignment == "start") {
                    m_alignment = dock_alignment_t::start;
                } else if (alignment == "end") {
                    m_alignment = dock_alignment_t::end;
                } else if (alignment == "center") {
                    m_alignment = dock_alignment_t::center;
                } else if (alignment == "fill") {
                    m_alignment = dock_alignment_t::fill;
                } else {
                    g_warning("configuration: invalid alignment. %s\n", alignment.c_str());
                }
            }

            string autohide = get_autohide(key_file);
            if (!autohide.empty()) {
                if (autohide == "autohide") {
                    m_autohide_type = dock_autohide_type_t::autohide;
                } else if (autohide == "intelihide") {
                    m_autohide_type = dock_autohide_type_t::intelihide;
                } else if (autohide == "none") {
                    m_autohide_type = dock_autohide_type_t::none;
                } else {
                    g_warning("configuration: invalid autohide mode. %s\n", autohide.c_str());
                }
            }

            string indicator = get_indicator_type_key(key_file);
            if (!indicator.empty()) {
                if (indicator == "dots") {
                    m_indicator_type = dock_indicator_type_t::dots;
                } else if (indicator == "lines") {
                    m_indicator_type = dock_indicator_type_t::lines;
                } else {
                    g_warning("configuration: invalid indicator mode. %s\n", indicator.c_str());
                }
            }

            // separator show line
            m_show_separator_line = get_separator_show_line(key_file);

            // separator size
            m_separator_size = get_separator_size(key_file);
            if (m_separator_size < 0 || m_separator_size > 20) {
                m_separator_size = DEF_DEFAULT_SEPARATOR_SIZE;
            }

            // separator margin
            m_separator_margin = get_separator_margin(key_file);
            if (m_separator_margin < 0 || m_separator_margin > 20) {
                m_separator_margin = DEF_DEFAULT_ICON_SIZE;
            }

            // styles
            string style_name = get_style(key_file);
            if (!style_name.empty()) {
                Color fill;
                Color stroke;
                double lineWidth;
                double ratio;
                int mask;

                const string panel = get_style_item(key_file, style_name, "panel");
                if (!panel.empty()) {
                    get_color_from_string(panel.c_str(), fill, stroke, lineWidth, ratio, mask);
                    m_theme.set_Panel(new ColorWindow(fill, stroke, lineWidth, ratio, mask));
                }

                const string panel_cell = get_style_item(key_file, style_name, "panel_cell");
                if (!panel_cell.empty()) {
                    get_color_from_string(panel_cell.c_str(), fill, stroke, lineWidth, ratio, mask);
                    m_theme.set_PanelCell(new ColorWindow(fill, stroke, lineWidth, ratio, mask));
                }

                const string panel_indicator =
                    get_style_item(key_file, style_name, "panel_indicator");
                if (!panel_cell.empty()) {
                    get_color_from_string(panel_indicator.c_str(), fill, stroke, lineWidth, ratio,
                                          mask);
                    m_theme.set_PanelIndicator(
                        new ColorWindow(fill, stroke, lineWidth, ratio, mask));
                }

                const string panel_drag = get_style_item(key_file, style_name, "panel_drag");
                if (!panel_cell.empty()) {
                    get_color_from_string(panel_drag.c_str(), fill, stroke, lineWidth, ratio, mask);
                    m_theme.set_PanelDrag(new ColorWindow(fill, stroke, lineWidth, ratio, mask));
                }
            } else {  // use default theme
                //    m_show_separator_line = false;
            }

            g_key_file_free(key_file);
        }

        // paramters override config settings
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

    Theme get_theme() { return m_theme; }

    bool is_show_title() { return m_show_title; }

    bool is_autohide() { return m_autohide_type == dock_autohide_type_t::autohide; }

    bool is_intelihide() { return m_autohide_type == dock_autohide_type_t::intelihide; }

    bool is_separator_line() { return m_show_separator_line; }

    void set_separator_line(bool value) { m_show_separator_line = value; }

    int get_window_start_end_margin() { return 20; }

    int get_separator_margin() { return m_separator_margin; }

    int get_separator_size() { return m_separator_size; }

    dock_location_t get_dock_location() { return m_location; }

    dock_indicator_type_t get_indicator_type() { return m_indicator_type; }

    int get_dock_area()
    {
        int area = m_icon_size + MARGIN;

        // if (Panel::m_stm.m_decrease_factor > 0) {
        // area = AppUpdater::m_dockitems[0]->get_width();  // Panel::m_stm.m_decrease_factor;
        //}

        return area;
    }

    int get_icon_size()
    {
        int icon_size = m_icon_size;

        // if (Panel::m_stm.m_decrease_factor > 0) {
        // icon_size -= Panel::m_stm.m_decrease_factor;
        //}

        return icon_size;
    }

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
