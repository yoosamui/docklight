#pragma once
#define WNCK_I_KNOW_THIS_IS_UNSTABLE 1

#include <libwnck/libwnck.h>
#include "common.h"
#include "components/dockitem.h"

DL_NS_BEGIN
namespace launcher_util
{
    bool launch(const string& appname, const string& desktop_file);
}  // namespace launcher_util

DL_NS_END

