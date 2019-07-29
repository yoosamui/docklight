#pragma once

#include <gmodule.h>
#include <gtkmm/enums.h>
#include "common.h"

DL_NS_BEGIN

typedef struct {
    void* m_this;
} static_members_t;

class Autohide
{
  public:
    Autohide();
    ~Autohide();

  private:
    static static_members_t m_stm;
};

DL_NS_END

