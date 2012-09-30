/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Tray.hpp                                                        July, 2012
 *  The nModules Project
 *
 *  Declaration of the Tray class.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#ifndef TRAY_HPP
#define TRAY_HPP

#include <map>
#include "TrayIcon.hpp"
#include "../nShared/DrawableWindow.hpp"
#include "../nShared/Settings.hpp"

using std::map;

class Tray {
public:
    explicit Tray(LPCSTR);
    virtual ~Tray();

    void LoadSettings(bool = false);
    TrayIcon* AddIcon(LPLSNOTIFYICONDATA);
    void RemoveIcon(TrayIcon*);
    LRESULT WINAPI HandleMessage(UINT, WPARAM, LPARAM);
    void Relayout();
    vector<TrayIcon*>::const_iterator FindIcon(TrayIcon* pNID);

private:
    // The name of this tray
    LPCSTR m_pszName;

    // The tray's window
    DrawableWindow* m_pWindow;

    // Settings for painting the tray background
    Settings* settings;

    // The tray icons
    vector<TrayIcon*> m_icons;

    // 
    RECT m_rMargin;

    // 
    int m_iRowSpacing;

    // 
    int m_iColSpacing;
};

#endif /* TRAY_HPP */
