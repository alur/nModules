/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  nPopup.h
 *  The nModules Project
 *
 *  General declarations for nPopup.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

enum class PopupLineType
{
    EndNew, // ~New
    EndFolder, // ~Folder
    New, // !New
    Folder, // Folder
    Separator, // !Separator
    Info, // !Info
    Command, // Anything
    Content, // Retrieves its items from an external source
    ContentPath, // Retrieves its items from a path
    ContentPathDynamic, // Retrieves its items from a path, reloading the items each time it is opened
    Invalid, // An invalid line
    Container
};

enum class PopupLevel
{
    Root,
    New,
    Folder
};

static PopupLineType ProcessPopupLine(LPCTSTR line, ContentPopup::ContentSource* source, LPTSTR title, UINT cchTitle,
    LPTSTR command, UINT cchCommand, LPTSTR icon, UINT cchIcon, LPTSTR prefix, UINT cchPrefix);

static void LoadSettings();
static void LoadPopups();
static bool LoadPopup(LPVOID f, PopupLevel level, Popup** out, LPCTSTR parentPrefix);
void __cdecl HandlePopupBang(HWND owner, LPCTSTR bang, LPCTSTR args);
