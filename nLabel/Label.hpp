/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Label.hpp                                                       July, 2012
 *  The nModules Project
 *
 *  Declaration of the Label class.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#ifndef LABEL_HPP
#define LABEL_HPP

#include "../nShared/DrawableWindow.hpp"

class Label {
public:
    explicit Label(LPCSTR);
    virtual ~Label();

    void LoadSettings(bool = false);
    void CreateLabelWindow();
    LRESULT WINAPI HandleMessage(UINT, WPARAM, LPARAM);

private:
    // The name of this label
    LPCSTR m_pszName;

    // The label's window
    DrawableWindow* m_pWindow;

    // PaintSettings
    PaintSettings* m_pPaintSettings;

    // The parent window
    HWND m_hWndParent;
};

#endif /* LABEL_HPP */
