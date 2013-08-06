/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  nPopup.h
 *  The nModules Project
 *
 *  General declarations for nPopup.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

enum POPUPLINETYPE {
    POPUPLINETYPE_ENDNEW,
    POPUPLINETYPE_ENDFOLDER,
    POPUPLINETYPE_NEW,
    POPUPLINETYPE_FOLDER,
    POPUPLINETYPE_SEPARATOR,
    POPUPLINETYPE_INFO,
    POPUPLINETYPE_COMMAND,
    POPUPLINETYPE_CONTENT,
    POPUPLINETYPE_CONTENTPATH,
    POPUPLINETYPE_CONTENTPATHDYNAMIC,
    POPUPLINETYPE_INVALID,
    POPUPLINETYPE_CONTAINER
};

enum POPUPLEVEL {
    POPUPLEVEL_ROOT,
    POPUPLEVEL_NEW,
    POPUPLEVEL_FOLDER
};

POPUPLINETYPE ProcessPopupLine(LPCTSTR line, ContentPopup::ContentSource* source, LPTSTR title, UINT cchTitle,
    LPTSTR command, UINT cchCommand, LPTSTR icon, UINT cchIcon, LPTSTR prefix, UINT cchPrefix);

void LoadSettings();
void LoadPopups();
bool LoadPopup(LPVOID f, POPUPLEVEL level, Popup** out, LPCTSTR parentPrefix);
void __cdecl HandlePopupBang(HWND owner, LPCTSTR bang, LPCTSTR args);
