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

POPUPLINETYPE ProcessPopupLine(LPCSTR line, ContentPopup::ContentSource* source, LPSTR title, UINT cchTitle,
    LPSTR command, UINT cchCommand, LPSTR icon, UINT cchIcon, LPSTR prefix, UINT cchPrefix);

void LoadSettings();
void LoadPopups();
bool LoadPopup(LPVOID f, POPUPLEVEL level, Popup** out, LPCSTR parentPrefix);
void __cdecl HandlePopupBang(HWND owner, LPCSTR bang, LPCSTR args);
