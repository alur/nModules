/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  IconGroup.hpp                                                 August, 2012
 *  The nModules Project
 *
 *  A group of icons. Essentially an *nIcon.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#ifndef ICONGROUP_HPP
#define ICONGROUP_HPP

#include "../nShared/DrawableWindow.hpp"
#include "../nShared/PaintSettings.hpp"

class IconGroup {
public:
    explicit IconGroup();
    virtual ~IconGroup();

    LRESULT WINAPI HandleMessage(UINT, WPARAM, LPARAM);
    
private:
    HWND m_hWnd;

    PaintSettings* m_pPaintSettings;
    DrawableWindow* m_pWindow;
};

#endif /* ICONGROUP_HPP */
