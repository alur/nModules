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

    LPCSTR StyleValueFromName(LPCSTR name) {
        if (_stricmp(name, "Center") == 0) {
            return "0";
        }
        else if (_stricmp(name, "Stretch") == 0) {
            return "2";
        }
        else if (_stricmp(name, "Fit") == 0) {
            return "6";
        }
        else if (_stricmp(name, "Fill") == 0) {
            return "10";
        }
        else if (_stricmp(name, "Span") == 0) {
            return "22";
        }
        return nullptr;
    }

    BangItem bangMap[] = {
        // Skips to the previous track
        BangItem("!SetWallpaper", [] (HWND, LPCSTR args) {
            char token1[MAX_PATH], token2[MAX_PATH];
            LPSTR bufs[] = { token1, token2 };

            LiteStep::CommandTokenize(args, bufs, 1, nullptr);

            // Check if the first token is a valid style.
            LPCSTR styleValue = StyleValueFromName(token1);

            if (styleValue == nullptr) {
                // Assume that the first token is the wallpaper file
            }

            SHSetValueA(HKEY_CURRENT_USER, "Control Panel\\Desktop", "Wallpaper", REG_SZ, token1, (DWORD)strlen(args));
            SendNotifyMessageW(HWND_BROADCAST, WM_SETTINGCHANGE, SPI_SETDESKWALLPAPER, 0);
        }),
        BangItem("!SetWallpaperStyle", [] (HWND, LPCSTR args) {
            char style[MAX_PATH];
            LPSTR bufs[] = { style };

            LiteStep::CommandTokenize(args, bufs, _countof(bufs), nullptr);

            LPCSTR value = StyleValueFromName(style);

            if (value) {
                SHSetValueA(HKEY_CURRENT_USER, "Control Panel\\Desktop", "WallpaperStyle", REG_SZ, value, (DWORD)strlen(value));
                SendNotifyMessageW(HWND_BROADCAST, WM_SETTINGCHANGE, SPI_SETDESKWALLPAPER, 0);
            }
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
