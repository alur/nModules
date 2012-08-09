/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Core.h                                                          July, 2012
 *  The nModules Project
 *
 *  Functions declarations for the CoreCom. All functions exported by nCore
 *  and some Init functions.
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#ifndef CORE_H
#define CORE_H

#include "../nShared/Versioning.h"

typedef DWORD ARGB;

#define INIT_FUNC(var,type,name) var = (type)GetProcAddress(hCoreInstance,name); if (var == NULL) return E_NOTIMPL;

namespace nCore {
    HRESULT Init(VERSION minVersion);
    bool Initalized();

    VERSION GetVersion();

    namespace InputParsing {
        HRESULT Init(HMODULE);

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
        HRESULT Init(HMODULE);

        BOOL FormatText(LPCWSTR pszSource, size_t cchDest, LPWSTR pszDest);
    }
}

#endif /* CORE_H */
