/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  TrayManager.h
 *  The nModules Project
 *
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "TrayIcon.hpp"
#include <vector>

using std::vector;
using std::string;

namespace TrayManager {
    typedef struct {
        vector<TrayIcon*>* icons;
        GUID guidItem;
        UINT uID;
        HWND hwnd;
    } TRAYICONDATA, *LPTRAYICONDATA;

    typedef vector<LPTRAYICONDATA>::iterator TRAYICONDATAITER;
    typedef map<string, Tray*>::const_iterator TRAYSCITER;

    void Stop();
    void GetScreenRect(TRAYICONDATAITER icon, LPRECT rect);

    TRAYICONDATAITER FindIcon(GUID guid);
    TRAYICONDATAITER FindIcon(HWND hWnd, UINT uID);
    TRAYICONDATAITER FindIcon(LiteStep::LPLSNOTIFYICONDATA pNID);
    TRAYICONDATAITER FindIcon(LiteStep::LPSYSTRAYINFOEVENT pSTE);
    
    void AddIcon(LiteStep::LPLSNOTIFYICONDATA pNID);
    void DeleteIcon(LiteStep::LPLSNOTIFYICONDATA pNID);
    void ModifyIcon(LiteStep::LPLSNOTIFYICONDATA pNID);
    void SetFocus(LiteStep::LPLSNOTIFYICONDATA pNID);
    void SetVersion(LiteStep::LPLSNOTIFYICONDATA pNID);

    void InitCompleted();
    
    LRESULT ShellMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
}
