#include "utils/pixbuf.h"
#include <glibmm/fileutils.h>
DL_NS_BEGIN

namespace pixbuf_util
{
    const Glib::RefPtr<Gdk::Pixbuf> get_from_file(const std::string& filename,
                                                  int width, int height)
    {
        Glib::RefPtr<Gdk::Pixbuf> result;

        try {
            result =
                Gdk::Pixbuf::create_from_file(filename, width, height, true);
        } catch (const Glib::FileError& ex) {
            g_error("FileError: %s", ex.what().c_str());
        } catch (const Gdk::PixbufError& ex) {
            g_error("PixbufError: %s", ex.what().c_str());
        }

        return result;
    }

}  // namespace pixbuf_util

DL_NS_END

