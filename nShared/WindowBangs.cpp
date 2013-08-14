/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  WindowBangs.cpp
 *  The nModules Project
 *
 *  Bangs for drawable windows.
 *   
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "LiteStep.h"
#include "WindowBangs.h"
#include <strsafe.h>
#include "../nCoreCom/Core.h"


namespace WindowBangs
{
    // Specified by the module during _Register. Used to map names -> drawables.
    Window* (*drawableFinder)(LPCTSTR) = nullptr;

    BangItem BangMap[] =
    {
        { _T("Hide"),                       Hide                       },
        { _T("Show"),                       Show                       },
        { _T("On"),                         On                         },
        { _T("Off"),                        Off                        },
        { _T("Move"),                       Move                       },
        { _T("Size"),                       Size                       },
        { _T("Position"),                   Position                   },
        { _T("SetAlwaysOnTop"),             SetAlwaysOnTop             },
        { _T("SetClickThrough"),            SetClickThrough            },
        { _T("SetParent"),                  SetParent                  },
        { _T("SetText"),                    SetText                    }
    };
}


using namespace LiteStep;


/// <summary>
/// Registers bangs.
/// </summary>
void WindowBangs::Register(LPCTSTR prefix, Window* (*drawableFinder)(LPCTSTR))
{
    TCHAR bangName[64];
    WindowBangs::drawableFinder = drawableFinder;
    for (BangItem item : BangMap) {
        StringCchPrintf(bangName, _countof(bangName), _T("!%s%s"), prefix, item.name);
        LiteStep::AddBangCommand(bangName, item.command);
    }
}


/// <summary>
/// Unregisters bangs.
/// </summary>
void WindowBangs::UnRegister(LPCTSTR prefix) {
    TCHAR bangName[64];
    for (BangItem item : BangMap) {
        StringCchPrintf(bangName, _countof(bangName), _T("!%s%s"), prefix, item.name);
        LiteStep::RemoveBangCommand(bangName);
    }
    WindowBangs::drawableFinder = nullptr;
}


/// <summary>
/// Hides the window.
/// </summary>
void WindowBangs::Hide(HWND, LPCTSTR args) {
    Window* drawable = drawableFinder(args);
    if (drawable != nullptr) {
        drawable->Hide();
    }
}


/// <summary>
/// Shows the window.
/// </summary>
void WindowBangs::Show(HWND, LPCTSTR args) {
    Window* drawable = drawableFinder(args);
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
void WindowBangs::Move(HWND, LPCTSTR args) {
    TCHAR prefix[MAX_RCCOMMAND], xstr[MAX_RCCOMMAND], ystr[MAX_RCCOMMAND],
        durationstr[MAX_RCCOMMAND], easingstr[MAX_RCCOMMAND];
    LPTSTR tokens [] = { prefix, xstr, ystr, durationstr, easingstr };

    int numTokens = LiteStep::CommandTokenize(args, tokens, _countof(tokens), nullptr);

    if (numTokens == 3 || numTokens == 4 || numTokens == 5) {
        Window* drawable = drawableFinder(prefix);
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
                    if (ParseCoordinate(durationstr, &duration)) {
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
void WindowBangs::Size(HWND, LPCTSTR args) {
    TCHAR prefix[MAX_RCCOMMAND], widthstr[MAX_RCCOMMAND], heightstr[MAX_RCCOMMAND],
        durationstr[MAX_RCCOMMAND], easingstr[MAX_RCCOMMAND];
    LPTSTR tokens [] = { prefix, widthstr, heightstr, durationstr, easingstr };

    int numTokens = LiteStep::CommandTokenize(args, tokens, _countof(tokens), nullptr);

    if (numTokens == 3 || numTokens == 4 || numTokens == 5) {
        Window* drawable = drawableFinder(prefix);
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
                    if (ParseCoordinate(durationstr, &duration)) {
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
void WindowBangs::Position(HWND, LPCTSTR args) {
    TCHAR prefix[MAX_RCCOMMAND], xstr[MAX_RCCOMMAND], ystr[MAX_RCCOMMAND],
        widthstr[MAX_RCCOMMAND], heightstr[MAX_RCCOMMAND],
        durationstr[MAX_RCCOMMAND], easingstr[MAX_RCCOMMAND];
    LPTSTR tokens [] = { prefix, xstr, ystr, widthstr, heightstr, durationstr, easingstr };

    int numTokens = LiteStep::CommandTokenize(args, tokens, _countof(tokens), nullptr);

    if (numTokens == 5 || numTokens == 6 || numTokens == 7) {
        Window* drawable = drawableFinder(prefix);
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
                    if (ParseCoordinate(durationstr, &duration)) {
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
void WindowBangs::SetAlwaysOnTop(HWND, LPCTSTR args) {
    TCHAR prefix[MAX_RCCOMMAND], value[MAX_RCCOMMAND];
    LPTSTR tokens [] = { prefix, value };

    if (LiteStep::CommandTokenize(args, tokens, _countof(tokens), nullptr) == 2) {
        Window* drawable = drawableFinder(prefix);
        if (drawable != nullptr) {
            drawable->SetAlwaysOnTop(LiteStep::ParseBool(value));
        }
    }
}


/// <summary>
/// Modifies the windows clickthrough setting.
/// </summary>
void WindowBangs::SetClickThrough(HWND, LPCTSTR args) {
    TCHAR prefix[MAX_RCCOMMAND], value[MAX_RCCOMMAND];
    LPTSTR tokens [] = { prefix, value };

    if (LiteStep::CommandTokenize(args, tokens, _countof(tokens), nullptr) == 2) {
        Window* drawable = drawableFinder(prefix);
        if (drawable != nullptr) {
            drawable->SetClickThrough(LiteStep::ParseBool(value));
        }
    }
}


/// <summary>
/// Changes the windows parent.
/// </summary>
void WindowBangs::SetParent(HWND, LPCTSTR args) {
    TCHAR prefix[MAX_RCCOMMAND], value[MAX_RCCOMMAND];
    LPTSTR tokens [] = { prefix, value };

    if (LiteStep::CommandTokenize(args, tokens, _countof(tokens), nullptr) == 2) {
        Window* drawable = drawableFinder(prefix);
        if (drawable != nullptr) {
            //drawable->GetWindow()->SetParent(value);
        }
    }
}


/// <summary>
/// Changes the windows parent.
/// </summary>
void WindowBangs::SetText(HWND, LPCTSTR args)
{
    TCHAR prefix[MAX_RCCOMMAND], value[MAX_LINE_LENGTH];
    LPTSTR tokens [] = { prefix, value };

    if (LiteStep::CommandTokenize(args, tokens, _countof(tokens), nullptr) == 2)
    {
        Window* drawable = drawableFinder(prefix);
        if (drawable != nullptr)
        {
            drawable->SetText(value);
        }
    }
}

/// <summary>
/// Adds an event handler.
/// </summary>
void WindowBangs::On(HWND, LPCTSTR /* args */)
{
}


/// <summary>
/// Removes an event handler.
/// </summary>
void WindowBangs::Off(HWND, LPCTSTR /* args */)
{
}