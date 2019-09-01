#include "utils/string.h"
#include <string.h>
#include <algorithm>
DL_NS_BEGIN

namespace string_util
{
    /**
     * Validate numeric strings.
     * returns true id the string is numeric, otherwise false.
     */
    bool is_numeric(const char *arg)
    {
        string s(arg);
        return is_numeric(s);
    }

    /**
     * Validate numeric strings.
     * returns true id the string is numeric, otherwise false.
     */
    bool is_numeric(const string &s)
    {
        std::string::const_iterator it = s.begin();
        while (it != s.end() && std::isdigit(*it)) ++it;
        return !s.empty() && it == s.end();
    }

    bool is_matching(const string &s, const char *characters)
    {
        return (strspn(s.c_str(), characters) == s.size());
    }

    string remove_extension(const string &s, const string extensions[])
    {
        string text(s);

        // find extensions
        for (int i = 0; i < (int)extensions->size() - 1; i++) {
            string e = extensions[i];
            size_t found = text.find(e);
            if (found != string::npos) {
                // let's replace the extension with an empty string:
                text.replace(text.find(e), text.length(), "");
            }
        }
        return text;
    }

    string string_to_lower(const char *strp)
    {
        std::string str = strp;
        transform(str.begin(), str.end(), str.begin(), ::tolower);

        return str;
    }
}  // namespace string_util
DL_NS_END
