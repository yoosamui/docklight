#include "components/panelbase.h"

DL_NS_BEGIN

int Panel_base::m_current_index;
Panel_base::Panel_base()
{
    Panel_base::m_current_index = 1;
}

int Panel_base::get_current_index()
{
    return m_current_index;
}
DL_NS_END
