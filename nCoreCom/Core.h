/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Core.h
 *  The nModules Project
 *
 *  Functions declarations for the CoreCom library. All functions exported by
 *  nCore and some Init functions.
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "../nShared/MonitorInfo.hpp"
#include "../nShared/Versioning.h"
#include "../nCore/IParsedText.hpp"

typedef DWORD ARGB;

#define INIT_FUNC(var,type,name) var = (type)GetProcAddress(hCoreInstance,name); if (var == NULL) return E_NOTIMPL;

namespace nCore {
    HRESULT Connect(VERSION minVersion);
    void Disconnect();
    bool Initalized();

    VERSION GetVersion();

    namespace InputParsing {
        HRESULT _Init(HMODULE);

        bool ParseColor(LPCSTR pszColor, ARGB * target);
        bool ParseCoordinate(LPCSTR, int *, bool canBeRelative = true, bool canBeNegative = true);
        bool ParseLength(LPCSTR, int *, bool canBeRelative = true, bool canBeNegative = false);
        bool ParseMonitor(LPCSTR pszMonitor, UINT * target);
        bool ParseBool(LPCSTR pszBool);

        int GetPrefixedRCInt(LPCSTR szPrefix, LPCSTR szOption, int nDefault);
        float GetPrefixedRCFloat(LPCSTR szPrefix, LPCSTR szOption, float fDefault);
        double GetPrefixedRCDouble(LPCSTR szPrefix, LPCSTR szOption, double dDefault);
        ARGB GetPrefixedRCColor(LPCSTR szPrefix, LPCSTR szOption, int nDefault);
        bool GetPrefixedRCBool(LPCSTR szPrefix, LPCSTR szOption, bool bDefault);
        bool GetPrefixedRCString(LPCSTR szPrefix, LPCSTR szOption, LPSTR pszBuffer, LPCSTR pszDefault, UINT cbBuffer);
        bool GetPrefixedRCWString(LPCSTR szPrefix, LPCSTR szOption, LPWSTR pszwBuffer, LPCSTR pszDefault, UINT cbBuffer);
        UINT GetPrefixedRCMonitor(LPCSTR szPrefix, LPCSTR szOption, UINT uDefault);
    }
    
    namespace System {
        HRESULT _Init(HMODULE);

        //
        MonitorInfo *FetchMonitorInfo();

        //
        IParsedText *ParseText(LPCWSTR text);
        BOOL RegisterDynamicTextFunction(LPCWSTR name, UCHAR numArgs, FORMATTINGPROC formatter, bool dynamic);
        BOOL UnRegisterDynamicTextFunction(LPCWSTR name, UCHAR numArgs);
        BOOL DynamicTextChangeNotification(LPCWSTR name, UCHAR numArgs);

        //
        void RegisterWindow(LPCSTR prefix, LPVOID window);
        void UnRegisterWindow(LPCSTR prefix);
        LPVOID FindRegisteredWindow(LPCSTR prefix);
    }
}
