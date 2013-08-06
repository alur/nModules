/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Bangs.cpp
 *  The nModules Project
 *
 *  Handles bang commands
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../nShared/LiteStep.h"
#include "Bangs.h"
#include <Shlwapi.h>
#include "../nCoreCom/Core.h"


namespace Bangs
{
    struct BangItem
    {
        BangItem(LPCTSTR name, LiteStep::BANGCOMMANDPROC handler)
        {
            this->name = name;
            this->handler = handler;
        }

        LPCTSTR name;
        LiteStep::BANGCOMMANDPROC handler;
    };

    LPCTSTR StyleValueFromName(LPCTSTR name)
    {
        if (_tcsicmp(name, _T("Center")) == 0)
        {
            return _T("0");
        }
        else if (_tcsicmp(name, _T("Stretch")) == 0)
        {
            return _T("2");
        }
        else if (_tcsicmp(name, _T("Fit")) == 0)
        {
            return _T("6");
        }
        else if (_tcsicmp(name, _T("Fill")) == 0)
        {
            return _T("10");
        }
        else if (_tcsicmp(name, _T("Span")) == 0)
        {
            return _T("22");
        }
        return nullptr;
    }

    BangItem bangMap[] = {
        // Skips to the previous track
        BangItem(_T("!SetWallpaper"), [] (HWND, LPCTSTR args)
        {
            TCHAR token1[MAX_PATH], token2[MAX_PATH];
            LPTSTR bufs[] = { token1, token2 };

            LiteStep::CommandTokenize(args, bufs, 1, nullptr);

            // Check if the first token is a valid style.
            LPCTSTR styleValue = StyleValueFromName(token1);

            if (styleValue == nullptr)
            {
                // Assume that the first token is the wallpaper file
            }

            SHSetValue(HKEY_CURRENT_USER, _T("Control Panel\\Desktop"), _T("Wallpaper"), REG_SZ, token1, (DWORD)_tcslen(args));
            SendNotifyMessage(HWND_BROADCAST, WM_SETTINGCHANGE, SPI_SETDESKWALLPAPER, 0);
        }),
        BangItem(_T("!SetWallpaperStyle"), [] (HWND, LPCTSTR args)
        {
            TCHAR style[MAX_PATH];
            LPTSTR bufs[] = { style };

            LiteStep::CommandTokenize(args, bufs, _countof(bufs), nullptr);

            LPCTSTR value = StyleValueFromName(style);

            if (value)
            {
                SHSetValue(HKEY_CURRENT_USER, _T("Control Panel\\Desktop"), _T("WallpaperStyle"), REG_SZ, value, (DWORD)_tcslen(value));
                SendNotifyMessage(HWND_BROADCAST, WM_SETTINGCHANGE, SPI_SETDESKWALLPAPER, 0);
            }
        })
    };
    

    /// <summary>
    /// Registers bangs.
    /// </summary>
    void _Register()
    {
        for (BangItem &bangItem : bangMap)
        {
            LiteStep::AddBangCommand(bangItem.name, bangItem.handler);
        }
    }


    /// <summary>
    /// Unregisters bangs.
    /// </summary>
    void _Unregister()
    {
        for (BangItem &bangItem : bangMap)
        {
            LiteStep::RemoveBangCommand(bangItem.name);
        }
    }
}
