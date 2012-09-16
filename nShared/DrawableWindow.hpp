/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  DrawableWindow.hpp                                              July, 2012
 *  The nModules Project
 *
 *  Function declarations for the DrawableWindow class.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#ifndef DRAWABLEWINDOW_HPP
#define DRAWABLEWINDOW_HPP

#include "PaintSettings.hpp"
#include <d2d1.h>
#include <vector>

using std::vector;

class DrawableWindow {
public:
    explicit DrawableWindow(HWND, LPCSTR, PaintSettings*, HINSTANCE);
    virtual ~DrawableWindow();

    HWND getWindow();

    LRESULT WINAPI HandleMessage(UINT, WPARAM, LPARAM);
    void UpdatePosition();

    HRESULT AddOverlay(D2D1_RECT_F, HICON);
    void PurgeOverlays();
    void UpdateOverlay(LPVOID, HICON);
    void UpdateBrushes();
    void Repaint();
    void Show();

private:
    // All we need in order to paint some type of overlay
    typedef struct Overlay {
        D2D1_RECT_F position;
        ID2D1Brush *brush;
        ID2D1Bitmap *bitmap;
    } Overlay;

    // The screen position of the window
    RECT m_scPosition;

    // 
    vector<Overlay> m_overlays;

    // The HWND we are rendering to
    HWND m_hWnd;

    // The parent of this window
    HWND m_hWndParent;

    // 
    PaintSettings* m_pPaintSettings;

    //
    ID2D1HwndRenderTarget *m_pRenderTarget;

    //
    ID2D1Brush *m_pBackBrush;

    //
    ID2D1Brush *m_pTextBrush;

    //
    IDWriteTextFormat *m_pTextFormat;

    // The text we are currently drawing
    LPCSTR m_pszText;

    // 
    D2D1_RECT_F m_backArea;

    //
    D2D1_RECT_F m_textArea;

    //
    HRESULT SetBlur(bool);

    //
    bool CreateWnd(LPCSTR, HINSTANCE);
};

#endif /* DRAWABLEWINDOW_HPP */
