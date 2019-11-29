#pragma once

#include <string.h>
#include <algorithm>
#include <sstream>
#include "common.h"

DL_NS_BEGIN

namespace string_util
{
    vector<string> split(const string &source, char delim);
    string split_find_token(const string &source, const string &token, char delim);
    bool split_token_exist(const string &source, const string &token, const string &delimeters);
    bool is_numeric(const char *arg);
    bool is_numeric(const string &s);
    bool is_matching(const string &s, const char *characters);
    string remove_extension(const string &text, const string extensions[]);
    string string_to_lower(const string &text);
}  // namespace string_util

DL_NS_END
