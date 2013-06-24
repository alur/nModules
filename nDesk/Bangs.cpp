/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Bangs.cpp
 *  The nModules Project
 *
 *  Handles bang commands
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../nShared/LiteStep.h"
#include <strsafe.h>
#include "Bangs.h"
#include "WorkArea.h"
#include "ClickHandler.hpp"
#include "DesktopPainter.hpp"
#include "Settings.h"
#include "../nCoreCom/Core.h"

extern MonitorInfo *g_pMonitorInfo;
extern ClickHandler *g_pClickHandler;
extern DesktopPainter *g_pDesktopPainter;

namespace Bangs {
    struct BangItem {
        BangItem(LPCSTR name, LiteStep::BANGCOMMANDPROC command) {
            this->name = name;
            this->command = command;
        }

        LPCSTR name;
        LiteStep::BANGCOMMANDPROC command;
    };

    BangItem bangMap[] = {
        // Sets the work area.
        BangItem("SetWorkArea", [] (HWND, LPCSTR args) {
            WorkArea::ParseLine(g_pMonitorInfo, args);
            SendNotifyMessage(HWND_BROADCAST, WM_SETTINGCHANGE, SPI_SETWORKAREA, 0);
        }),
        
        // Adds a click handler.
        BangItem("On", [] (HWND, LPCSTR args) {
            g_pClickHandler->AddHandler(args);
        }),

        // Removes click handlers.
        BangItem("Off", [] (HWND, LPCSTR args) {
            g_pClickHandler->RemoveHandlers(args);
        }),

        // Sets the transition duration.
        BangItem("SetTransitionDuration", [] (HWND, LPCSTR args) {
            g_pDesktopPainter->SetTransitionTime(atoi(args));
        }),

        // Sets the transition square size. Force it to be >= 2.
        BangItem("SetTransitionSquareSize", [] (HWND, LPCSTR args) {
            g_pDesktopPainter->SetSquareSize(max(2, atoi(args)));
        }),

        // Sets the transition effect.
        BangItem("SetTransitionEffect", [] (HWND, LPCSTR args) {
            g_pDesktopPainter->SetTransitionType(nDesk::Settings::TransitionTypeFromString(args));
        }),

        // Sets or clears the invalid all on update flag.
        BangItem("SetInvalidateAllOnUpdate", [] (HWND, LPCSTR args) {
            g_pDesktopPainter->SetInvalidateAllOnUpdate(nCore::InputParsing::ParseBool(args));
        })
    };
}


/// <summary>
/// Registers bangs.
/// </summary>
void Bangs::_Register() {
    char bangName[64];
    for (auto &bang : bangMap) {
        StringCchPrintf(bangName, _countof(bangName), "!nDesk%s", bang.name);
        LiteStep::AddBangCommand(bangName, bang.command);
    }
}


/// <summary>
/// Unregisters bangs.
/// </summary>
void Bangs::_Unregister() {
    char bangName[64];
    for (auto &bang : bangMap) {
        StringCchPrintf(bangName, _countof(bangName), "!nDesk%s", bang.name);
        LiteStep::RemoveBangCommand(bangName);
    }
}
