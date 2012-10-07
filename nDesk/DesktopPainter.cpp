/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*   DesktopPainter.cpp
*   The nModules Project
*
*   Paints the desktop.
*   
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include <Windows.h>
#include <math.h>
#include "shlwapi.h"
#include "DesktopPainter.hpp"
#include "../nShared/Debugging.h"
#include "../nShared/MonitorInfo.hpp"
#include "../nShared/Factories.h"
#include "../nShared/Macros.h"
#include <d2d1.h>
#include <wincodec.h>
#include <assert.h>

extern MonitorInfo * g_pMonitorInfo;

using namespace D2D1;


/// <summary>
/// Creates a new instance of the DesktopPainter class.
/// </summary>
DesktopPainter::DesktopPainter(HWND hWnd) {
    DWORD start = GetTickCount();

    // Initalize
    m_pWallpaperBrush = NULL;
    m_pOldWallpaperBrush = NULL;
    m_TransitionEffect = NULL;
    m_pRenderTarget = NULL;
    m_bInvalidateAllOnUpdate = false;
    ZeroMemory(&m_TransitionSettings, sizeof(TransitionEffect::TransitionSettings));

    //
    m_hWnd = hWnd;

    //
    CalculateSizeDepdenentStuff();

    //
    m_TransitionSettings.iFrameInterval = 16; // 62.5 frames per second
    m_TransitionSettings.iTime = 625; // milliseconds
    m_TransitionSettings.iSquareSize = 100;
    m_TransitionSettings.fFadeTime = 0.2f;
    SetTransitionType(NONE);

    TRACE("It took %d ms to init", GetTickCount() - start);
}


/// <summary>
/// Destroys this instance of the DesktopPainter class.
/// </summary>
DesktopPainter::~DesktopPainter() {
    DiscardDeviceResources();

    if (m_TransitionEffect) {
        delete m_TransitionEffect;
    }
}


/// <summary>
/// Releases all D2D device depenent resources
/// </summary>
void DesktopPainter::DiscardDeviceResources() {
    SAFERELEASE(m_pRenderTarget)
    SAFERELEASE(m_pWallpaperBrush)
    SAFERELEASE(m_pOldWallpaperBrush)
}


/// <summary>
/// Returns the window we are drawing to.
/// </summary>
HWND DesktopPainter::GetWindow() {
    return m_hWnd;
}


/// <summary>
/// (Re)Creates all device dependent resources
/// </summary>
HRESULT DesktopPainter::ReCreateDeviceResources() {
    HRESULT hr = S_OK;

    if (!m_pRenderTarget) {
        ID2D1Factory *pD2DFactory = NULL;
        Factories::GetD2DFactory(reinterpret_cast<LPVOID*>(&pD2DFactory));

        D2D1_RENDER_TARGET_PROPERTIES p = RenderTargetProperties(D2D1_RENDER_TARGET_TYPE_DEFAULT, PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE));
        D2D1_HWND_RENDER_TARGET_PROPERTIES h = HwndRenderTargetProperties(m_hWnd, D2D1::SizeU(g_pMonitorInfo->m_virtualDesktop.width, g_pMonitorInfo->m_virtualDesktop.height));

        hr = pD2DFactory->CreateHwndRenderTarget(&p, &h, &m_pRenderTarget);

        // Update the wallpaper
        if (SUCCEEDED(hr)) {
            UpdateWallpaper(true);
        }
    }

    return hr;
}


/// <summary>
/// Changes the transition type
/// </summary>
void DesktopPainter::SetTransitionType(TransitionType transitionType) {
    m_TransitionType = transitionType;

    // If we are in the middle of a transition, let the current effect cleanup
    if (m_pOldWallpaperBrush != NULL) {
        m_TransitionEffect->End();
    }

    // Delete any existing transition effect
    if (m_TransitionEffect) {
        delete m_TransitionEffect;
    }

    //
    m_TransitionEffect = TransitionEffectFromType(transitionType);

    if (m_TransitionEffect) {
        m_TransitionEffect->Initialize(&m_TransitionSettings);
        if (m_pOldWallpaperBrush != NULL) {
            m_TransitionEffect->Start(m_pOldWallpaperBrush, m_pWallpaperBrush);
        }
    }
    else if (m_pOldWallpaperBrush != NULL) {
        m_TransitionSettings.iProgress = m_TransitionSettings.iTime;
        SAFERELEASE(m_pOldWallpaperBrush)
    }
}


/// <summary>
/// Retrives a new transition effect class based on the specified transition type
/// </summary>
TransitionEffect* DesktopPainter::TransitionEffectFromType(TransitionType transitionType) {
    switch (transitionType) {
    case FADE_IN: return new FadeEffect(FadeEffect::FadeType::FADE_IN);
    case FADE_OUT: return new FadeEffect(FadeEffect::FadeType::FADE_OUT);

    case SLIDE_BOTH_LEFT: return new SlideEffect(SlideEffect::SlideType::BOTH, SlideEffect::SlideDirection::LEFT);
    case SLIDE_BOTH_RIGHT: return new SlideEffect(SlideEffect::SlideType::BOTH, SlideEffect::SlideDirection::RIGHT);
    case SLIDE_BOTH_UP: return new SlideEffect(SlideEffect::SlideType::BOTH, SlideEffect::SlideDirection::UP);
    case SLIDE_BOTH_DOWN: return new SlideEffect(SlideEffect::SlideType::BOTH, SlideEffect::SlideDirection::DOWN);
    case SLIDE_IN_LEFT: return new SlideEffect(SlideEffect::SlideType::NEW, SlideEffect::SlideDirection::LEFT);
    case SLIDE_IN_RIGHT: return new SlideEffect(SlideEffect::SlideType::NEW, SlideEffect::SlideDirection::RIGHT);
    case SLIDE_IN_UP: return new SlideEffect(SlideEffect::SlideType::NEW, SlideEffect::SlideDirection::UP);
    case SLIDE_IN_DOWN: return new SlideEffect(SlideEffect::SlideType::NEW, SlideEffect::SlideDirection::DOWN);
    case SLIDE_OUT_LEFT: return new SlideEffect(SlideEffect::SlideType::OLD, SlideEffect::SlideDirection::LEFT);
    case SLIDE_OUT_RIGHT: return new SlideEffect(SlideEffect::SlideType::OLD, SlideEffect::SlideDirection::RIGHT);
    case SLIDE_OUT_UP: return new SlideEffect(SlideEffect::SlideType::OLD, SlideEffect::SlideDirection::UP);
    case SLIDE_OUT_DOWN: return new SlideEffect(SlideEffect::SlideType::OLD, SlideEffect::SlideDirection::DOWN);
    case SCAN_LEFT: return new SlideEffect(SlideEffect::SlideType::SCAN, SlideEffect::SlideDirection::LEFT);
    case SCAN_RIGHT: return new SlideEffect(SlideEffect::SlideType::SCAN, SlideEffect::SlideDirection::RIGHT);
    case SCAN_UP: return new SlideEffect(SlideEffect::SlideType::SCAN, SlideEffect::SlideDirection::UP);
    case SCAN_DOWN: return new SlideEffect(SlideEffect::SlideType::SCAN, SlideEffect::SlideDirection::DOWN);

    case SQUARES_RANDOM_IN: return new GridEffect(GridEffect::GridType::RANDOM, GridEffect::GridStyle::SHOW_NEW);
    case SQUARES_RANDOM_OUT: return new GridEffect(GridEffect::GridType::RANDOM, GridEffect::GridStyle::HIDE_OLD);
    case SQUARES_LINEAR_HORIZONTAL_IN: return new GridEffect(GridEffect::GridType::LINEAR_HORIZONTAL, GridEffect::GridStyle::SHOW_NEW);
    case SQUARES_LINEAR_HORIZONTAL_OUT: return new GridEffect(GridEffect::GridType::LINEAR_HORIZONTAL, GridEffect::GridStyle::HIDE_OLD);
    case SQUARES_LINEAR_VERTICAL_IN: return new GridEffect(GridEffect::GridType::LINEAR_VERTICAL, GridEffect::GridStyle::SHOW_NEW);
    case SQUARES_LINEAR_VERTICAL_OUT: return new GridEffect(GridEffect::GridType::LINEAR_VERTICAL, GridEffect::GridStyle::HIDE_OLD);
    case SQUARES_TRIANGULAR_BOTTOM_RIGHT_IN: return new GridEffect(GridEffect::GridType::TRIANGULAR, GridEffect::GridStyle::SHOW_NEW);
    case SQUARES_TRIANGULAR_BOTTOM_RIGHT_OUT: return new GridEffect(GridEffect::GridType::TRIANGULAR, GridEffect::GridStyle::HIDE_OLD);
    case SQUARES_CLOCKWISE_IN: return new GridEffect(GridEffect::GridType::CLOCKWISE, GridEffect::GridStyle::SHOW_NEW);
    case SQUARES_CLOCKWISE_OUT: return new GridEffect(GridEffect::GridType::CLOCKWISE, GridEffect::GridStyle::HIDE_OLD);
    case SQUARES_COUNTERCLOCKWISE_IN: return new GridEffect(GridEffect::GridType::COUNTERCLOCKWISE, GridEffect::GridStyle::SHOW_NEW);
    case SQUARES_COUNTERCLOCKWISE_OUT:return new GridEffect(GridEffect::GridType::COUNTERCLOCKWISE, GridEffect::GridStyle::HIDE_OLD);

    default: return NULL;
    }
}


/// <summary>
/// Changes the transition time
/// </summary>
void DesktopPainter::SetTransitionTime(int iTransitionTime) {
    assert(iTransitionTime >= 0);
    m_TransitionSettings.iTime = iTransitionTime;
}


/// <summary>
/// Changes the square size for square based animations
/// </summary>
void DesktopPainter::SetSquareSize(int iSquareSize) {
    m_TransitionSettings.iSquareSize = iSquareSize;
    if (m_TransitionEffect) m_TransitionEffect->Resize();
}


/// <summary>
/// Changes the frame interval.
/// </summary>
void DesktopPainter::SetFrameInterval(int iFrameInterval) {
    assert(iFrameInterval > 0);
    m_TransitionSettings.iFrameInterval = iFrameInterval;
}


/// <summary>
/// Changes m_bInvalidateAllOnUpdate.
/// </summary>
void DesktopPainter::SetInvalidateAllOnUpdate(bool bValue) {
    m_bInvalidateAllOnUpdate = bValue;
}


/// <summary>
/// Should be called when the desktop has been resized.
/// </summary>
void DesktopPainter::Resize() {
    // Resize the window
    SetWindowPos(m_hWnd, HWND_BOTTOM, g_pMonitorInfo->m_virtualDesktop.rect.left, g_pMonitorInfo->m_virtualDesktop.rect.top,
        g_pMonitorInfo->m_virtualDesktop.width, g_pMonitorInfo->m_virtualDesktop.height, SWP_NOACTIVATE|SWP_NOSENDCHANGING);

    // Resize the render target
    m_pRenderTarget->Resize(D2D1::SizeU(g_pMonitorInfo->m_virtualDesktop.width, g_pMonitorInfo->m_virtualDesktop.height));

    CalculateSizeDepdenentStuff();

    if (m_TransitionEffect) {
        m_TransitionEffect->Resize();
    }

    UpdateWallpaper(true);
}


/// <summary>
/// Should be called when the desktop has been resized, or during init.
/// </summary>
void DesktopPainter::CalculateSizeDepdenentStuff() {
    // Update the virtual desktop rect
    m_TransitionSettings.WPRect.top = 0;
    m_TransitionSettings.WPRect.left = 0;
    m_TransitionSettings.WPRect.bottom = (float)g_pMonitorInfo->m_virtualDesktop.height;
    m_TransitionSettings.WPRect.right = (float)g_pMonitorInfo->m_virtualDesktop.width;
}


/// <summary>
/// Updates the desktop wallpaper.
/// </summary>
/// <param name="bNoTransition">If true, there will be no transition.</param>
void DesktopPainter::UpdateWallpaper(bool bNoTransition) {
    // If we are currently doing a transition, end it.
    if (m_pOldWallpaperBrush != NULL) {
        TransitionEnd();
        KillTimer(m_hWnd, 1337);
    }

    m_pOldWallpaperBrush = m_pWallpaperBrush;
    CreateWallpaperBrush(&m_pWallpaperBrush);

    // If we are going to do a transition animation
    if (!bNoTransition && m_pOldWallpaperBrush != NULL && m_TransitionType != NONE) {
        TransitionStart();
    }
    else {
        SAFERELEASE(m_pOldWallpaperBrush)
    }

    Redraw();
}


/// <summary>
/// Causes the whole desktop window to be redrawn
/// </summary>
void DesktopPainter::Redraw() {
    InvalidateRect(m_bInvalidateAllOnUpdate ? NULL : m_hWnd, NULL, true);
    UpdateWindow(m_hWnd);
}


/// <summary>
/// Should be called when a new frame in the transition needs to be painted.
/// </summary>
void DesktopPainter::TransitionStep() {
    m_TransitionSettings.iProgress += m_TransitionSettings.iFrameInterval;
    if (m_TransitionSettings.iProgress >= m_TransitionSettings.iTime) {
        KillTimer(m_hWnd, 1337);
    }
    Redraw();
}


/// <summary>
/// Regular painting
/// </summary>
void DesktopPainter::Paint() {
    m_pRenderTarget->FillRectangle(m_TransitionSettings.WPRect, m_pWallpaperBrush);
}


/// <summary>
/// Called prior to the first painting call, to let the transition effect initialize.
/// </summary>
void DesktopPainter::TransitionStart() {
    m_TransitionSettings.iProgress = 0;
    SetTimer(m_hWnd, 1337, 16, NULL);
    m_TransitionEffect->Start(m_pOldWallpaperBrush, m_pWallpaperBrush);
}


/// <summary>
/// Called after the transition is done, to let the transition effect do cleanup.
/// </summary>
void DesktopPainter::TransitionEnd() {
    m_TransitionEffect->End();
    SAFERELEASE(m_pOldWallpaperBrush)

    // Repaint, just for good measure
    Paint();
}


/// <summary>
/// Paints a composite of the previous wallpaper and the current one.
/// </summary>
void DesktopPainter::PaintComposite() {
    m_TransitionEffect->Paint(m_pRenderTarget, min(1.0f, (float)m_TransitionSettings.iProgress/m_TransitionSettings.iTime));

    // We are done with the transition, let go of the old wallpaper
    if (m_TransitionSettings.iProgress >= m_TransitionSettings.iTime) {
        TransitionEnd();
    }
}


/// <summary>
/// Handles certain window messages.
/// </summary>
LRESULT DesktopPainter::HandleMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_ERASEBKGND:
        return 1;

    case WM_PAINT:
        if (SUCCEEDED(ReCreateDeviceResources())) {
            m_pRenderTarget->BeginDraw();

            // m_pOldWallpaperBrush being non zero indicates that we are in the middle of a transition
            if (m_pOldWallpaperBrush != NULL) {
                PaintComposite();
            }
            else {
                Paint();
            }

            // If EndDraw fails we need to recreate all device-dependent resources
            if (m_pRenderTarget->EndDraw() == D2DERR_RECREATE_TARGET) {
                DiscardDeviceResources();
            }
        }
        ValidateRect(hWnd, NULL);
        return 0;
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}


/// <summary>
/// Creates a brush of the current wallpaper.
/// </summary>
HRESULT DesktopPainter::CreateWallpaperBrush(ID2D1BitmapBrush** ppBitmapBrush) {
    // Information about the wallpaper
    WCHAR wszWallpaperPath[MAX_PATH];
    int iWallpaperStyle;
    bool bTileWallpaper;

    // Temporary values
    char szTemp[32];
    DWORD dwSize, dwType;

    // D2D/WIC interfaces
    ID2D1Bitmap *pBitmap = NULL;
    ID2D1BitmapRenderTarget* pBitmapRender = NULL;
    IWICImagingFactory *pWICFactory = NULL;
    IWICBitmapDecoder *pDecoder = NULL;
    IWICBitmapFrameDecode *pSource = NULL;
    IWICFormatConverter *pConverter = NULL;
    IWICBitmapScaler *pScaler = NULL;

    // Get the path to the wallpaper
    dwSize = sizeof(wszWallpaperPath); dwType = REG_SZ;
    SHGetValueW(HKEY_CURRENT_USER, L"Control Panel\\Desktop", L"Wallpaper", &dwType, &wszWallpaperPath, &dwSize);

    // Get whether or not to tile the wallpaper
    dwSize = sizeof(szTemp);
    SHGetValue(HKEY_CURRENT_USER, "Control Panel\\Desktop", "TileWallpaper", &dwType, &szTemp, &dwSize);
    bTileWallpaper = atoi(szTemp) ? true : false;

    // Get whether or not to stretch the wallpaper
    dwSize = sizeof(szTemp);
    SHGetValue(HKEY_CURRENT_USER, "Control Panel\\Desktop", "WallpaperStyle", &dwType, &szTemp, &dwSize);
    iWallpaperStyle = atoi(szTemp);

    // Create a bitmap the size of the virtual screen
    m_pRenderTarget->CreateCompatibleRenderTarget(&pBitmapRender);

    // Start rendering the wallpaper
    pBitmapRender->BeginDraw();

    // Fill the bitmap with the background color
    DWORD DesktopColor = GetSysColor(COLOR_DESKTOP);
    pBitmapRender->Clear(D2D1::ColorF(RGB(GetBValue(DesktopColor), GetGValue(DesktopColor), GetRValue(DesktopColor))));

    // Load the desktop background
    Factories::GetWICFactory(reinterpret_cast<LPVOID*>(&pWICFactory));
    if (SUCCEEDED(pWICFactory->CreateDecoderFromFilename(wszWallpaperPath, NULL, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &pDecoder))) {
        // Get the first frame of the wallpaper
        pDecoder->GetFrame(0, &pSource);

        // Get the size of the wallpaper
        UINT cxWallpaper, cyWallpaper;
        pSource->GetSize(&cxWallpaper, &cyWallpaper);

        if (bTileWallpaper) { // Tile
            // Convert it to a D2D1 bitmap
            pWICFactory->CreateFormatConverter(&pConverter);
            pConverter->Initialize(pSource, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.f, WICBitmapPaletteTypeMedianCut);
            m_pRenderTarget->CreateBitmapFromWicBitmap(pConverter, 0, &pBitmap);

            // The x/y points where we should start tiling the image
            int xInitial = -g_pMonitorInfo->m_virtualDesktop.rect.left + (int)floor((float)g_pMonitorInfo->m_virtualDesktop.rect.left/cxWallpaper)*cxWallpaper;
            int yInitial = -g_pMonitorInfo->m_virtualDesktop.rect.top + (int)floor((float)g_pMonitorInfo->m_virtualDesktop.rect.top/cyWallpaper)*cyWallpaper;
            // Do the tiling
            for (int x = xInitial; x < g_pMonitorInfo->m_virtualDesktop.width - g_pMonitorInfo->m_virtualDesktop.rect.left; x += cxWallpaper) {
                for (int y = yInitial; y < g_pMonitorInfo->m_virtualDesktop.height - g_pMonitorInfo->m_virtualDesktop.rect.top; y += cyWallpaper) {
                    D2D1_RECT_F f; f.top = (float)y; f.left = (float)x; f.right = (float)(x + cxWallpaper); f.bottom = (float)(y + cyWallpaper);
                    pBitmapRender->DrawBitmap(pBitmap, f);
                }
            }
        }
        else { // Some type of stretching
            // Work out the dimensions the wallpaper should be stretched to
            int WallpaperResX, WallpaperResY;
            double scaleX, scaleY;
            switch (iWallpaperStyle) {
            case 2: // Stretch
                WallpaperResX = g_pMonitorInfo->m_monitors[0].width;
                WallpaperResY = g_pMonitorInfo->m_monitors[0].height;
                break;
            case 6: // Fit
                scaleX = (double)g_pMonitorInfo->m_monitors[0].width/cxWallpaper;
                scaleY = (double)g_pMonitorInfo->m_monitors[0].height/cyWallpaper;
                if (scaleX > scaleY) {
                    WallpaperResY = g_pMonitorInfo->m_monitors[0].height;
                    WallpaperResX = (int)(scaleY*cxWallpaper);
                }
                else {
                    WallpaperResY = (int)(scaleX*cyWallpaper);
                    WallpaperResX = g_pMonitorInfo->m_monitors[0].width;
                }
                break;
            case 10: // Fill
                scaleX = (double)g_pMonitorInfo->m_monitors[0].width/cxWallpaper;   
                scaleY = (double)g_pMonitorInfo->m_monitors[0].height/cyWallpaper;
                if (scaleX < scaleY) {
                    WallpaperResY = g_pMonitorInfo->m_monitors[0].height;
                    WallpaperResX = (int)(scaleY*cxWallpaper);
                }
                else {
                    WallpaperResY = (int)(scaleX*cyWallpaper);
                    WallpaperResX = g_pMonitorInfo->m_monitors[0].width;
                }
                break;
            case 22: // Span, essentially a fill over the virtual desktop
                scaleX = (double)g_pMonitorInfo->m_virtualDesktop.width/cxWallpaper;    
                scaleY = (double)g_pMonitorInfo->m_virtualDesktop.height/cyWallpaper;
                if (scaleX < scaleY) {
                    WallpaperResY = g_pMonitorInfo->m_virtualDesktop.height;
                    WallpaperResX = (int)(scaleY*cxWallpaper);
                }
                else {
                    WallpaperResY = (int)(scaleX*cyWallpaper);
                    WallpaperResX = g_pMonitorInfo->m_virtualDesktop.width;
                }
                break;
            default: // Center (actually 0), but this way we can fail graciously if the value is invalid
                WallpaperResX = cxWallpaper;
                WallpaperResY = cyWallpaper;
                break;
            }

            // Stretch the wallpaper as necesary
            pWICFactory->CreateBitmapScaler(&pScaler);
            pScaler->Initialize(pSource, (UINT)WallpaperResX, (UINT)WallpaperResY, WICBitmapInterpolationModeCubic);

            // Convert it to a D2D1 bitmap
            pWICFactory->CreateFormatConverter(&pConverter);
            pConverter->Initialize(pScaler, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.f, WICBitmapPaletteTypeMedianCut);
            m_pRenderTarget->CreateBitmapFromWicBitmap(pConverter, 0, &pBitmap);

            if (iWallpaperStyle == 22) {
                // Center the stretched wallpaper on the virtual desktop
                D2D1_RECT_F dest, source;
                dest.left = 0;
                dest.top = 0;
                dest.right = (float)g_pMonitorInfo->m_virtualDesktop.width;
                dest.bottom = (float)g_pMonitorInfo->m_virtualDesktop.height;
                source = dest;

                if (WallpaperResY == g_pMonitorInfo->m_virtualDesktop.height) {
                    // Center by width
                    source.left = (WallpaperResX - g_pMonitorInfo->m_virtualDesktop.width)/2.0f;
                    source.right = (float)(source.left + g_pMonitorInfo->m_virtualDesktop.width);
                }
                else {
                    // Center by height
                    source.top = (WallpaperResY - g_pMonitorInfo->m_virtualDesktop.height)/2.0f;
                    source.bottom = (float)(source.top + g_pMonitorInfo->m_virtualDesktop.height);
                }

                pBitmapRender->DrawBitmap(pBitmap, dest, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, source);
            }
            else {
                // Center the stretched wallpaper on all monitors
                for (int i = 0; i < (int)g_pMonitorInfo->m_monitors.size(); i++) {
                    D2D1_RECT_F dest, source;

                    // Work out X coordinates and width
                    if (g_pMonitorInfo->m_monitors[i].width > WallpaperResX) {
                        dest.left = (g_pMonitorInfo->m_monitors[i].width - WallpaperResX)/2.0f;
                        dest.right = (float)(dest.left + WallpaperResX);
                        source.left = 0.0f;
                        source.right = (float)WallpaperResX;
                    }
                    else {
                        dest.left = 0.0f;
                        dest.right = (float)g_pMonitorInfo->m_monitors[i].width;
                        source.left = (WallpaperResX - g_pMonitorInfo->m_monitors[i].width)/2.0f;
                        source.right = (float)(source.left + g_pMonitorInfo->m_monitors[i].width);
                    }
                    dest.left += g_pMonitorInfo->m_monitors[i].rect.left - g_pMonitorInfo->m_virtualDesktop.rect.left;
                    dest.right += g_pMonitorInfo->m_monitors[i].rect.left - g_pMonitorInfo->m_virtualDesktop.rect.left;

                    // Work out Y coordinates and height
                    if (g_pMonitorInfo->m_monitors[i].height > WallpaperResY) {
                        dest.top = (g_pMonitorInfo->m_monitors[i].height - WallpaperResY)/2.0f;
                        dest.bottom = (float)(dest.top + WallpaperResY);
                        source.top = 0.0f;
                        source.bottom = (float)WallpaperResY;
                    }
                    else {
                        dest.top = 0.0f;
                        dest.bottom = (float)g_pMonitorInfo->m_monitors[i].height;
                        source.top = (WallpaperResY - g_pMonitorInfo->m_monitors[i].height)/2.0f;
                        source.bottom = (float)(source.top + g_pMonitorInfo->m_monitors[i].height);
                    }
                    dest.top += g_pMonitorInfo->m_monitors[i].rect.top - g_pMonitorInfo->m_virtualDesktop.rect.top;
                    dest.bottom += g_pMonitorInfo->m_monitors[i].rect.top - g_pMonitorInfo->m_virtualDesktop.rect.top;

                    pBitmapRender->DrawBitmap(pBitmap, dest, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, source);
                }
            }
        }

        SAFERELEASE(pConverter)
        SAFERELEASE(pDecoder)
        SAFERELEASE(pBitmap)
        SAFERELEASE(pSource)
        SAFERELEASE(pScaler)
    }

    // Finish rendering
    pBitmapRender->EndDraw();

    pBitmapRender->GetBitmap(&pBitmap);
    m_pRenderTarget->CreateBitmapBrush(pBitmap, ppBitmapBrush);

    SAFERELEASE(pBitmap)
    SAFERELEASE(pBitmapRender)

    return S_OK;
}
