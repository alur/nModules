/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  SuicidalContentPopup.cpp
 *  The nModules Project
 *
 *  A content popup which destroys itself when closed.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "SuicidalContentPopup.hpp"


SuicidalContentPopup::SuicidalContentPopup(LPCSTR path, LPCSTR title, LPCSTR prefix) : ContentPopup(path, true, title, "", prefix)
{
}


void SuicidalContentPopup::Close()
{
    Popup::Close();
    delete this;
}
