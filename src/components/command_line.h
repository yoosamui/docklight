#pragma once

#include <getopt.h>
#include <stdio.h>
#include "common.h"
#include <gmodule.h>

DL_NS_BEGIN

namespace cli
{
typedef struct {
    char arg = 0;
    char* string_value{nullptr};
    int int_value = 0;
} result_t;

class arguments
{
  public:
    arguments(int argc, char** argv);
    ~arguments();
    bool validate();
    const GSList* get_results();

  private:
    int m_argc;
    char** m_argv;
    GSList* m_list = nullptr;
    int m_error_code = EXIT_SUCCESS;

    int parse(int opt,const char* coptarg);
    int set_monitor(const char* coptarg);
    int set_position(const char* optarg);
    int set_alignment(const char* coptarg);
    int set_location(const char* coptarg);
    bool is_numeric(const char* coptarg);
    int set_resize(const char* coptarg);
    void add(const char arg, const char* cvalue, int ivalue);
    void usage();
};
}  // namespace cli
DL_NS_END

