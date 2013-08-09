/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  TrayIcon.cpp
 *  The nModules Project
 *
 *  Implementation of the TrayIcon class. A single icon in the tray.
 *   
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../nShared/LiteStep.h"
#include <strsafe.h>
#include "../nShared/LSModule.hpp"
#include "Tray.hpp"
#include "TrayIcon.hpp"
#include "Windowsx.h"
#include <shellapi.h>


extern LSModule gLSModule;
extern HWND g_hWndTrayNotify;


/// <summary>
/// Constructor
/// </summary>
TrayIcon::TrayIcon(Drawable* parent, LiteStep::LPLSNOTIFYICONDATA pNID, Settings* parentSettings) : Drawable(parent, _T("Icon"))
{
    // Init
    this->callbackID = 0;
    this->callbackMessage = 0xFFFFFFFF;
    this->callbackWindow = nullptr;
    this->icon = nullptr;
    this->iconSize = 0;
    this->showingTip =  false;
    this->showTip = false;
    this->version = 0;
    this->guid = GUID_NULL;
    this->flags = 0;
    this->tip[0] = L'\0';

    // Create the drawable window
    mSettings = parentSettings->CreateChild(_T("Icon"));

    StateSettings defaultStateSettings;
    defaultStateSettings.backgroundBrush.color = Color::Create(0x00000000);
    mWindow->Initialize(nullptr, &defaultStateSettings);
    this->showingTip = false;

    //
    LoadSettings();

    //
    HandleAdd(pNID);
}


/// <summary>
/// Destructor
/// </summary>
TrayIcon::~TrayIcon() {
}


/// <summary>
/// Loads RC settings.
/// </summary>
void TrayIcon::LoadSettings(bool /* bIsRefresh */) {
    this->iconSize = mSettings->GetInt(_T("Size"), 16);
}


/// <summary>
/// Shows the trayicon.
/// </summary>
void TrayIcon::Show() {
    mWindow->Show();
}


/// <summary>
/// Handles NIM_ADD.
/// </summary>
void TrayIcon::HandleAdd(LiteStep::LPLSNOTIFYICONDATA pNID) {
    this->callbackWindow = pNID->hWnd;
    this->callbackID = pNID->uID;
    GetWindowThreadProcessId(pNID->hWnd, &mProcessID);
    HandleModify(pNID);
}


/// <summary>
/// Handles NIM_MODIFY.
/// </summary>
void TrayIcon::HandleModify(LiteStep::LPLSNOTIFYICONDATA pNID) {
    if ((pNID->uFlags & NIF_MESSAGE) == NIF_MESSAGE) {
        this->callbackMessage = pNID->uCallbackMessage;
    }
    if ((pNID->uFlags & NIF_ICON) == NIF_ICON) {
        SetIcon(pNID->hIcon);
    }
    if ((pNID->uFlags & NIF_TIP) == NIF_TIP) {
        StringCchCopy(this->tip, TRAY_MAX_TIP_LENGTH, pNID->szTip);
        if (this->showingTip) {
            if (this->tip[0] != L'\0') {
                RECT r;
                mWindow->GetScreenRect(&r);
                ((Tray*)mParent)->ShowTip(this->tip, &r);
            }
            else {
                ((Tray*)mParent)->HideTip();
            }
        }
    }

    if ((this->flags & NIF_GUID) != NIF_GUID && (pNID->uFlags & NIF_GUID) == NIF_GUID) {
        this->flags &= NIF_GUID;
        this->guid = pNID->guidItem;
    }

    // TODO::NIF_STATE

    if ((pNID->uFlags & NIF_INFO) == NIF_INFO) {
        WCHAR info[TRAY_MAX_INFO_LENGTH], infoTitle[TRAY_MAX_INFOTITLE_LENGTH];

        // uTimeout is only valid on 2000 and XP, so we can safely ignore it.
        StringCchCopy(info, TRAY_MAX_INFO_LENGTH, pNID->szInfo);
        StringCchCopy(infoTitle, TRAY_MAX_INFOTITLE_LENGTH, pNID->szInfoTitle);

        ((Tray*)mParent)->EnqueueBalloon(this, infoTitle, info, pNID->dwInfoFlags, pNID->hBalloonIcon, (pNID->uFlags & NIF_REALTIME) == NIF_REALTIME);
    }

    this->showTip = true;
}


/// <summary>
/// Handles NIM_SETVERSION.
/// </summary>
void TrayIcon::HandleSetVersion(LiteStep::LPLSNOTIFYICONDATA pNID) {
    this->version = pNID->uVersion;
}


/// <summary>
/// Updates the icon.
/// </summary>
void TrayIcon::SetIcon(HICON icon) {
    if (this->icon != icon) {
        this->icon = icon;
        mWindow->ClearOverlays();
        D2D1_RECT_F f;
        f.bottom = (float)this->iconSize; f.top = 0; f.left = 0; f.right = (float)this->iconSize;
        this->iconOverlay = mWindow->AddOverlay(f, this->icon);
        mWindow->Repaint();
    }
}


/// <summary>
/// Repositions the icon.
/// </summary>
void TrayIcon::Reposition(RECT rect) {
    mWindow->SetPosition(rect);
}


/// <summary>
/// Repositions the icon.
/// </summary>
void TrayIcon::Reposition(UINT x, UINT y, UINT width, UINT height) {
    mWindow->SetPosition(x, y, width, height);
}


/// <summary>
/// Gets the screen coordinate rect of this tray icon.
/// </summary>
void TrayIcon::GetScreenRect(LPRECT rect) {
    mWindow->GetScreenRect(rect);
}


/// <summary>
/// Sends a message to the owner of the icon.
/// </summary>
void TrayIcon::SendCallback(UINT message, WPARAM /* wParam */, LPARAM /* lParam */) {
    if (this->version >= 4) {
        RECT r;
        mWindow->GetScreenRect(&r);
        PostMessage(this->callbackWindow, this->callbackMessage, (WPARAM)MAKEWPARAM(r.left, r.top), (LPARAM)MAKELPARAM(message, this->callbackID));
    }
    else {
        PostMessage(this->callbackWindow, this->callbackMessage, (WPARAM)this->callbackID, (LPARAM)message);
    }
}


/// <summary>
/// Handles window messages for the icon's window.
/// </summary>
LRESULT WINAPI TrayIcon::HandleMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam, LPVOID) {
    if (message >= WM_MOUSEFIRST && message <= WM_MOUSELAST && this->showTip) {
        if (message == WM_MOUSEMOVE && !this->showingTip) {
            if (!IsWindow(this->callbackWindow)) {
                // The icon went away...
                LiteStep::LSNOTIFYICONDATA lsNID;
                lsNID.cbSize = sizeof(lsNID);
                lsNID.hWnd = this->callbackWindow;
                lsNID.uID = this->callbackID;
                lsNID.guidItem = this->guid;
                lsNID.uFlags = this->flags & NIF_GUID;
                PostMessage(gLSModule.GetMessageWindow(), LM_SYSTRAY, NIM_DELETE, (LPARAM)&lsNID);
                return 0;
            }
            this->showingTip = true;
            if (this->tip[0] != L'\0') {
                RECT r;
                mWindow->GetScreenRect(&r);
                ((Tray*)mParent)->ShowTip(this->tip, &r);
            }
        }

        if (message == WM_RBUTTONDOWN || message == WM_LBUTTONDOWN) {
            ((Tray*)mParent)->HideTip();
        }

        AllowSetForegroundWindow(mProcessID);

        if (this->version >= 4) {
            if (message == WM_RBUTTONUP) {
                SendCallback(WM_CONTEXTMENU, wParam, lParam);
            }
            else if (message == WM_LBUTTONUP) {
                SendCallback(NIN_SELECT, wParam, lParam);
            }
            else {
                SendCallback(message, wParam, lParam);
            }
        }
        else {
            SendCallback(message, wParam, lParam);
        }
    }
    else if (message == WM_MOUSELEAVE) {
        ((Tray*)mParent)->HideTip();
        this->showingTip = false;
    }
    else {
        return DefWindowProc(window, message, wParam, lParam);
    }
    return 0;
}
