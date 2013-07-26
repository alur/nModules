/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  DrawableBangs.cpp
 *  The nModules Project
 *
 *  Bangs for drawable windows.
 *   
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "LiteStep.h"
#include "DrawableWindowBangs.h"
#include <strsafe.h>
#include "../nCoreCom/Core.h"


namespace DrawableWindowBangs {
    // Specified by the module during _Register. Used to map names -> drawables.
    DrawableWindow* (*drawableFinder)(LPCSTR) = nullptr;

    BangItem BangMap[] = {
        { "Hide",                       Hide                       },
        { "Show",                       Show                       },
        { "On",                         On                         },
        { "Off",                        Off                        },
        { "Move",                       Move                       },
        { "Size",                       Size                       },
        { "Position",                   Position                   },
        { "SetAlwaysOnTop",             SetAlwaysOnTop             },
        { "SetClickThrough",            SetClickThrough            },
        { "SetParent",                  SetParent                  },
        { "SetText",                    SetText                    }
    };
}


/// <summary>
/// Registers bangs.
/// </summary>
void DrawableWindowBangs::_Register(LPCSTR prefix, DrawableWindow* (*drawableFinder)(LPCSTR)) {
    TCHAR bangName[64];
    DrawableWindowBangs::drawableFinder = drawableFinder;
    for (BangItem item : BangMap) {
        StringCchPrintf(bangName, _countof(bangName), "!%s%s", prefix, item.name);
        LiteStep::AddBangCommand(bangName, item.command);
    }
}


/// <summary>
/// Unregisters bangs.
/// </summary>
void DrawableWindowBangs::_UnRegister(LPCSTR prefix) {
    TCHAR bangName[64];
    for (BangItem item : BangMap) {
        StringCchPrintf(bangName, _countof(bangName), "!%s%s", prefix, item.name);
        LiteStep::RemoveBangCommand(bangName);
    }
    DrawableWindowBangs::drawableFinder = nullptr;
}


/// <summary>
/// Hides the window.
/// </summary>
void DrawableWindowBangs::Hide(HWND, LPCSTR args) {
    DrawableWindow* drawable = drawableFinder(args);
    if (drawable != nullptr) {
        drawable->Hide();
    }
}


/// <summary>
/// Shows the window.
/// </summary>
void DrawableWindowBangs::Show(HWND, LPCSTR args) {
    DrawableWindow* drawable = drawableFinder(args);
    if (drawable != nullptr) {
        drawable->Show();
        if (drawable->IsChild()) {
            drawable->Repaint();
        }
    }
}


/// <summary>
/// Shows the window.
/// </summary>
void DrawableWindowBangs::Move(HWND, LPCSTR args) {
    TCHAR prefix[MAX_RCCOMMAND], xstr[MAX_RCCOMMAND], ystr[MAX_RCCOMMAND];
    LPTSTR tokens [] = { prefix, xstr, ystr };

    if (LiteStep::CommandTokenize(args, tokens, _countof(tokens), nullptr) == 3) {
        DrawableWindow* drawable = drawableFinder(prefix);
        if (drawable != nullptr) {
            drawable->Move(atoi(xstr), atoi(ystr));
            if (drawable->IsChild()) {
                drawable->Repaint();
            }
        }
    }
}


/// <summary>
/// Shows the window.
/// </summary>
void DrawableWindowBangs::Size(HWND, LPCSTR args) {
    TCHAR prefix[MAX_RCCOMMAND], width[MAX_RCCOMMAND], height[MAX_RCCOMMAND];
    LPTSTR tokens [] = { prefix, width, height };

    if (LiteStep::CommandTokenize(args, tokens, _countof(tokens), nullptr) == 3) {
        DrawableWindow* drawable = drawableFinder(prefix);
        if (drawable != nullptr) {
            drawable->Resize(atoi(width), atoi(height));
            if (drawable->IsChild()) {
                drawable->Repaint();
            }
        }
    }
}


/// <summary>
/// Shows the window.
/// </summary>
void DrawableWindowBangs::Position(HWND, LPCSTR args) {
    TCHAR prefix[MAX_RCCOMMAND], xstr[MAX_RCCOMMAND], ystr[MAX_RCCOMMAND],
        width[MAX_RCCOMMAND], height[MAX_RCCOMMAND];
    LPTSTR tokens [] = { prefix, xstr, ystr, width, height };

    if (LiteStep::CommandTokenize(args, tokens, _countof(tokens), nullptr) == 5) {
        DrawableWindow* drawable = drawableFinder(prefix);
        if (drawable != nullptr) {
            drawable->SetPosition(atoi(xstr), atoi(ystr), atoi(width), atoi(height));
            drawable->Repaint();
        }
    }
}


/// <summary>
/// SetAlwaysOnTop
/// </summary>
void DrawableWindowBangs::SetAlwaysOnTop(HWND, LPCSTR args) {
    TCHAR prefix[MAX_RCCOMMAND], value[MAX_RCCOMMAND];
    LPTSTR tokens [] = { prefix, value };

    if (LiteStep::CommandTokenize(args, tokens, _countof(tokens), nullptr) == 2) {
        DrawableWindow* drawable = drawableFinder(prefix);
        if (drawable != nullptr) {
            drawable->SetAlwaysOnTop(nCore::InputParsing::ParseBool(value));
        }
    }
}


/// <summary>
/// Modifies the windows clickthrough setting.
/// </summary>
void DrawableWindowBangs::SetClickThrough(HWND, LPCSTR args) {
    TCHAR prefix[MAX_RCCOMMAND], value[MAX_RCCOMMAND];
    LPTSTR tokens [] = { prefix, value };

    if (LiteStep::CommandTokenize(args, tokens, _countof(tokens), nullptr) == 2) {
        DrawableWindow* drawable = drawableFinder(prefix);
        if (drawable != nullptr) {
            drawable->SetClickThrough(nCore::InputParsing::ParseBool(value));
        }
    }
}


/// <summary>
/// Changes the windows parent.
/// </summary>
void DrawableWindowBangs::SetParent(HWND, LPCSTR args) {
    TCHAR prefix[MAX_RCCOMMAND], value[MAX_RCCOMMAND];
    LPTSTR tokens [] = { prefix, value };

    if (LiteStep::CommandTokenize(args, tokens, _countof(tokens), nullptr) == 2) {
        DrawableWindow* drawable = drawableFinder(prefix);
        if (drawable != nullptr) {
            //drawable->GetWindow()->SetParent(value);
        }
    }
}


/// <summary>
/// Changes the windows parent.
/// </summary>
void DrawableWindowBangs::SetText(HWND, LPCSTR args) {
    TCHAR prefix[MAX_RCCOMMAND], value[MAX_LINE_LENGTH];
    LPTSTR tokens [] = { prefix, value };

    if (LiteStep::CommandTokenize(args, tokens, _countof(tokens), nullptr) == 2) {
        DrawableWindow* drawable = drawableFinder(prefix);
        if (drawable != nullptr) {
            WCHAR realValue[MAX_LINE_LENGTH];
            MultiByteToWideChar(CP_ACP, 0, value, (int)strlen(value)+1, realValue, _countof(realValue));
            drawable->SetText(realValue);
        }
    }
}

/// <summary>
/// Adds an event handler.
/// </summary>
void DrawableWindowBangs::On(HWND, LPCSTR /* args */) {
}


/// <summary>
/// Removes an event handler.
/// </summary>
void DrawableWindowBangs::Off(HWND, LPCSTR /* args */) {
}
