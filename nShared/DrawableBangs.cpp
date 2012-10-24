/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  DrawableBangs.cpp
 *  The nModules Project
 *
 *  Bangs for drawable windows.
 *   
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "LiteStep.h"
#include "DrawableBangs.h"
#include <strsafe.h>

namespace DrawableBangs {
    // Specified by the module during _Register. Used to map names -> drawables.
    Drawable* (*drawableFinder)(LPCSTR);

    BangItem BangMap[] = {
        { "Hide",                       Hide                       },
        { "Show",                       Show                       },
        { "On",                         On                         },
        { "Off",                        Off                        },
        { NULL,                         NULL                       }
    };
}


/// <summary>
/// Registers bangs.
/// </summary>
void DrawableBangs::_Register(LPCSTR prefix, Drawable* (*drawableFinder)(LPCSTR)) {
    char szBangName[64];
    DrawableBangs::drawableFinder = drawableFinder;
    for (int i = 0; BangMap[i].pCommand != NULL; i++) {
        StringCchPrintf(szBangName, sizeof(szBangName), "!%s%s", prefix, BangMap[i].szName);
        LiteStep::AddBangCommand(szBangName, BangMap[i].pCommand);
    }
}


/// <summary>
/// Unregisters bangs.
/// </summary>
void DrawableBangs::_UnRegister(LPCSTR prefix) {
    char szBangName[64];
    for (int i = 0; BangMap[i].pCommand != NULL; i++) {
        StringCchPrintf(szBangName, sizeof(szBangName), "!%s%s", prefix, BangMap[i].szName);
        LiteStep::RemoveBangCommand(szBangName);
    }
    DrawableBangs::drawableFinder = NULL;
}


/// <summary>
/// Hides the window.
/// </summary>
void DrawableBangs::Hide(HWND, LPCSTR args) {
    Drawable* drawable = drawableFinder(args);
    if (drawable != NULL) {
        drawable->GetWindow()->Hide();
    }
}


/// <summary>
/// Shows the window.
/// </summary>
void DrawableBangs::Show(HWND, LPCSTR args) {
    Drawable* drawable = drawableFinder(args);
    if (drawable != NULL) {
        drawable->GetWindow()->Show();
    }
}


/// <summary>
/// Adds an event handler.
/// </summary>
void DrawableBangs::On(HWND, LPCSTR args) {
}


/// <summary>
/// Removes an event handler.
/// </summary>
void DrawableBangs::Off(HWND, LPCSTR args) {
}
