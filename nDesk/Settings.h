/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Settings.h
 *  The nModules Project
 *  
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "DesktopPainter.hpp"

namespace nDesk {
    namespace Settings {
        void Load();
        DesktopPainter::TransitionType TransitionTypeFromString(LPCSTR pszTransition);
    }
}