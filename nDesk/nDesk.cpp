/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *	nDesk.cpp														July, 2012
 *	The nModules Project
 *
 *	Main .cpp file for the nDesk module.
 *      
 *													             Erik Welander
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "nDesk.h"
#include "../nCoreCom/Core.h"
#include "../nShared/MonitorInfo.hpp"
#include "DesktopPainter.hpp"
#include "ClickHandler.hpp"
#include "WorkArea.h"
#include "Bangs.h"
#include "../nShared/Error.h"
#include "../nShared/Factories.h"
#include "Settings.h"

// Constants
const int  g_minCoreVersion = 0x01000000;
LPCSTR g_rcsRevision		= "1.0";
LPCSTR g_szAppName			= "nDesk";
LPCSTR g_szMainHandler		= "DesktopBackgroundClass";
LPCSTR g_szAuthor			= "Alurcard2";

// The messages we want from the core
UINT g_lsMessages[] = { LM_GETREVID, LM_REFRESH, 0 };
HWND g_hwndMain;

// Class pointers
DesktopPainter* g_pDesktopPainter;
MonitorInfo* g_pMonitorInfo;
ClickHandler* g_pClickHandler;

/// <summary>
/// The main entry point for this DLL.
/// </summary>
BOOL APIENTRY DllMain( HANDLE hModule, DWORD ul_reason_for_call, LPVOID /* lpReserved */) {
	if (ul_reason_for_call == DLL_PROCESS_ATTACH)
		DisableThreadLibraryCalls((HINSTANCE)hModule);
	return TRUE;
}

/// <summary>
/// Called by the LiteStep core when this module is loaded.
/// </summary>
int initModuleEx(HWND /* hWndParent */, HINSTANCE hDllInstance, LPCSTR /* szPath */) {
	// Initalize communication with the core
	switch (nCore::Init(g_minCoreVersion)) {
	case S_OK:
		break;
	default:
		ErrorMessage(E_LVL_ERROR, "There was a problem connecting to nCore!");
		return 1;
	}

	// Initialize
	g_pMonitorInfo = new MonitorInfo();
	g_pClickHandler = new ClickHandler();
	g_pDesktopPainter = NULL; // Initalized on WM_CREATE

	// Create the main window
	if (!CreateMainWindow(hDllInstance))
		return 1;

	// Load bang commands
	Bangs::_Register();

	// Load settings
	Settings::Load();

	// Reset the work area for all monitors
	WorkArea::ResetWorkAreas(g_pMonitorInfo);
	WorkArea::LoadSettings(g_pMonitorInfo);

	return 0;
}

/// <summary>
/// Called by the LiteStep core when this module is about to be unloaded.
/// </summary>
void quitModule(HINSTANCE hDllInstance) {
	// Reset the work area for all monitors
	WorkArea::ResetWorkAreas(g_pMonitorInfo);

	// Unregister bangs
	Bangs::_Unregister();

	// Destroy the main window
	if (g_hwndMain) {
		SendMessage(GetLitestepWnd(), LM_UNREGISTERMESSAGE, (WPARAM)g_hwndMain, (LPARAM)g_lsMessages);
		DestroyWindow(g_hwndMain);
	}

	// Delete global classes
	if (g_pDesktopPainter) delete g_pDesktopPainter;
	if (g_pClickHandler) delete g_pClickHandler;
	if (g_pMonitorInfo) delete g_pMonitorInfo;

	// Unregister the desktopbackgrounclass
	UnregisterClass(g_szMainHandler, hDllInstance);

	// Let go of any factories we've allocated
	Factories::Release();
}

/// <summary>
/// Creates the main message handler.
/// </summary>
/// <param name="hInst">The instance to attach this message handler to.</param>
bool CreateMainWindow(HINSTANCE hInst) {
	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof(WNDCLASSEX));
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.lpfnWndProc = MainProc;
	wc.hInstance = hInst;
	wc.lpszClassName = g_szMainHandler;
	wc.hIconSm = 0;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.style = CS_DBLCLKS;

	if (!RegisterClassEx(&wc)) {
		ErrorMessage(E_LVL_ERROR, TEXT("Error registering the desktop background class, is there already a desktop module loaded?"));
		return false;
	}

	g_hwndMain = CreateWindowEx(WS_EX_TOOLWINDOW, g_szMainHandler, "", WS_POPUP|WS_CLIPSIBLINGS|WS_CLIPCHILDREN,
		g_pMonitorInfo->m_virtualDesktop.rect.left, g_pMonitorInfo->m_virtualDesktop.rect.top, g_pMonitorInfo->m_virtualDesktop.width, g_pMonitorInfo->m_virtualDesktop.height,
		NULL, NULL, hInst, NULL);

	if (!g_hwndMain) {
		ErrorMessage(E_LVL_ERROR, TEXT("Failed to create the desktop background window!"));
		UnregisterClass(g_szMainHandler, hInst);
		return false;
	}

	SendMessage(GetLitestepWnd(), LM_REGISTERMESSAGE, (WPARAM)g_hwndMain, (LPARAM) g_lsMessages);

	SetWindowLongPtr(g_hwndMain, GWLP_USERDATA, MAGIC_DWORD);
	SetWindowPos(g_hwndMain, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE|SWP_NOSENDCHANGING);
	ShowWindow(g_hwndMain, SW_SHOWNOACTIVATE);

	return true;
}

/// <summary>
/// Handles the main window's messages.
/// </summary>
/// <param name="hWnd">The window the message is for.</param>
/// <param name="uMsg">The type of message.</param>
/// <param name="wParam">wParam</param>
/// <param name="lParam">lParam</param>
LRESULT WINAPI MainProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch(uMsg) {
	case WM_CREATE:
		g_pDesktopPainter = new DesktopPainter(hWnd);
		return 0;

	case LM_GETREVID: {
		size_t uLength;
		StringCchPrintf((char*)lParam, 64, "%s: %s", g_szAppName, g_rcsRevision);
			
		if (SUCCEEDED(StringCchLength((char*)lParam, 64, &uLength)))
			return uLength;

		lParam = NULL;
		return 0;
	}

	case LM_REFRESH:
		g_pClickHandler->Refresh();
		WorkArea::LoadSettings(g_pMonitorInfo, true);
		Settings::Load();
		return 0;
	
	case WM_MOUSEWHEEL:
	case WM_MOUSEHWHEEL:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_LBUTTONDBLCLK:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MBUTTONDBLCLK:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_RBUTTONDBLCLK:
	case WM_XBUTTONDOWN:
	case WM_XBUTTONUP:
	case WM_XBUTTONDBLCLK:
		g_pClickHandler->HandleClick(uMsg, wParam, lParam);
		return 0;
	
	case WM_PAINT:
	case WM_ERASEBKGND:
		return g_pDesktopPainter->HandleMessage(hWnd, uMsg, wParam, lParam);
	
	case WM_WINDOWPOSCHANGING: {
		// Keep the hWnd at the bottom of the window stack
		WINDOWPOS *c = (WINDOWPOS*)lParam;
		c->hwnd = g_hwndMain;
		c->hwndInsertAfter = HWND_BOTTOM;
		c->flags |= SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOMOVE;
		return 0;
	}

	case WM_DISPLAYCHANGE:
		g_pMonitorInfo->Update();
		g_pDesktopPainter->Resize();
		break;
	
	case WM_SETTINGCHANGE:
		if (wParam == SPI_SETDESKWALLPAPER) {
			g_pDesktopPainter->UpdateWallpaper();
			return 0;
		}
		break;

	case WM_TIMER:
		switch (wParam) {
		case 1337:
			g_pDesktopPainter->TransitionStep();
		}
		break;
	
	case WM_CLOSE:
		// If someone tries to exit the desktop window, lets make it a windows shutdown.
		PostMessage(GetLitestepWnd(), LM_RECYCLE, 3, 0);
		return 0;

	case WM_SYSCOMMAND:
		switch (wParam) {
			// For using the standard alt+F4 to shutdown windows
			case SC_CLOSE:
				PostMessage(GetLitestepWnd(), LM_RECYCLE, 3, 0);
				return 0;
			default:
				break;
		}
		break;

	case WM_CHILDACTIVATE:
	case WM_NCACTIVATE:
	case WM_SETFOCUS:
	case WM_KILLFOCUS:
	case WM_ACTIVATEAPP:
	case WM_ACTIVATE:
	case WM_PARENTNOTIFY:
		SetWindowPos(g_hwndMain,HWND_BOTTOM,0,0,0,0,SWP_NOACTIVATE);
		break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
