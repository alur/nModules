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
        LSNOTIFYICONDATA NID;
    } TRAYICONDATA, *LPTRAYICONDATA;

    typedef vector<LPTRAYICONDATA>::iterator TRAYICONDATAITER;
    typedef map<string, Tray*>::const_iterator TRAYSCITER;

    void Stop();
    void GetScreenRect(TRAYICONDATAITER icon, LPRECT rect);

    TRAYICONDATAITER FindIcon(GUID guid);
    TRAYICONDATAITER FindIcon(HWND hWnd, UINT uID);
    TRAYICONDATAITER FindIcon(LPLSNOTIFYICONDATA pNID);
    TRAYICONDATAITER FindIcon(LPSYSTRAYINFOEVENT pSTE);
    
    void AddIcon(LPLSNOTIFYICONDATA pNID);
    void DeleteIcon(LPLSNOTIFYICONDATA pNID);
    void ModifyIcon(LPLSNOTIFYICONDATA pNID);
    void SetFocus(LPLSNOTIFYICONDATA pNID);
    void SetVersion(LPLSNOTIFYICONDATA pNID);

    void InitCompleted();
    
    
    LRESULT ShellMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
}
