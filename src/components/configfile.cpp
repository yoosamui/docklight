#include "components/configfile.h"

namespace docklight
{
    ConfigFile::ConfigFile()
    {
        g_message("Create ConfigFile.");
    }
    ConfigFile::~ConfigFile()
    {
        if (m_key_file) {
            g_key_file_free(m_key_file);
        }

        m_key_file = nullptr;
    }

    bool ConfigFile::load()
    {
        // set_default_style();
        std::string filepath = read_filepath();
        if (filepath.empty()) {
            g_warning("Configuration could not be found!");
            return false;
        }

        GError* error = nullptr;
        m_key_file = g_key_file_new();
        auto found = g_key_file_load_from_file(m_key_file, filepath.c_str(),
                                               GKeyFileFlags::G_KEY_FILE_NONE, &error);
        if (!found) {
            if (error) {
                g_error("Load: configuration could not be found. %s %s\n", error->message,
                        filepath.c_str());

                g_error_free(error);
                error = nullptr;
                g_key_file_free(m_key_file);
                found = false;
            }

            g_key_file_free(m_key_file);
            m_key_file = nullptr;
        } else {
            auto monitor_name = read_monitor_name();
            if (!monitor_name.empty()) {
                m_monitor_name = monitor_name;
            }

            std::string autohide = read_autohide();
            if (!autohide.empty()) {
                if (autohide == "autohide") {
                    m_autohide_mode = dock_autohide_type_t::autohide;
                } else if (autohide == "intelihide") {
                    m_autohide_mode = dock_autohide_type_t::intelihide;
                } else if (autohide == "none") {
                    m_autohide_mode = dock_autohide_type_t::none;
                } else {
                    g_warning("configuration: invalid autohide mode. %s\n", autohide.c_str());
                }
            }
            // location
            std::string location = read_location();
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
                    g_warning("configuration: invalid location. [%s] \n", location.c_str());
                }
            }

            // alignment
            std::string alignment = read_alignment();
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

            // icon alignment
            std::string icon_alignment = read_icon_alignment();
            if (!icon_alignment.empty()) {
                if (icon_alignment == "start") {
                    m_icon_alignment = dock_icon_alignment_t::start;
                } else if (icon_alignment == "end") {
                    m_icon_alignment = dock_icon_alignment_t::end;
                } else if (icon_alignment == "center") {
                    m_icon_alignment = dock_icon_alignment_t::center;
                } else {
                    g_warning("configuration: invalid icon alignment. %s\n",
                              icon_alignment.c_str());
                }
            }

            // Indicator type
            std::string indicator = read_indicator_type_key();
            if (!indicator.empty()) {
                if (indicator == "dots") {
                    m_indicator_type = dock_indicator_type_t::dots;
                } else if (indicator == "lines") {
                    m_indicator_type = dock_indicator_type_t::lines;
                } else {
                    g_warning("configuration: invalid indicator mode. %s\n", indicator.c_str());
                }
            }

            // the icon size
            m_icon_size = m_custom_icon_size = read_icon_size();
            m_image_size = m_custom_image_size = read_image_size();

            // separator show line
            m_show_separator_line = read_separator_show_line();

            // separator size
            m_separator_size = read_separator_size();
            if (m_separator_size < 0 || m_separator_size > 20) {
                m_separator_size = DEF_SEPARATOR_SIZE;
            }

            // separator margin
            // m_separator_margin = read_separator_margin();
            // if (m_separator_margin < 0 || m_separator_margin > 20) {
            // m_separator_margin = DEF_ICON_SIZE;
            //}

            m_anchor_margin = read_anchor_margin();
            if (m_anchor_margin < 1 || m_anchor_margin > 20) {
                m_anchor_margin = DEF_AUTOHIDE_ANCHORT_MARGIN;
            }

            m_animation_delay = read_animation_delay();
            if (m_animation_delay < 0.0 || m_animation_delay > 20.0) {
                m_animation_delay = DEF_AUTOHIDE_ANIMATION_DELAY;
            }

            m_hide_delay = read_hide_delay();
            if (m_hide_delay < 0.0 || m_hide_delay > 4.0) {
                m_hide_delay = DEF_AUTOHIDE_HIDE_DELAY;
            }
        }
        return found;
    }

    std::string ConfigFile::read_filepath()
    {
        auto user_name = system::get_current_user();

        char config_dir[512];
        sprintf(config_dir, "/home/%s/.config/docklight", user_name.c_str());

        system::create_directory_if_not_exitst(config_dir);

        char buff[PATH_MAX];
        sprintf(buff, "%s/%s", config_dir, m_filename.c_str());

        return buff;
    }

    std::string ConfigFile::read_autohide()
    {
        GError* error = nullptr;
        char* value = g_key_file_get_string(m_key_file, "dock", "autohide_mode", &error);
        if (error) {
            g_error_free(error);
            error = nullptr;

            return std::string{};
        }

        return value;
    }

    int ConfigFile::read_icon_size()
    {
        GError* error = nullptr;
        int value = g_key_file_get_integer(m_key_file, "dock", "icon_size", &error);
        if (error) {
            g_error_free(error);
            error = nullptr;

            return DEF_ICON_MAXSIZE;
        }

        return value;
    }

    int ConfigFile::read_image_size()
    {
        GError* error = nullptr;
        int value = g_key_file_get_integer(m_key_file, "dock", "preview_image_size", &error);
        if (error) {
            g_error_free(error);
            error = nullptr;

            return DEF_PREVIEW_IMAGE_MAX_SIZE;
        }

        return value;
    }

    std::string ConfigFile::read_monitor_name()
    {
        GError* error = nullptr;
        char* value = g_key_file_get_string(m_key_file, "dock", "monitor_name", &error);
        if (error) {
            g_error_free(error);
            error = nullptr;
            return std::string{};
        }

        return value;
    }
    std::string ConfigFile::read_location()
    {
        GError* error = nullptr;
        char* value = g_key_file_get_string(m_key_file, "dock", "location", &error);
        if (error) {
            g_error_free(error);
            error = nullptr;
            return std::string{};
        }

        return value;
    }

    std::string ConfigFile::read_alignment()
    {
        GError* error = nullptr;
        char* value = g_key_file_get_string(m_key_file, "dock", "alignment", &error);
        if (error) {
            g_error_free(error);
            error = nullptr;

            return std::string{};
        }

        return value;
    }

    std::string ConfigFile::read_icon_alignment()
    {
        GError* error = nullptr;
        char* value = g_key_file_get_string(m_key_file, "dock", "icon_alignment", &error);
        if (error) {
            g_error_free(error);
            error = nullptr;

            return std::string{};
        }

        return value;
    }

    std::string ConfigFile::read_indicator_type_key()
    {
        GError* error = nullptr;
        char* value = g_key_file_get_string(m_key_file, "dock", "indicator_type", &error);
        if (error) {
            g_error_free(error);
            error = nullptr;

            return std::string{};
        }

        return value;
    }

    bool ConfigFile::read_separator_show_line()
    {
        GError* error = nullptr;
        bool result;
        result = g_key_file_get_boolean(m_key_file, "separator", "show_line", &error);
        if (error) {
            g_error_free(error);
            error = nullptr;

            return false;
        }

        return result;
    }

    // int ConfigFile::read_separator_size()
    //{
    // return m_separator_size;
    //}

    int ConfigFile::read_separator_margin()
    {
        GError* error = nullptr;
        int value = g_key_file_get_integer(m_key_file, "separator", "separator_margin", &error);
        if (error) {
            g_error_free(error);
            error = nullptr;

            return 0;  // m_separator_margin;
        }

        return value;
    }

    // int ConfigFile::read_anchor_margin()
    //{
    ////
    // return m_anchor_margin;
    //}

    // float ConfigFile::get_animation_delay()
    //{
    ////
    // return m_animation_delay;
    //}

    // float ConfigFile::get_hide_delay()
    //{
    ////
    // return m_hide_delay;
    //}

    int ConfigFile::read_separator_size()
    {
        GError* error = nullptr;
        int value = g_key_file_get_integer(m_key_file, "separator", "separator_size", &error);
        if (error) {
            g_error_free(error);
            error = nullptr;

            return m_separator_size;
        }

        return value;
    }

    int ConfigFile::read_anchor_margin()
    {
        GError* error = nullptr;
        int value = g_key_file_get_integer(m_key_file, "dock", "anchor_margin", &error);
        if (error) {
            g_error_free(error);
            error = nullptr;

            return m_anchor_margin;
        }

        return value;
    }

    float ConfigFile::read_animation_delay()
    {
        GError* error = nullptr;
        double value = g_key_file_get_double(m_key_file, "dock", "animation_delay", &error);
        if (error) {
            g_error_free(error);
            error = nullptr;

            return m_animation_delay;
        }

        return value;
    }

    float ConfigFile::read_hide_delay()
    {
        GError* error = nullptr;
        double value = g_key_file_get_double(m_key_file, "dock", "hide_delay", &error);
        if (error) {
            g_error_free(error);
            error = nullptr;

            return m_hide_delay;
        }

        return value;
    }
}  // namespace docklight
