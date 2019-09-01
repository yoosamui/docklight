#pragma once

#include "common.h"
DL_NS_BEGIN

namespace string_util
{
    bool is_numeric(const char *arg);
    bool is_numeric(const string &s);
    bool is_matching(const string &s, const char *characters);
    string remove_extension(const string &text, const string extensions[]);
    string string_to_lower(const char *strp);
}  // namespace string_util

DL_NS_END
