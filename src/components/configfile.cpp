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

        GError* error = nullptr;
        m_key_file = g_key_file_new();
        auto found = g_key_file_load_from_file(m_key_file, filepath.c_str(),
                                               GKeyFileFlags::G_KEY_FILE_NONE, &error);
        if (!found) {
            if (error) {
                g_warning("Load: configuration could not be found. %s %s\n", error->message,
                          filepath.c_str());
                g_error_free(error);
                error = nullptr;
                g_key_file_free(m_key_file);
                found = false;
            }

            g_key_file_free(m_key_file);
            m_key_file = nullptr;
        }
        return found;
    }

    std::string ConfigFile::read_filepath()
    {
        std::string user_name = System::get_current_user();

        char config_dir[512];
        sprintf(config_dir, "/home/%s/.config/docklight", user_name.c_str());

        System::create_directory_if_not_exitst(config_dir);

        char buff[PATH_MAX];
        sprintf(buff, "%s/%s", config_dir, m_filename.c_str());

        return buff;
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

            return m_separator_margin;
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
