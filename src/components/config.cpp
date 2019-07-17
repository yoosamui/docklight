
#include "components/config.h"

DL_NS_BEGIN
namespace config
{
    dock_location m_location = dock_location::bottom;

    void load(const GSList* args_list) {}
    dock_location get_dock_location() { return m_location; }

}  // namespace config
DL_NS_END
