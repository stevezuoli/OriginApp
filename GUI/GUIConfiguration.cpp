#include "GUI/GUIConfiguration.h"
GUIConfiguration::GUIConfiguration()
    : m_touchSlot(5)
{
}

const GUIConfiguration* GUIConfiguration::GetInstance()
{
    static GUIConfiguration s_guiConfiguration;
    return &s_guiConfiguration;
}

