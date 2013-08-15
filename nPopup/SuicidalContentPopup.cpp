/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  SuicidalContentPopup.cpp
 *  The nModules Project
 *
 *  A content popup which destroys itself when closed.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "SuicidalContentPopup.hpp"


SuicidalContentPopup::SuicidalContentPopup(LPCTSTR path, LPCTSTR title, LPCTSTR prefix) : ContentPopup(path, true, title, L"", prefix)
{
}


void SuicidalContentPopup::PostClose()
{
    delete this;
}
