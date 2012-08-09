/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Settings.h                                                    August, 2012
 *  The nModules Project
 *  
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#ifndef SETTINGS_H
#define SETTINGS_H

#include "DesktopPainter.hpp"

namespace Settings {
    void Load();
    DesktopPainter::EasingType EasingFromString(LPCSTR pszTransition);
    DesktopPainter::TransitionType TransitionTypeFromString(LPCSTR pszTransition);
}

#endif /* SETTINGS_H */
