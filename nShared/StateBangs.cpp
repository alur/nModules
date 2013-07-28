//--------------------------------------------------------------------------------------
// StateBangs.cpp
// The nModules Project
//
// Bangs for states
//
//--------------------------------------------------------------------------------------
#include "StateBangs.h"
#include "LiteStep.h"
#include <strsafe.h>
#include "../nCoreCom/Core.h"


static std::function<DrawableWindow* (LPCSTR)> windowFinder;


static State *FindState(LPCTSTR *args, int numArgs, DrawableWindow *&window) {
    char buffer[MAX_RCCOMMAND];
    int numTokens = LiteStep::CommandTokenize(*args, nullptr, 0, nullptr);

    if (numTokens == numArgs + 1 || numTokens == numArgs + 2) {
        LiteStep::GetToken(*args, buffer, args, 0);
        window = windowFinder(buffer);
        if (window) {
            if (numTokens == numArgs + 2) {
                LiteStep::GetToken(*args, buffer, args, 0);
                return window->GetState(buffer);
            }
            else {
                return window->GetState("");
            }
        }
    }

    return nullptr;
}


static struct BangItem {
    BangItem(LPCTSTR name, LiteStep::BANGCOMMANDPROC proc) {
        this->name = name;
        this->proc = proc;
    }

    LPCTSTR name;
    LiteStep::BANGCOMMANDPROC proc;
} BangMap [] = {
    BangItem(TEXT("SetCornerRadiusX"),           [] (HWND, LPCTSTR args) -> void {
        DrawableWindow *window = nullptr;
        State *state = FindState(&args, 1, window);
        if (state) {
            char arg[MAX_RCCOMMAND];
            LiteStep::GetToken(args, arg, nullptr, 0);
            state->SetCornerRadiusX(float(atof(arg)));
            window->Repaint();
        }
    }),
    BangItem(TEXT("SetCornerRadiusY"),           [] (HWND, LPCTSTR args) -> void {
        DrawableWindow *window = nullptr;
        State *state = FindState(&args, 1, window);
        if (state) {
            char arg[MAX_RCCOMMAND];
            LiteStep::GetToken(args, arg, nullptr, 0);
            state->SetCornerRadiusY(float(atof(arg)));
            window->Repaint();
        }
    }),
    BangItem(TEXT("SetOutlineWidth"),            [] (HWND, LPCTSTR args) -> void {
        DrawableWindow *window = nullptr;
        State *state = FindState(&args, 1, window);
        if (state) {
            char arg[MAX_RCCOMMAND];
            LiteStep::GetToken(args, arg, nullptr, 0);
            state->SetOutlineWidth(float(atof(arg)));
            window->Repaint();
        }
    }),
    /*BangItem(TEXT("SetFont"),                    [] (HWND, LPCTSTR args) -> void {
        DrawableWindow *window = nullptr;
        State *state = FindState(&args, 1, window);
        if (state) {
            char arg[MAX_RCCOMMAND];
            LiteStep::GetToken(args, arg, nullptr, 0);
            //state->SetFont(arg);
        }
    }),
    BangItem(TEXT("SetFontSize"),                [] (HWND, LPCTSTR args) -> void {
        DrawableWindow *window = nullptr;
        State *state = FindState(&args, 1, window);
        if (state) {
            char arg[MAX_RCCOMMAND];
            LiteStep::GetToken(args, arg, nullptr, 0);
            //state->SetFontSize(strtof(arg, nullptr));
        }
    }),
    BangItem(TEXT("SetFontStretch"),             [] (HWND, LPCTSTR args) -> void {
        DrawableWindow *window = nullptr;
        State *state = FindState(&args, 1, window);
        if (state) {
            char arg[MAX_RCCOMMAND];
            LiteStep::GetToken(args, arg, nullptr, 0);
            //state->SetFontStretch(StateSettings::ParseFontStretch(arg));
        }
    }),
    BangItem(TEXT("SetFontStyle"),               [] (HWND, LPCTSTR args) -> void {
        DrawableWindow *window = nullptr;
        State *state = FindState(&args, 1, window);
        if (state) {
            char arg[MAX_RCCOMMAND];
            LiteStep::GetToken(args, arg, nullptr, 0);
            //state->SetFontStyle(StateSettings::ParseFontStyle(arg));
        }
    }),
    BangItem(TEXT("SetFontWeight"),              [] (HWND, LPCTSTR args) -> void {
        DrawableWindow *window = nullptr;
        State *state = FindState(&args, 1, window);
        if (state) {
            char arg[MAX_RCCOMMAND];
            LiteStep::GetToken(args, arg, nullptr, 0);
            //state->SetFontWeight(StateSettings::ParseFontWeight(arg));
        }
    }),*/
    BangItem(TEXT("SetReadingDirection"),        [] (HWND, LPCTSTR args) -> void {
        DrawableWindow *window = nullptr;
        State *state = FindState(&args, 1, window);
        if (state) {
            char arg[MAX_RCCOMMAND];
            LiteStep::GetToken(args, arg, nullptr, 0);
            state->SetReadingDirection(StateSettings::ParseReadingDirection(arg));
            window->Repaint();
        }
    }),
    BangItem(TEXT("SetTextAlign"),               [] (HWND, LPCTSTR args) -> void {
        DrawableWindow *window = nullptr;
        State *state = FindState(&args, 1, window);
        if (state) {
            char arg[MAX_RCCOMMAND];
            LiteStep::GetToken(args, arg, nullptr, 0);
            state->SetTextAlignment(StateSettings::ParseTextAlignment(arg));
            window->Repaint();
        }
    }),
    BangItem(TEXT("SetTextOffset"),              [] (HWND, LPCTSTR args) -> void {
        DrawableWindow *window = nullptr;
        State *state = FindState(&args, 4, window);
        if (state) {
            char left[MAX_RCCOMMAND], top[MAX_RCCOMMAND], right[MAX_RCCOMMAND], bottom[MAX_RCCOMMAND];
            LPSTR buffers[] = { left, top, right, bottom };
            LiteStep::CommandTokenize(args, buffers, _countof(buffers), nullptr);
            state->SetTextOffsets(float(atof(left)), float(atof(top)), float(atof(right)), float(atof(bottom)));
            window->Repaint();
        }
    }),
    BangItem(TEXT("SetTextRotation"),            [] (HWND, LPCTSTR args) -> void {
        DrawableWindow *window = nullptr;
        State *state = FindState(&args, 1, window);
        if (state) {
            char arg[MAX_RCCOMMAND];
            LiteStep::GetToken(args, arg, nullptr, 0);
            state->SetTextRotation(float(strtof(arg, nullptr)));
            window->Repaint();
        }
    }),
    BangItem(TEXT("SetTextTrimmingGranularity"), [] (HWND, LPCTSTR args) -> void {
        DrawableWindow *window = nullptr;
        State *state = FindState(&args, 1, window);
        if (state) {
            char arg[MAX_RCCOMMAND];
            LiteStep::GetToken(args, arg, nullptr, 0);
            state->SetTextTrimmingGranuality(StateSettings::ParseTrimmingGranularity(arg));
            window->Repaint();
        }
    }),
    BangItem(TEXT("SetTextVerticalAlign"),       [] (HWND, LPCTSTR args) -> void {
        DrawableWindow *window = nullptr;
        State *state = FindState(&args, 1, window);
        if (state) {
            char arg[MAX_RCCOMMAND];
            LiteStep::GetToken(args, arg, nullptr, 0);
            state->SetTextVerticalAlign(StateSettings::ParseParagraphAlignment(arg));
            window->Repaint();
        }
    }),
    BangItem(TEXT("SetWordWrapping"),            [] (HWND, LPCTSTR args) -> void {
        DrawableWindow *window = nullptr;
        State *state = FindState(&args, 1, window);
        if (state) {
            char arg[MAX_RCCOMMAND];
            LiteStep::GetToken(args, arg, nullptr, 0);
            state->SetWordWrapping(StateSettings::ParseWordWrapping(arg));
            window->Repaint();
        }
    })
};


/// <summary>
/// Registers bangs.
/// </summary>
void StateBangs::_Register(LPCTSTR prefix, std::function<DrawableWindow* (LPCSTR)> windowFinder) {
    TCHAR bangName[64];
    ::windowFinder = windowFinder;
    for (BangItem item : BangMap) {
        StringCchPrintf(bangName, _countof(bangName), TEXT("!%s%s"), prefix, item.name);
        LiteStep::AddBangCommand(bangName, item.proc);
    }
}


/// <summary>
/// Unregisters bangs.
/// </summary>
void StateBangs::_UnRegister(LPCTSTR prefix) {
    TCHAR bangName[64];
    for (BangItem item : BangMap) {
        StringCchPrintf(bangName, _countof(bangName), TEXT("!%s%s"), prefix, item.name);
        LiteStep::RemoveBangCommand(bangName);
    }
    ::windowFinder = nullptr;
}
