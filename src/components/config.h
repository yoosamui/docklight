#pragma once

#include <gmodule.h>
#include "common.h"

DL_NS_BEGIN
namespace config
{
    void load(const GSList* args_list);
    dock_location get_dock_location();

}  // namespace config
DL_NS_END

