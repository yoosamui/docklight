#pragma once

#include "common.h"

DL_NS_BEGIN

namespace system_util
{
    bool file_exists(const std::string& name);
    string file_exists(const string& directory, string& file_name);
    const string get_current_path(const string& str);
    bool create_directory_if_not_exitst(const char* directory_name);
    const string get_current_user();

}  // namespace system_util

DL_NS_END
