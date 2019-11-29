#include "utils/string.h"

DL_NS_BEGIN

namespace string_util
{
    vector<string> split(const string &source, char delim)
    {
        stringstream string_stream(source);
        string item;
        vector<string> elems;
        while (getline(string_stream, item, delim)) {
            elems.push_back(move(item));
        }
        return elems;
    }

    string split_find_token(const string &source, const string &token, char delim)
    {
        stringstream string_stream(source);
        string item;
        while (getline(string_stream, item, delim)) {
            if (item == token) {
                return item;
            }
        }
        return {};
    }

    bool split_token_exist(const string &source, const string &token, const string &delimeters)
    {
        string item;

        for (size_t i = 0; i < delimeters.size(); i++) {
            stringstream string_stream(source);
            while (getline(string_stream, item, delimeters[i])) {
                if (item == token) {
                    return true;
                }
            }
        }

        return false;
    }

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
        string::const_iterator it = s.begin();
        while (it != s.end() && isdigit(*it)) ++it;
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

    string string_to_lower(const string &text)
    {
        string str = text;
        transform(str.begin(), str.end(), str.begin(), ::tolower);
        return str;
    }
}  // namespace string_util
DL_NS_END
