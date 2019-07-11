#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>
#include <functional>

#define DL_NS_BEGIN    \
  namespace docklight {
#define DL_NS_END \
  }


using namespace std;


enum class dock_location: unsigned int
{
    bottom = 0,
    left,
    top,
    right,
};


