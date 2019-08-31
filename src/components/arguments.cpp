#include "arguments.h"
#include <components/device.h>
#include <getopt.h>

#include "utils/file.h"
#include "utils/string.h"

DL_NS_BEGIN

namespace cli
{
    /**
     * Constructor
     */
    Arguments::Arguments(int argc, char **argv)
    {
        m_argc = argc;
        m_argv = argv;
    }

    /**
     * Destructor
     * free memory allocated by  GList
     */
    Arguments::~Arguments()
    {
        if (m_list == nullptr) {
            return;
        }

        for (GSList *l = m_list; l; l = l->next) {
            free(l->data);
        }
        g_slist_free(m_list);
        g_print("free Arguments class.\n");
    }

    /**
     * Validate the given Arguments by getopt
     */
    bool Arguments::validate()
    {
        // clang-format off
        const char* short_opts = {"m:l:p:a:r:c:h"};
        const option long_opts[] = {
            {"monitor", required_argument, nullptr, 'm'},
            {"location", required_argument, nullptr, 'l'},
            {"position", required_argument, nullptr, 'p'},
            {"alignment", required_argument, nullptr, 'a'},
            {"resize", required_argument, nullptr, 'r'},
            {"config", required_argument, nullptr, 'c'},
            {"help", optional_argument, nullptr, 'h'},
            {nullptr,0,nullptr,0}
        };
        // clang-format on

        int opt = 0;
        while ((opt = getopt_long(m_argc, m_argv, short_opts, long_opts, nullptr)) != -1) {
            if (parse(opt, optarg) == EXIT_FAILURE) {
                m_error_code = EXIT_FAILURE;
                this->usage();
                break;
            }
        }

        return m_error_code == EXIT_SUCCESS;
    };

    /**
     * Parse all options
     */
    int Arguments::parse(int opt, const char *coptarg)
    {
        m_error_code = EXIT_SUCCESS;

        // TODO: handle optional parameters.
        if (!coptarg) {
            return EXIT_FAILURE;
        }

        if ((char)opt == 'm') return this->set_monitor(coptarg);
        if ((char)opt == 'l') return this->set_location(coptarg);
        if ((char)opt == 'p') return this->set_position(coptarg);
        if ((char)opt == 'a') return this->set_alignment(coptarg);
        if ((char)opt == 'r') return this->set_icon_size(coptarg);
        if ((char)opt == 'c') return this->set_config(coptarg);

        if ((char)opt == 'h') {
            m_error_code = EXIT_FAILURE;
            return m_error_code;
        }

        return m_error_code;
    }

    /**
     * Validate the monitor number.
     * Returns an error if the parameter is not numeric or out of range
     */
    int Arguments::set_monitor(const char *coptarg)
    {
        if (!string_util::is_numeric(coptarg)) {
            g_warning("argument monitor allow only  numeric values.");
            m_error_code = EXIT_FAILURE;
            return m_error_code;
        }

        auto count = device::monitor::get_monitor_count();

        gint value = atoi(coptarg);
        if (value > count) {
            g_warning("Only %d monitors detected.", count);
            m_error_code = EXIT_FAILURE;
            return m_error_code;
        }

        this->add('m', coptarg, value);

        return EXIT_SUCCESS;
    }

    /**
     * Validate the dock location
     * locations are bottom, left, top or right
     */
    int Arguments::set_location(const char *coptarg)
    {
        m_error_code = EXIT_SUCCESS;
        if (strcmp("bottom", coptarg) == 0 || strcmp("left", coptarg) == 0 ||
            strcmp("top", coptarg) == 0 || strcmp("right", coptarg) == 0) {
            add('l', coptarg, 0);
            return m_error_code;
        }

        g_warning("invalid argument. please use bottom, left, top or right.");
        m_error_code = EXIT_FAILURE;
        return m_error_code;
    }

    /**
     * Validate the dock  position x or y coordinate
     */
    int Arguments::set_position(const char *coptarg)
    {
        if (!string_util::is_numeric(coptarg)) {
            g_warning("argument position allow only numeric values.");
            m_error_code = EXIT_FAILURE;
            return m_error_code;
        }

        add('p', coptarg, atoi(coptarg));

        m_error_code = EXIT_SUCCESS;
        return m_error_code;
    }

    /**
     * Validate the alignment
     * left, center or right
     * this will be converted in vertical mode to top center bottom
     */
    gint Arguments::set_alignment(const char *coptarg)
    {
        m_error_code = EXIT_SUCCESS;
        if (strcmp("start", coptarg) == 0 || strcmp("center", coptarg) == 0 ||
            strcmp("end", coptarg) == 0 || strcmp("fill", coptarg) == 0) {
            add('a', coptarg, 0);
            return m_error_code;
        }

        g_warning(
            "Argument alignment. please use start, end, center or fill "
            "center.");
        m_error_code = EXIT_FAILURE;
        return m_error_code;
        add('a', coptarg, 0);

        m_error_code = EXIT_SUCCESS;
        return m_error_code;
    }

    /**
     * Validate the resize argument
     */
    int Arguments::set_icon_size(const char *coptarg)
    {
        if (!string_util::is_numeric(coptarg)) {
            g_warning("argument icon size allow only numeric values.");
            m_error_code = EXIT_FAILURE;
            return m_error_code;
        }

        add('r', coptarg, atoi(coptarg));

        m_error_code = EXIT_SUCCESS;
        return m_error_code;
    }

    /**
     * Validate the configuartion file
     */
    int Arguments::set_config(const char *coptarg)
    {
        if (!file_util::is_file_exits(coptarg)) {
            g_warning("argument config. file could not be found.");
            m_error_code = EXIT_FAILURE;
            return m_error_code;
        }

        add('c', coptarg, 0);

        m_error_code = EXIT_SUCCESS;
        return m_error_code;
    }
    /**
     * Add the values to the result list
     */
    void Arguments::add(const char arg, const char *cvalue, int ivalue)
    {
        result_t *data = static_cast<result_t *>(malloc(sizeof(result_t)));
        if (!data) {
            return;
        }

        if (cvalue) {
            size_t size = strlen(cvalue) + 1;
            data->string_value = static_cast<char *>(malloc(size));
            if (!data->string_value) {
                delete data;
                return;
            }

            strcpy(data->string_value, cvalue);
        }

        data->arg = arg;
        data->int_value = ivalue;

        m_list = g_slist_append(m_list, data);
    }

    /**
     * gets the result options list
     */
    const GSList *Arguments::get_results() { return m_list; }

    void Arguments::usage()
    {
        // clang-format off
        g_print("--------------------------------\n");
        g_print(" docklight Arguments usage.\n");
        g_print("--------------------------------\n");
        g_print("--m --monitor number to dock.(0 - n) default active.\n");
        g_print("--l --location to dock (bottom, left, top or right) default bottom.\n");
        g_print("--p --position to dock. coordinate for x or y. default none\n");
        g_print("--a --alignment position  (left , center  or right) default center.\n");
        g_print("--r --resize if is set to 0 the dock window will not resized.\n");
        g_print("--c --config the path of the configuration file to use.\n");
        g_print("--h --help display this help.\n");
        g_print("\nExamples:\n");
        g_print("use monitor 1: docklight --m 1\n");
        g_print("use location:  docklight --l top --c /home/user/.config/docklight/docklight.conf\n");
        g_print("use position:  docklight --l top --p 10\n");
        g_print("\n\n");

        // clang-format on
    }
}  // namespace cli

DL_NS_END
