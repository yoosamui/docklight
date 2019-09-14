#pragma once

#include <string>
#include <vector>
#include "common.h"
#include "components/dockitem.h"

DL_NS_BEGIN

namespace desktopfile_util
{
    bool get_app_info(appinfo_t& info);
    bool get_app_info(WnckWindow* window, appinfo_t& info);

}  // namespace desktopfile_util

DL_NS_END
