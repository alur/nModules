/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Bangs.cpp
 *  The nModules Project
 *
 *  Handles bang commands
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../nShared/LiteStep.h"
#include "Bangs.h"
#include <strsafe.h>
#include <Shlwapi.h>
#include "../nCoreCom/Core.h"


namespace Bangs {
    struct BangItem {
        BangItem(LPCSTR name, LiteStep::BANGCOMMANDPROC handler) {
            this->name = name;
            this->handler = handler;
        }

        LPCSTR name;
        LiteStep::BANGCOMMANDPROC handler;
    };

    BangItem bangMap[] = {
        // Skips to the previous track
        BangItem("!SetWallpaper", [] (HWND, LPCSTR args) {
            char file[MAX_PATH];
            LPSTR bufs[] = { file };

            LiteStep::CommandTokenize(args, bufs, 1, nullptr);

            SHSetValueA(HKEY_CURRENT_USER, "Control Panel\\Desktop", "Wallpaper", REG_SZ, file, (DWORD)strlen(args));
            SendNotifyMessageW(HWND_BROADCAST, WM_SETTINGCHANGE, SPI_SETDESKWALLPAPER, 0);
        })
    };
    

    /// <summary>
    /// Registers bangs.
    /// </summary>
    void _Register() {
        for (BangItem &bangItem : bangMap) {
            LiteStep::AddBangCommand(bangItem.name, bangItem.handler);
        }
    }


    /// <summary>
    /// Unregisters bangs.
    /// </summary>
    void _Unregister() {
        for (BangItem &bangItem : bangMap) {
            LiteStep::RemoveBangCommand(bangItem.name);
        }
    }
}
