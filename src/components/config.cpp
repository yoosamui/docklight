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
    static constexpr const int DEF_DOCKAREA_MARGIN = 12;
    static constexpr const char *DEF_CONFIG_FILENAME = "docklight.config";
    static constexpr const int DEF_ICON_SIZE = 36;
    static constexpr const int DEF_SEPARATOR_MARGIN = 12;
    static constexpr const int DEF_SEPARATOR_SIZE = 8;
    static constexpr const double DEF_AUTOHIDE_ANIMATION_DELAY = 5.0;
    static constexpr const int DEF_AUTOHIDE_ANCHORT_MARGIN = 20;
    static constexpr const double DEF_AUTOHIDE_HIDE_DELAY = 0.5;

    using namespace style;

    Theme m_theme;
    dock_location_t m_location = dock_location_t::bottom;
    int m_icon_size = DEF_ICON_SIZE;
    int m_separator_margin = DEF_SEPARATOR_MARGIN;
    int m_separator_size = DEF_SEPARATOR_SIZE;
    dock_indicator_type_t m_indicator_type = dock_indicator_type_t::dots;
    bool m_show_title = true;
    bool m_show_separator_line = true;
    dock_alignment_t m_alignment = dock_alignment_t::center;
    dock_autohide_type_t m_autohide_type = dock_autohide_type_t::intelihide;
    float m_animation_delay = DEF_AUTOHIDE_ANIMATION_DELAY;
    int m_anchor_margin = DEF_AUTOHIDE_ANCHORT_MARGIN;
    double m_hide_delay = DEF_AUTOHIDE_HIDE_DELAY;

    void get_color_from_string(const std::string &s, Color &fill, Color &stroke, double &lineWidth,
                               double &ratio, int &mask)
    {
        string currentLocale = setlocale(LC_NUMERIC, NULL);
        setlocale(LC_NUMERIC, "C");

        const int MAXBUFF = 64;
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
        m_theme.set_Panel(new ColorWindow(Color(0, 0.50, 0.66, 1),Color(1, 1, 1, 0), 0, 3, 0));
        m_theme.set_PanelCell(new ColorWindow(Color(0, 0.50, 0.66, 1),Color(1, 1, 1, 1), 0.5, 3, 0));
        m_theme.set_PanelDrag(new ColorWindow(Color(1, 1, 1, 0.4), Color(1, 1, 1, 1), 2.5, 3, 0));
        m_theme.set_PanelIndicator(new ColorWindow(Color(1, 1, 1, 0.7), Color(1, 1, 1, 1), 2, 0, 0));
        m_theme.set_PanelSeparator(new ColorWindow(Color(0, 0.50, 0.66, 1),Color(1, 1, 1, 1.0), 1.0, 0, 0));
        m_theme.set_Preview(new ColorWindow());
        m_theme.set_PreviewCell(new ColorWindow(Color(1, 1, 1, 0.2), Color(1, 1, 1, 1), 1, 3, 0));
        m_theme.set_PreviewTitleText(new ColorWindow(Color(1,1,1,0.4), Color(1, 1, 1, 1), 0, 0, 0));
        m_theme.set_PreviewClose(new ColorWindow(Color(0.854, 0.062, 0.133, 1), Color(1, 1, 1, 1), 2.0, 0, 0));
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

    int get_anchor_margin(GKeyFile *key_file)
    {
        GError *error = nullptr;
        int value = g_key_file_get_integer(key_file, "dock", "anchor_margin", &error);
        if (error) {
            g_error_free(error);
            error = nullptr;

            return m_anchor_margin;
        }

        return value;
    }

    float get_animation_delay(GKeyFile *key_file)
    {
        GError *error = nullptr;
        double value = g_key_file_get_double(key_file, "dock", "animation_delay", &error);
        if (error) {
            g_error_free(error);
            error = nullptr;

            return m_animation_delay;
        }

        return value;
    }

    float get_hide_delay(GKeyFile *key_file)
    {
        GError *error = nullptr;
        double value = g_key_file_get_double(key_file, "dock", "hide_delay", &error);
        if (error) {
            g_error_free(error);
            error = nullptr;

            return m_hide_delay;
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
                m_icon_size = DEF_ICON_SIZE;
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
                m_separator_size = DEF_SEPARATOR_SIZE;
            }

            // separator margin
            m_separator_margin = get_separator_margin(key_file);
            if (m_separator_margin < 0 || m_separator_margin > 20) {
                m_separator_margin = DEF_ICON_SIZE;
            }

            m_anchor_margin = get_anchor_margin(key_file);
            if (m_anchor_margin < 1 || m_anchor_margin > 20) {
                m_anchor_margin = DEF_AUTOHIDE_ANCHORT_MARGIN;
            }

            m_animation_delay = get_animation_delay(key_file);
            if (m_animation_delay < 0.0 || m_animation_delay > 20.0) {
                m_animation_delay = DEF_AUTOHIDE_ANIMATION_DELAY;
            }

            m_hide_delay = get_hide_delay(key_file);
            if (m_hide_delay < 0.0 || m_hide_delay > 4.0) {
                m_hide_delay = DEF_AUTOHIDE_HIDE_DELAY;
            }

            // styles
            string style_name = get_style(key_file);
            if (!style_name.empty()) {
                Color fill;
                Color stroke;
                double lineWidth;
                double ratio;
                int mask;

                // clang-format off
                const string panel = get_style_item(key_file, style_name, "panel");
                if (!panel.empty()) {
                    get_color_from_string(panel.c_str(), fill, stroke, lineWidth, ratio, mask);
                    m_theme.set_Panel(new ColorWindow(fill, stroke, lineWidth, ratio, mask));
                }

                const string panel_gradient = get_style_item(key_file, style_name, "panel_gradient");
                if (!panel_gradient.empty()) {
                    get_color_from_string(panel_gradient.c_str(), fill, stroke, lineWidth, ratio, mask);
                    m_theme.set_PanelGradient(new ColorWindow(fill, stroke, lineWidth, ratio, mask));
                }

                const string panel_cell = get_style_item(key_file, style_name, "panel_cell");
                if (!panel_cell.empty()) {
                    get_color_from_string(panel_cell.c_str(), fill, stroke, lineWidth, ratio, mask);
                    m_theme.set_PanelCell(new ColorWindow(fill, stroke, lineWidth, ratio, mask));
                }

                const string panel_indicator = get_style_item(key_file, style_name, "panel_indicator");
                if (!panel_cell.empty()) {
                    get_color_from_string(panel_indicator.c_str(), fill, stroke, lineWidth, ratio, mask);
                    m_theme.set_PanelIndicator(new ColorWindow(fill, stroke, lineWidth, ratio, mask));
                }

                const string panel_separator = get_style_item(key_file, style_name, "panel_separator");
                if (!panel_cell.empty()) {
                    get_color_from_string(panel_separator.c_str(), fill, stroke, lineWidth, ratio, mask);
                    m_theme.set_PanelSeparator(new ColorWindow(fill, stroke, lineWidth, ratio, mask));
                }

                const string panel_drag = get_style_item(key_file, style_name, "panel_drag");
                if (!panel_cell.empty()) {
                    get_color_from_string(panel_drag.c_str(), fill, stroke, lineWidth, ratio, mask);
                    m_theme.set_PanelDrag(new ColorWindow(fill, stroke, lineWidth, ratio, mask));
                }

                // preview
                const string preview = get_style_item(key_file, style_name, "preview");
                if (!preview.empty()) {
                    get_color_from_string(preview.c_str(), fill, stroke, lineWidth, ratio, mask);
                    m_theme.set_Preview(new ColorWindow(fill, stroke, lineWidth, ratio, mask));
                }

                const string preview_gradient = get_style_item(key_file, style_name, "preview_gradient");
                if (!preview_gradient.empty()) {
                    get_color_from_string(preview_gradient.c_str(), fill, stroke, lineWidth, ratio, mask);
                    m_theme.set_PreviewGradient(new ColorWindow(fill, stroke, lineWidth, ratio, mask));
                }

                const string preview_cell = get_style_item(key_file, style_name, "preview_cell");
                if (!preview_cell.empty()) {
                    get_color_from_string(preview_cell.c_str(), fill, stroke, lineWidth, ratio,  mask);
                    m_theme.set_PreviewCell(new ColorWindow(fill, stroke, lineWidth, ratio, mask));
                }

                const string preview_title_text =  get_style_item(key_file, style_name, "preview_title_text");
                if (!preview_title_text.empty()) {
                    get_color_from_string(preview_title_text.c_str(), fill, stroke, lineWidth,  ratio, mask);
                    m_theme.set_PreviewTitleText( new ColorWindow(fill, stroke, lineWidth, ratio, mask));
                }

                const string preview_close =  get_style_item(key_file, style_name, "preview_close");
                if (!preview_close.empty()) {
                    get_color_from_string(preview_close.c_str(), fill, stroke, lineWidth,  ratio, mask);
                    m_theme.set_PreviewClose( new ColorWindow(fill, stroke, lineWidth, ratio, mask));
                }

                // clang-format on

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
                    device::monitor::set_current_monitor(data->int_value);
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

    bool is_autohide_none() { return m_autohide_type == dock_autohide_type_t::none; }

    bool is_autohide() { return m_autohide_type == dock_autohide_type_t::autohide; }

    bool is_intelihide() { return m_autohide_type == dock_autohide_type_t::intelihide; }

    bool is_separator_line() { return m_show_separator_line; }

    void set_separator_line(bool value) { m_show_separator_line = value; }

    int get_window_start_end_margin()
    {
        if (get_dock_alignment() == dock_alignment_t::fill) {
            return 10;
        }

        return 20;
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

    int get_separator_margin() { return m_separator_margin; }

    int get_separator_size() { return m_separator_size; }

    dock_location_t get_dock_location() { return m_location; }

    dock_indicator_type_t get_indicator_type() { return m_indicator_type; }

    int get_dock_area()
    {
        int area = m_icon_size + DEF_DOCKAREA_MARGIN;
        return area;
    }

    int get_icon_size()
    {
        int icon_size = m_icon_size;
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
