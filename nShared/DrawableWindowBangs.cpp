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
    TCHAR prefix[MAX_RCCOMMAND], xstr[MAX_RCCOMMAND], ystr[MAX_RCCOMMAND],
        durationstr[MAX_RCCOMMAND], easingstr[MAX_RCCOMMAND];
    LPTSTR tokens [] = { prefix, xstr, ystr, durationstr, easingstr };

    using namespace nCore::InputParsing;

    int numTokens = LiteStep::CommandTokenize(args, tokens, _countof(tokens), nullptr);

    if (numTokens == 3 || numTokens == 4 || numTokens == 5) {
        DrawableWindow* drawable = drawableFinder(prefix);
        if (drawable != nullptr) {
            int x = 0, y = 0;

            if (ParseCoordinate(xstr, &x) && ParseCoordinate(ystr, &y)) {
                if (numTokens == 3) {
                    drawable->Move(x, y);
                    if (drawable->IsChild()) {
                        drawable->Repaint();
                    }
                }
                else {
                    int duration = 0;
                    if (nCore::InputParsing::ParseCoordinate(durationstr, &duration)) {
                        drawable->SetAnimation(x, y, drawable->GetDrawingSettings()->width, 
                            drawable->GetDrawingSettings()->height, duration,
                            numTokens == 4 ? Easing::Type::Linear : Easing::EasingFromString(easingstr));
                    }
                }
            }
        }
    }
}


/// <summary>
/// Shows the window.
/// </summary>
void DrawableWindowBangs::Size(HWND, LPCSTR args) {
    TCHAR prefix[MAX_RCCOMMAND], widthstr[MAX_RCCOMMAND], heightstr[MAX_RCCOMMAND],
        durationstr[MAX_RCCOMMAND], easingstr[MAX_RCCOMMAND];
    LPTSTR tokens [] = { prefix, widthstr, heightstr, durationstr, easingstr };

    using namespace nCore::InputParsing;

    int numTokens = LiteStep::CommandTokenize(args, tokens, _countof(tokens), nullptr);

    if (numTokens == 3 || numTokens == 4 || numTokens == 5) {
        DrawableWindow* drawable = drawableFinder(prefix);
        if (drawable != nullptr) {
            int width = 0, height = 0;

            if (ParseLength(widthstr, &width) && ParseLength(heightstr, &height)) {
                if (numTokens == 3) {
                    drawable->Resize(width, height);
                    if (drawable->IsChild()) {
                        drawable->Repaint();
                    }
                }
                else {
                    int duration = 0;
                    if (nCore::InputParsing::ParseCoordinate(durationstr, &duration)) {
                        drawable->SetAnimation(drawable->GetDrawingSettings()->x, 
                            drawable->GetDrawingSettings()->y, width, height, duration,
                            numTokens == 4 ? Easing::Type::Linear : Easing::EasingFromString(easingstr));
                    }
                }
            }
        }
    }
}


/// <summary>
/// Shows the window.
/// </summary>
void DrawableWindowBangs::Position(HWND, LPCSTR args) {
    TCHAR prefix[MAX_RCCOMMAND], xstr[MAX_RCCOMMAND], ystr[MAX_RCCOMMAND],
        widthstr[MAX_RCCOMMAND], heightstr[MAX_RCCOMMAND],
        durationstr[MAX_RCCOMMAND], easingstr[MAX_RCCOMMAND];
    LPTSTR tokens [] = { prefix, xstr, ystr, widthstr, heightstr, durationstr, easingstr };

    using namespace nCore::InputParsing;

    int numTokens = LiteStep::CommandTokenize(args, tokens, _countof(tokens), nullptr);

    if (numTokens == 5 || numTokens == 6 || numTokens == 7) {
        DrawableWindow* drawable = drawableFinder(prefix);
        if (drawable != nullptr) {
            int width = 0, height = 0, x = 0, y = 0;

            if (ParseLength(widthstr, &width) && ParseLength(heightstr, &height)
                && ParseCoordinate(xstr, &x) && ParseCoordinate(ystr, &y)) {
                if (numTokens == 5) {
                    drawable->SetPosition(x, y, width, height);
                    if (drawable->IsChild()) {
                        drawable->Repaint();
                    }
                }
                else {
                    int duration = 0;
                    if (nCore::InputParsing::ParseCoordinate(durationstr, &duration)) {
                        drawable->SetAnimation(x, y, width, height, duration,
                            numTokens == 6 ? Easing::Type::Linear : Easing::EasingFromString(easingstr));
                    }
                }
            }
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
