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
#include <algorithm>

extern ClickHandler *g_pClickHandler;
extern DesktopPainter *g_pDesktopPainter;

namespace Bangs {
    struct BangItem {
        BangItem(LPCTSTR name, LiteStep::BANGCOMMANDPROC command) {
            this->name = name;
            this->command = command;
        }

        LPCTSTR name;
        LiteStep::BANGCOMMANDPROC command;
    };

    BangItem bangMap[] = {
        // Sets the work area.
        BangItem(_T("SetWorkArea"), [] (HWND, LPCTSTR args) {
            WorkArea::ParseLine(&nCore::FetchMonitorInfo(), args);
            SendNotifyMessage(HWND_BROADCAST, WM_SETTINGCHANGE, SPI_SETWORKAREA, 0);
        }),
        
        // Adds a click handler.
        BangItem(_T("On"), [] (HWND, LPCTSTR args) {
            g_pClickHandler->AddHandler(args);
        }),

        // Removes click handlers.
        BangItem(_T("Off"), [] (HWND, LPCTSTR args) {
            g_pClickHandler->RemoveHandlers(args);
        }),

        // Sets the transition duration.
        BangItem(_T("SetTransitionDuration"), [] (HWND, LPCTSTR args) {
            g_pDesktopPainter->SetTransitionTime(_ttoi(args));
        }),

        // Sets the transition square size. Force it to be >= 2.
        BangItem(_T("SetTransitionSquareSize"), [] (HWND, LPCTSTR args) {
            g_pDesktopPainter->SetSquareSize(std::max(2, _ttoi(args)));
        }),

        // Sets the transition effect.
        BangItem(_T("SetTransitionEffect"), [] (HWND, LPCTSTR args) {
            g_pDesktopPainter->SetTransitionType(nDesk::Settings::TransitionTypeFromString(args));
        }),

        // Sets or clears the invalid all on update flag.
        BangItem(_T("SetInvalidateAllOnUpdate"), [] (HWND, LPCTSTR args) {
            g_pDesktopPainter->SetInvalidateAllOnUpdate(LiteStep::ParseBool(args));
        })
    };
}


/// <summary>
/// Registers bangs.
/// </summary>
void Bangs::_Register() {
    TCHAR bangName[64];
    for (auto &bang : bangMap)
    {
        StringCchPrintf(bangName, _countof(bangName), _T("!nDesk%s"), bang.name);
        LiteStep::AddBangCommand(bangName, bang.command);
    }
}


/// <summary>
/// Unregisters bangs.
/// </summary>
void Bangs::_Unregister() {
    TCHAR bangName[64];
    for (auto &bang : bangMap)
    {
        StringCchPrintf(bangName, _countof(bangName), _T("!nDesk%s"), bang.name);
        LiteStep::RemoveBangCommand(bangName);
    }
}
