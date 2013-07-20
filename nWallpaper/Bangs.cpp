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

            LiteStep::CommandTokenize(args, bufs, _countof(bufs), nullptr);

            SHSetValueA(HKEY_CURRENT_USER, "Control Panel\\Desktop", "Wallpaper", REG_SZ, file, (DWORD)strlen(args));
            SendNotifyMessageW(HWND_BROADCAST, WM_SETTINGCHANGE, SPI_SETDESKWALLPAPER, 0);
        }),
        BangItem("!SetWallpaperStyle", [] (HWND, LPCSTR args) {
            char style[MAX_PATH];
            LPSTR bufs[] = { style };

            LiteStep::CommandTokenize(args, bufs, _countof(bufs), nullptr);

            LPCSTR value = nullptr;

            if (_stricmp(style, "Center") == 0) {
                value = "0";
            }
            else if (_stricmp(style, "Stretch") == 0) {
                value = "2";
            }
            else if (_stricmp(style, "Fit") == 0) {
                value = "6";
            }
            else if (_stricmp(style, "Fill") == 0) {
                value = "10";
            }
            else if (_stricmp(style, "Span") == 0) {
                value = "22";
            }
            else {
                return;
            }

            SHSetValueA(HKEY_CURRENT_USER, "Control Panel\\Desktop", "WallpaperStyle", REG_SZ, value, (DWORD)strlen(value));
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
