/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Icon.hpp                                                   September, 2012
 *  The nModules Project
 *
 *  A single icon in an IconGroup.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#ifndef ICON_HPP
#define ICON_HPP

#include "../nShared/DrawableWindow.hpp"
#include "../nShared/PaintSettings.hpp"

class Icon {
public:
    explicit Icon();
    virtual ~Icon();

    LRESULT WINAPI HandleMessage(UINT, WPARAM, LPARAM);
    
private:
    HWND m_hWnd;

    PaintSettings* m_pPaintSettings;
    DrawableWindow* m_pWindow;
};

#endif /* ICON_HPP */
