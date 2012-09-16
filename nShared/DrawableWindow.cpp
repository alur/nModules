/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  DrawableWindow.cpp                                              July, 2012
 *  The nModules Project
 *
 *  Implementation of the DrawableWindow class. A generic drawable window.
 *
 *  Big TODO::Transparent child windows should also have the parent window
 *  painted beneath them.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include <strsafe.h>
#include "../headers/lsapi.h"
#include "../nCoreCom/Core.h"
#include "Macros.h"
#include "DrawableWindow.hpp"
#include "PaintSettings.hpp"
#include <d2d1.h>
#include <dwmapi.h>
#include <dwrite.h>
#include <Wincodec.h>
#include "Factories.h"
#include "Debugging.h"

using namespace D2D1;


/// <summary>
/// Constructor
/// </summary>
DrawableWindow::DrawableWindow(HWND parent, LPCSTR pszWndClass, PaintSettings * paintSettings, HINSTANCE hInst) {
    m_hWndParent = parent;
    m_pszText = "Test";
    m_pPaintSettings = paintSettings;
    
    // Copy over the position from the paint settings.
    memcpy(&m_scPosition, &m_pPaintSettings->position, sizeof(RECT));

    CreateWnd(pszWndClass, hInst);
}


/// <summary>
/// Destructor
/// </summary>
DrawableWindow::~DrawableWindow() {
    KillTimer(m_hWnd, 1337);

    if (m_hWnd) {
        DestroyWindow(m_hWnd);
    }

    SAFERELEASE(&m_pRenderTarget);
    SAFERELEASE(&m_pBackBrush);
    SAFERELEASE(&m_pTextBrush);
    SAFERELEASE(&m_pTextFormat);
    PurgeOverlays();
}


/// <summary>
/// Gets the HWND of this class.
/// </summary>
HWND DrawableWindow::getWindow() {
    return m_hWnd;
}


/// <summary>
/// Reloads graphics resources from the paintsettings object.
/// </summary>
void DrawableWindow::UpdateBrushes() {
    // Update the background brush
    ID2D1SolidColorBrush *backBrush = reinterpret_cast<ID2D1SolidColorBrush*>(m_pBackBrush);
    backBrush->SetColor(m_pPaintSettings->backColor);

    Repaint();
}


/// <summary>
/// Repaints the entire window.
/// </summary>
void DrawableWindow::Repaint() {
    InvalidateRect(m_hWnd, NULL, TRUE);
    UpdateWindow(m_hWnd);
}


/// <summary>
/// Adds an overlay to this window.
/// </summary>
HRESULT DrawableWindow::AddOverlay(D2D1_RECT_F f, HICON hIcon) {
    IWICBitmap *pSource = NULL;
    IWICBitmapScaler *pScaler = NULL;
    IWICFormatConverter *pConverter = NULL;
    IWICImagingFactory *pFactory = NULL;
    ID2D1Bitmap *pBitmap = NULL;
    Overlay overlay;
    UINT width, height;
    
    HRESULT hr = S_OK;

    // Create our helper objects.
    CHECKHR(hr, Factories::GetWICFactory(reinterpret_cast<LPVOID*>(&pFactory)));
    CHECKHR(hr, pFactory->CreateBitmapScaler(&pScaler));
    CHECKHR(hr, pFactory->CreateFormatConverter(&pConverter));

    // Generate a WIC bitmap
    CHECKHR(hr, pFactory->CreateBitmapFromHICON(hIcon, &pSource));

    // Resize the image
    CHECKHR(hr, pSource->GetSize(&width, &height));
    CHECKHR(hr, pScaler->Initialize(pSource, (UINT)(f.right - f.left), (UINT)(f.bottom - f.top), WICBitmapInterpolationModeCubic));
    
    // Convert it to an ID2D1Bitmap
    CHECKHR(hr, pConverter->Initialize(pScaler, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.f, WICBitmapPaletteTypeMedianCut));
    CHECKHR(hr, m_pRenderTarget->CreateBitmapFromWicBitmap(pConverter, 0, &pBitmap));

    // Create a brush based on the bitmap
    CHECKHR(hr, m_pRenderTarget->CreateBitmapBrush(pBitmap, reinterpret_cast<ID2D1BitmapBrush**>(&overlay.brush)));
    
    // Move the origin of the brush to match the overlay position
    overlay.brush->SetTransform(D2D1::Matrix3x2F::Translation(f.left, f.top));
    overlay.position = f;

    // Transfer control here if CHECKHR failed
    CHECKHR_END();

    // Release stuff
    SAFERELEASE(&pScaler);
    SAFERELEASE(&pConverter);
    SAFERELEASE(&pSource);
    SAFERELEASE(&pBitmap);

    // Add the overlays to the overlay list
    if (SUCCEEDED(hr)) {
        m_overlays.push_back(overlay);
    }
    else {
        TRACE("DrawableWindow::AddOverlay failed");
    }

    return hr;
}


/// <summary>
/// Removes all overlays from the window.
/// </summary>
void DrawableWindow::PurgeOverlays() {
    for (vector<Overlay>::iterator iter = m_overlays.begin(); iter != m_overlays.end(); iter++) {
        SAFERELEASE(&iter->brush);
    }
    m_overlays.clear();
}


/// <summary>
/// Enables/Disables bluring of the window background.
/// </summary>
HRESULT DrawableWindow::SetBlur(bool bEnable) {
    // Specify blur-behind and blur region.
    DWM_BLURBEHIND bb = {0};
    bb.dwFlags = bEnable ? DWM_BB_ENABLE : NULL;
    bb.hRgnBlur = NULL;
    bb.fEnable = bEnable;

    // Enable blur-behind.
    return DwmEnableBlurBehindWindow(m_hWnd, &bb);
}


/// <summary>
/// Creates the window we are drawing.
/// </summary>
bool DrawableWindow::CreateWnd(LPCSTR pszWndClass, HINSTANCE hInst) {
    if (m_hWndParent) {
        RECT parentRect;
        GetWindowRect(m_hWndParent, &parentRect);
        m_scPosition.bottom += parentRect.top;
        m_scPosition.top += parentRect.top;
        m_scPosition.left += parentRect.left;
        m_scPosition.right += parentRect.left;
    }

    DWORD exStyle = WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW;
    DWORD style = WS_POPUP;

    if (m_pPaintSettings->m_bAlwaysOnTop) exStyle |= WS_EX_TOPMOST;

    m_hWnd = CreateWindowEx(exStyle, pszWndClass, "", style,
        m_scPosition.left, m_scPosition.top, m_scPosition.right - m_scPosition.left,
        m_scPosition.bottom - m_scPosition.top, m_hWndParent, NULL, hInst, NULL);
    
    if (!m_hWnd) {
        return false;
    }

    D2D1_SIZE_U size = D2D1::SizeU(
        m_scPosition.right - m_scPosition.left,
        m_scPosition.bottom - m_scPosition.top
    );

    m_backArea.top = 0;
    m_backArea.left = 0;
    m_backArea.right = (float)size.width;
    m_backArea.bottom = (float)size.height;
    m_textArea = m_backArea;
    m_textArea.bottom -= m_pPaintSettings->textOffset.bottom;
    m_textArea.top += m_pPaintSettings->textOffset.top;
    m_textArea.left += m_pPaintSettings->textOffset.left;
    m_textArea.right -= m_pPaintSettings->textOffset.right;

    // Create Direct2D objects
    ID2D1Factory *pD2DFactory = NULL;
    Factories::GetD2DFactory(reinterpret_cast<LPVOID*>(&pD2DFactory));
    pD2DFactory->CreateHwndRenderTarget(
        RenderTargetProperties(
            D2D1_RENDER_TARGET_TYPE_DEFAULT,
            PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)
        ),
        HwndRenderTargetProperties(m_hWnd, size),
        &m_pRenderTarget
    );

    m_pRenderTarget->CreateSolidColorBrush(m_pPaintSettings->backColor, (ID2D1SolidColorBrush**)&m_pBackBrush);
    m_pRenderTarget->CreateSolidColorBrush(m_pPaintSettings->fontColor, (ID2D1SolidColorBrush**)&m_pTextBrush);

    // Create DirectWrite objects
    IDWriteFactory *pDWFactory = NULL;
    Factories::GetDWriteFactory(reinterpret_cast<LPVOID*>(&pDWFactory));
    pDWFactory->CreateTextFormat(
        m_pPaintSettings->font,
        NULL,
        DWRITE_FONT_WEIGHT_REGULAR,
        DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        m_pPaintSettings->fontSize,
        L"en-US",
        &m_pTextFormat);

    m_pTextFormat->SetTextAlignment(m_pPaintSettings->textAlignment);
    m_pTextFormat->SetParagraphAlignment(m_pPaintSettings->textVerticalAlignment);

    SetWindowPos(m_hWnd, 0, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);

    MARGINS m;
    ZeroMemory(&m, sizeof(m));
    m.cyTopHeight = m_scPosition.bottom - m_scPosition.top;
    m.cyTopHeight = INT_MAX;
    
    DwmExtendFrameIntoClientArea(m_hWnd, &m);

    SetTimer(m_hWnd, 1337, 1000, NULL);

    return true;
}


/// <summary>
/// Shows the window.
/// </summary>
void DrawableWindow::Show() {
    ShowWindow(m_hWnd, SW_SHOWNOACTIVATE);
}


/// <summary>
/// Causes the window to move to the position specified in its paintsettings.
/// </summary>
void DrawableWindow::UpdatePosition() {
    // Copy over the position from the paint settings.
    memcpy(&m_scPosition, &m_pPaintSettings->position, sizeof(RECT));

    // If this is a child window we need to make its position relative to its parent.
    if (m_hWndParent) {
        RECT parentRect;
        GetWindowRect(m_hWndParent, &parentRect);
        m_scPosition.bottom += parentRect.top;
        m_scPosition.top += parentRect.top;
        m_scPosition.left += parentRect.left;
        m_scPosition.right += parentRect.left;
    }

    D2D1_SIZE_U size = D2D1::SizeU(
        m_scPosition.right - m_scPosition.left,
        m_scPosition.bottom - m_scPosition.top
    );

    m_backArea.top = 0;
    m_backArea.left = 0;
    m_backArea.right = (float)size.width;
    m_backArea.bottom = (float)size.height;

    m_textArea = m_backArea;
    m_textArea.bottom -= m_pPaintSettings->textOffset.bottom;
    m_textArea.top += m_pPaintSettings->textOffset.top;
    m_textArea.left += m_pPaintSettings->textOffset.left;
    m_textArea.right -= m_pPaintSettings->textOffset.right;

    SetWindowPos(m_hWnd, 0, m_scPosition.left, m_scPosition.top, size.width, size.height, SWP_NOZORDER | SWP_FRAMECHANGED);
    m_pRenderTarget->Resize(size);

    InvalidateRect(m_hWnd, NULL, TRUE);
    UpdateWindow(m_hWnd);
}


/// <summary>
/// Handles window messages for this drawablewindow.
/// </summary>
LRESULT WINAPI DrawableWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_ERASEBKGND:
        return 1;

    case WM_PAINT:
        WCHAR wszText[MAX_LINE_LENGTH];
        m_pRenderTarget->BeginDraw();
        m_pRenderTarget->Clear();
        m_pRenderTarget->FillRectangle(m_backArea, m_pBackBrush);
        for (vector<Overlay>::iterator iter = m_overlays.begin(); iter != m_overlays.end(); iter++) {
            m_pRenderTarget->FillRectangle(iter->position, iter->brush);
        }
        nCore::System::FormatText(m_pPaintSettings->text, sizeof(wszText)/sizeof(WCHAR), wszText);
        m_pRenderTarget->DrawText(wszText, lstrlenW(wszText), m_pTextFormat, m_textArea, m_pTextBrush, D2D1_DRAW_TEXT_OPTIONS_CLIP);
        m_pRenderTarget->EndDraw();
        
        ValidateRect(m_hWnd, NULL);

        return 0;

    case WM_SIZE:
        return 0;

    case WM_TIMER:
        switch (wParam) {
        case 1337:
            Repaint();
            return 1;
        }
        break;

    case WM_DISPLAYCHANGE:
        return 0;

    case WM_LBUTTONDOWN:
        {
        }
        return 0;

    case WM_RBUTTONUP:
        {
            //LSExecute(NULL, "!About", SW_SHOW);
        }
        return 0;

    }
    return DefWindowProc(m_hWnd, uMsg, wParam, lParam);
}
