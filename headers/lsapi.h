#ifndef LSSDK_LSAPI_H
#define LSSDK_LSAPI_H

#include <windows.h>

// General Constants
#define MAX_LINE_LENGTH 4096
#define MAGIC_DWORD     0x49474541

// LM_RELOADMODULE/LM_UNLOADMODULE
#define LMM_PATH      0x0000
#define LMM_HINSTANCE 0x1000

// LM_SYSTRAYINFOEVENT
#define TRAYEVENT_GETICONPOS  1
#define TRAYEVENT_GETICONSIZE 2

// EnumLSData
#define ELD_BANGS       1
#define ELD_MODULES     2
#define ELD_REVIDS      3
#define ELD_BANGS_V2    4
#define ELD_PERFORMANCE 5

// EnumModulesProc
#define LS_MODULE_THREADED 0x0001

// LSLog, LSLogPrintf
#define LOG_ERROR   1
#define LOG_WARNING 2
#define LOG_NOTICE  3
#define LOG_DEBUG   4

// is_valid_pattern
#define PATTERN_VALID  0
#define PATTERN_ESC   -1
#define PATTERN_RANGE -2
#define PATTERN_CLOSE -3
#define PATTERN_EMPTY -4

// matche
#define MATCH_VALID   1
#define MATCH_END     2
#define MATCH_ABORT   3
#define MATCH_RANGE   4
#define MATCH_LITERAL 5
#define MATCH_PATTERN 6

// LM_SYSTRAY
#define TRAY_MAX_TIP_LENGTH       128
#define TRAY_MAX_INFO_LENGTH      256
#define TRAY_MAX_INFOTITLE_LENGTH 64

typedef struct LSNOTIFYICONDATA {
    DWORD cbSize;
    HWND hWnd;
    UINT uID;
    UINT uFlags;
    UINT uCallbackMessage;
    HICON hIcon;
    CHAR szTip[TRAY_MAX_TIP_LENGTH];
    DWORD dwState;
    DWORD dwStateMask;
    CHAR szInfo[TRAY_MAX_INFO_LENGTH];
    union {
        UINT uTimeout;
        UINT uVersion;
    } DUMMYUNIONNAME;
    CHAR szInfoTitle[TRAY_MAX_INFOTITLE_LENGTH];
    DWORD dwInfoFlags;
    GUID guidItem;
    HICON hBalloonIcon;
} LSNOTIFYICONDATA, *LPLSNOTIFYICONDATA;

// LM_SYSTRAYINFOEVENT
#define TRAYEVENT_GETICONPOS  1
#define TRAYEVENT_GETICONSIZE 2

typedef struct _SYSTRAYINFOEVENT {
    DWORD cbSize;
    DWORD dwEvent;
    HWND hWnd;
    UINT uID;
    GUID guidItem;
} SYSTRAYINFOEVENT, *LPSYSTRAYINFOEVENT;

// Messages
#define LM_SAVEDATA              8892  // Module -> Core
#define LM_RESTOREDATA           8893  // Module -> Core
#define LM_SYSTRAY               9214  // Core   -> Module
#define LM_SYSTRAYREADY          9215  // Module -> Core
#define LM_SYSTRAYINFOEVENT      9216  // Core   -> Module
#define LM_REGISTERMESSAGE       9263  // Module -> Core
#define LM_UNREGISTERMESSAGE     9264  // Module -> Core
#define LM_GETREVID              9265  // Core   -> Module
#define LM_UNLOADMODULE          9266  // Module -> Core
#define LM_RELOADMODULE          9267  // Module -> Core
#define LM_REGISTERHOOKMESSAGE   9268  // Module -> Core
#define LM_UNREGISTERHOOKMESSAGE 9269  // Module -> Core
#define LM_REFRESH               9305  // Core   -> Module
#define LM_WINDOWCREATED         9501  // Core   -> Module
#define LM_WINDOWDESTROYED       9502  // Core   -> Module
#define LM_ACTIVATESHELLWINDOW   9503  // Core   -> Module
#define LM_WINDOWACTIVATED       9504  // Core   -> Module
#define LM_GETMINRECT            9505  // Core   -> Module
#define LM_REDRAW                9506  // Core   -> Module
#define LM_TASKMAN               9507  // Core   -> Module
#define LM_LANGUAGE              9508  // Core   -> Module
#define LM_ACCESSIBILITYSTATE    9511  // Core   -> Module
#define LM_APPCOMMAND            9512  // Core   -> Module
#define LM_WINDOWREPLACED        9513  // Core   -> Module
#define LM_WINDOWREPLACING       9514  // Core   -> Module
#define LM_MONITORCHANGED        9516  // Core   -> Module

// Callback Function Pointers
typedef VOID (__cdecl * BANGCOMMANDPROC)(HWND hwndOwner, LPCSTR pszArgs);
typedef VOID (__cdecl * BANGCOMMANDPROCEX)(HWND hwndOwner, LPCSTR pszBangCommandName, LPCSTR pszArgs);
typedef BOOL (__stdcall * ENUMBANGSPROC)(LPCSTR pszBangCommandName, LPARAM lParam);
typedef BOOL (__stdcall * ENUMMODULESPROC)(LPCSTR pszPath, DWORD fdwFlags, LPARAM lParam);
typedef BOOL (__stdcall * ENUMREVIDSPROC)(LPCSTR pszRevID, LPARAM lParam);
typedef BOOL (__stdcall * ENUMBANGSV2PROC)(HINSTANCE hinstModule, LPCSTR pszBangCommandName, LPARAM lParam);

// Functions
#define EXTERN_CDECL(type) EXTERN_C DECLSPEC_IMPORT type __cdecl
#define EXTERN_STDCALL(type) EXTERN_C DECLSPEC_IMPORT type __stdcall

EXTERN_CDECL(BOOL) AddBangCommand(LPCSTR pszBangCommandName, BANGCOMMANDPROC pfnCallback);
EXTERN_CDECL(BOOL) AddBangCommandEx(LPCSTR pszBangCommandName, BANGCOMMANDPROCEX pfnCallback);
EXTERN_CDECL(HBITMAP) BitmapFromIcon(HICON hIcon);
EXTERN_CDECL(HRGN) BitmapToRegion(HBITMAP hbmBitmap, COLORREF crTransparent, COLORREF crTolerance, INT xOffset, INT yOffset);
EXTERN_CDECL(VOID) CommandParse(LPCSTR pszString, LPSTR pszCommandToken, LPSTR pszCommandArgs, UINT cbCommandToken, UINT cbCommandArgs);
EXTERN_CDECL(INT) CommandTokenize(LPCSTR pszString, LPSTR * ppszBuffers, UINT cBuffers, LPSTR pszExtraBuffer);
EXTERN_CDECL(HRESULT) EnumLSData(UINT uType, FARPROC pfnCallback, LPARAM lParam);
EXTERN_CDECL(VOID) Frame3D(HDC hdcDst, RECT rcFrame, COLORREF crTop, COLORREF crBottom, INT nWidth);
EXTERN_CDECL(HWND) GetLitestepWnd();
EXTERN_CDECL(VOID) GetLSBitmapSize(HBITMAP hbmBitmap, INT *pnWidth, INT *pnHeight);
EXTERN_CDECL(BOOL) GetRCBool(LPCSTR pszKeyName, BOOL fValue);
EXTERN_CDECL(BOOL) GetRCBoolDef(LPCSTR pszKeyName, BOOL fDefault);
EXTERN_CDECL(COLORREF) GetRCColor(LPCSTR pszKeyName, COLORREF crDefault);
EXTERN_CDECL(INT) GetRCCoordinate(LPCSTR pszKeyName, INT nDefault, INT nLimit);
EXTERN_CDECL(INT) GetRCInt(LPCSTR pszKeyName, INT nDefault);
EXTERN_CDECL(BOOL) GetRCLine(LPCSTR pszKeyName, LPSTR pszBuffer, UINT cbBuffer, LPCSTR pszDefault);
EXTERN_CDECL(BOOL) GetRCString(LPCSTR pszKeyName, LPSTR pszBuffer, LPCSTR pszDefault, UINT cbBuffer);
EXTERN_CDECL(VOID) GetResStr(HINSTANCE hInstance, UINT uID, LPSTR pszBuffer, UINT cbBuffer, LPCSTR pszDefault);
EXTERN_CDECL(VOID) GetResStrEx(HINSTANCE hInstance, UINT uID, LPSTR pszBuffer, UINT cbBuffer, LPCSTR pszDefault, ...);
EXTERN_CDECL(BOOL) GetToken(LPCSTR pszString, LPSTR pszBuffer, LPCSTR *ppszNext, BOOL fBrackets);
EXTERN_CDECL(BOOL) is_valid_pattern(LPCSTR pszPattern, INT *pnError);
EXTERN_CDECL(BOOL) LCClose(LPVOID pFile);
EXTERN_CDECL(LPVOID) LCOpen(LPCSTR pszPath);
EXTERN_CDECL(BOOL) LCReadNextCommand(LPVOID pFile, LPSTR pszBuffer, UINT cbBuffer);
EXTERN_CDECL(BOOL) LCReadNextConfig(LPVOID pFile, LPCSTR pszKeyName, LPSTR pszBuffer, UINT cbBuffer);
EXTERN_CDECL(BOOL) LCReadNextLine(LPVOID pFile, LPSTR pszBuffer, UINT cbBuffer);
EXTERN_CDECL(INT) LCTokenize(LPCSTR pszString, LPSTR *ppszBuffers, UINT cBuffers, LPSTR pszExtraBuffer);
EXTERN_CDECL(HICON) LoadLSIcon(LPCSTR pszPath, LPVOID pReserved);
EXTERN_CDECL(HBITMAP) LoadLSImage(LPCSTR pszPath, LPVOID pReserved);
EXTERN_CDECL(HINSTANCE) LSExecute(HWND hwndOwner, LPCSTR pszCommandLine, INT nShowCmd);
EXTERN_CDECL(HINSTANCE) LSExecuteEx(HWND hwndOwner, LPCSTR pszOperation, LPCSTR pszCommand, LPCSTR pszArgs, LPCSTR pszDirectory, INT nShowCmd);
EXTERN_STDCALL(BOOL) LSGetImagePath(LPSTR pszBuffer, UINT cbBuffer);
EXTERN_STDCALL(BOOL) LSGetLitestepPath(LPSTR pszBuffer, UINT cbBuffer);
EXTERN_CDECL(BOOL) LSEnumDisplayMonitors(HDC, LPCRECT, MONITORENUMPROC, LPARAM); // See Win32 EnumDisplayMonitors
EXTERN_CDECL(BOOL) LSEnumDisplayDevices(LPCSTR, DWORD, PDISPLAY_DEVICEA, DWORD); // See Win32 EnumDisplayDevices
EXTERN_CDECL(BOOL) LSGetMonitorInfo(HMONITOR, LPMONITORINFO);                    // See Win32 GetMonitorInfo
EXTERN_CDECL(INT) LSGetSystemMetrics(INT);                                       // See Win32 GetSystemMetrics
EXTERN_CDECL(BOOL) LSGetVariable(LPCSTR pszKeyName, LPSTR pszBuffer);
EXTERN_CDECL(BOOL) LSGetVariableEx(LPCSTR pszKeyName, LPSTR pszBuffer, UINT cbBuffer);
EXTERN_STDCALL(BOOL) LSLog(INT nLevel, LPCSTR pszModule, LPCSTR pszMessage);
EXTERN_CDECL(BOOL) LSLogPrintf(INT nLevel, LPCSTR pszModule, LPCSTR pszFormat, ...);
EXTERN_CDECL(HMONITOR) LSMonitorFromPoint(POINT, DWORD);                         // See Win32 MonitorFromPoint
EXTERN_CDECL(HMONITOR) LSMonitorFromRect(LPCRECT, DWORD);                        // See Win32 MonitorFromRect
EXTERN_CDECL(HMONITOR) LSMonitorFromWindow(HWND, DWORD);                         // See Win32 MonitorFromWindow
EXTERN_CDECL(BOOL) LSSetVariable(LPCSTR pszKeyName, LPCSTR pszValue);
EXTERN_CDECL(BOOL) match(LPCSTR pszPattern, LPCSTR pszText);
EXTERN_CDECL(INT) matche(LPCSTR pszPattern, LPCSTR pszText);
EXTERN_CDECL(BOOL) ParseBangCommand(HWND hwndOwner, LPCSTR pszBangCommandName, LPCSTR pszArgs);
EXTERN_CDECL(INT) ParseCoordinate(LPCSTR pszString, INT nDefault, INT nLimit);
EXTERN_CDECL(BOOL) RemoveBangCommand(LPCSTR pszBangCommandName);
EXTERN_CDECL(VOID) SetDesktopArea(INT nLeft, INT nTop, INT nRight, INT nBottom);
EXTERN_CDECL(VOID) TransparentBltLS(HDC hdcDest, INT nXDest, INT nYDest, INT nWidth, INT nHeight, HDC hdcSrc, INT nXSrc, INT nYSrc, COLORREF crTransparent);
EXTERN_CDECL(VOID) VarExpansion(LPSTR pszBuffer, LPCSTR pszString);
EXTERN_CDECL(VOID) VarExpansionEx(LPSTR pszBuffer, LPCSTR pszString, UINT cbBuffer);

#endif // LSSDK_LSAPI_H
