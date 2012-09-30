/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  DrawableWindow.cpp                                              July, 2012
 *  The nModules Project
 *
 *  A generic drawable window.
 *  
 *  TODO::Possibly implement the default settings as a state. 
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include <strsafe.h>
#include "../headers/lsapi.h"
#include "../nCoreCom/Core.h"
#include "Macros.h"
#include "DrawableWindow.hpp"
#include "DrawableSettings.hpp"
#include <d2d1.h>
#include <dwmapi.h>
#include <dwrite.h>
#include <Wincodec.h>
#include "Factories.h"
#include "Debugging.h"
#include "Color.h"

using namespace D2D1;

#define TIMER_UPDATE_TEXT 1


/// <summary>
/// Constructor
/// </summary>
DrawableWindow::DrawableWindow(HWND parent, LPCSTR windowClass, HINSTANCE instance, Settings *settings, DrawableSettings* defaultSettings) {
    this->parent = parent;
    this->settings = settings;
    this->drawingSettings = new DrawableSettings();
    this->defaultDrawingSettings = defaultSettings;
    this->nextState = 1;
    this->activeState = this->states.end();

    LoadSettings();

    CreateWnd(windowClass, instance);
}


/// <summary>
/// Destructor
/// </summary>
DrawableWindow::~DrawableWindow() {
    KillTimer(this->window, TIMER_UPDATE_TEXT);

    if (this->window) {
        DestroyWindow(this->window);
    }

    for (STATE iter = this->states.begin(); iter != this->states.end(); iter++) {
        SAFEDELETE(iter->defaultSettings);
        SAFEDELETE(iter->drawingSettings);
        SAFEDELETE(iter->settings);
    }
    this->states.clear();

    SAFEDELETE(this->drawingSettings);
    SAFEDELETE(this->defaultDrawingSettings);

    SAFERELEASE(this->renderTarget);
    SAFERELEASE(this->backBrush);
    SAFERELEASE(this->textBrush);
    SAFERELEASE(this->textFormat);
    PurgeOverlays();
}


/// <summary>
/// Gets the HWND of this class.
/// </summary>
void DrawableWindow::LoadSettings() {
    this->drawingSettings->Load(this->settings, this->defaultDrawingSettings);
    this->scPosition.left = this->drawingSettings->x;
    this->scPosition.top = this->drawingSettings->y;
    this->scPosition.right = this->scPosition.left + this->drawingSettings->width;
    this->scPosition.bottom = this->scPosition.top + this->drawingSettings->height;

    if (_stricmp(this->drawingSettings->textAlign, "Center") == 0)
        this->textAlignment = DWRITE_TEXT_ALIGNMENT_CENTER;
    else if (_stricmp(this->drawingSettings->textAlign, "Right") == 0)
        this->textAlignment = DWRITE_TEXT_ALIGNMENT_TRAILING;
    else
        this->textAlignment = DWRITE_TEXT_ALIGNMENT_LEADING;

    if (_stricmp(this->drawingSettings->textVerticalAlign, "Middle") == 0)
        this->textVerticalAlignment = DWRITE_PARAGRAPH_ALIGNMENT_CENTER;
    else if (_stricmp(this->drawingSettings->textVerticalAlign, "Bottom") == 0)
        this->textVerticalAlignment = DWRITE_PARAGRAPH_ALIGNMENT_FAR;
    else
        this->textVerticalAlignment = DWRITE_PARAGRAPH_ALIGNMENT_NEAR;
}


/// <summary>
/// Gets the HWND of this class.
/// </summary>
HWND DrawableWindow::GetWindow() {
    return this->window;
}


/// <summary>
/// Repaints the entire window.
/// </summary>
void DrawableWindow::Repaint() {
    InvalidateRect(this->window, NULL, TRUE);
    UpdateWindow(this->window);
}


/// <summary>
/// Adds a new state.
/// </summary>
DrawableWindow::STATE DrawableWindow::AddState(LPCSTR prefix, DrawableSettings* defaultSettings, int defaultPriority) {
    State state;
    state.defaultSettings = defaultSettings;
    state.settings = settings->CreateChild(prefix);
    state.settings->AppendGroup(settings->prefix);
    state.priority = state.settings->GetInt("Priority", defaultPriority);
    state.state = nextState <<= 1;
    state.active = false;
    state.drawingSettings = new DrawableSettings();

    state.drawingSettings->Load(state.settings, defaultSettings);

    // Insert the state based on its priority.
    list<State>::iterator iter;
    for (iter = states.begin(); iter != states.end() && iter->priority < state.priority; iter++);
    return states.insert(iter, state);
}


/// <summary>
/// Actives a certain state.
/// </summary>
void DrawableWindow::ActivateState(DrawableWindow::STATE state) {
    STATE iter = state;
    state->active = true;
    if (this->activeState == this->states.end() || this->activeState->priority > iter->priority) {
        this->activeState = iter;
        HandleActiveStateChange();
    }
}


/// <summary>
/// Clears a certain state.
/// </summary>
void DrawableWindow::ClearState(DrawableWindow::STATE state) {
    STATE iter = state;
    iter->active = false;
    if (iter == this->activeState) {
        // We just cleared the active state, find the highest priority next active state.
        for (iter++; iter != this->states.end() && !iter->active; iter++);
        this->activeState = iter;
        HandleActiveStateChange();
    }
}


/// <summary>
/// Clears a certain state.
/// </summary>
void DrawableWindow::HandleActiveStateChange() {
    DrawableSettings* drawSettings;
    if (this->activeState == this->states.end()) {
        drawSettings = this->drawingSettings;
    }
    else {
        drawSettings = this->activeState->drawingSettings;
    }

    // TODO::Really should update all settings.
    ID2D1SolidColorBrush *backBrush = reinterpret_cast<ID2D1SolidColorBrush*>(this->backBrush);
    backBrush->SetColor(Color::ARGBToD2D(drawSettings->color));

    Repaint();
}


/// <summary>
/// Adds an overlay to this window.
/// </summary>
HRESULT DrawableWindow::AddOverlay(D2D1_RECT_F position, HICON icon) {
    IWICBitmap* source = NULL;
    IWICBitmapScaler* scaler = NULL;
    IWICFormatConverter* converter = NULL;
    IWICImagingFactory* factory = NULL;
    ID2D1Bitmap* bitmap = NULL;
    Overlay overlay;
    UINT width, height;
    
    HRESULT hr = S_OK;

    // Create our helper objects.
    CHECKHR(hr, Factories::GetWICFactory(reinterpret_cast<LPVOID*>(&factory)));
    CHECKHR(hr, factory->CreateBitmapScaler(&scaler));
    CHECKHR(hr, factory->CreateFormatConverter(&converter));

    // Generate a WIC bitmap
    CHECKHR(hr, factory->CreateBitmapFromHICON(icon, &source));

    // Resize the image
    CHECKHR(hr, source->GetSize(&width, &height));
    CHECKHR(hr, scaler->Initialize(source, (UINT)(position.right - position.left), (UINT)(position.bottom - position.top), WICBitmapInterpolationModeCubic));
    
    // Convert it to an ID2D1Bitmap
    CHECKHR(hr, converter->Initialize(scaler, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.f, WICBitmapPaletteTypeMedianCut));
    CHECKHR(hr, this->renderTarget->CreateBitmapFromWicBitmap(converter, 0, &bitmap));

    // Create a brush based on the bitmap
    CHECKHR(hr, this->renderTarget->CreateBitmapBrush(bitmap, reinterpret_cast<ID2D1BitmapBrush**>(&overlay.brush)));
    
    // Move the origin of the brush to match the overlay position
    overlay.brush->SetTransform(D2D1::Matrix3x2F::Translation(position.left, position.top));
    overlay.position = position;

    // Transfer control here if CHECKHR failed
    CHECKHR_END();

    // Release stuff
    SAFERELEASE(scaler);
    SAFERELEASE(converter);
    SAFERELEASE(source);
    SAFERELEASE(bitmap);

    // Add the overlays to the overlay list
    if (SUCCEEDED(hr)) {
        this->overlays.push_back(overlay);
    }
    else {
        TRACE("DrawableWindow::AddOverlay failed!");
    }

    return hr;
}


DrawableSettings* DrawableWindow::GetSettings() {
    return this->drawingSettings;
}


/// <summary>
/// Removes all overlays from the window.
/// </summary>
void DrawableWindow::PurgeOverlays() {
    for (vector<Overlay>::iterator iter = this->overlays.begin(); iter != this->overlays.end(); iter++) {
        SAFERELEASE(iter->brush);
    }
    this->overlays.clear();
}


/// <summary>
/// Creates the window we are drawing.
/// </summary>
bool DrawableWindow::CreateWnd(LPCSTR pszWndClass, HINSTANCE hInst) {
    if (this->parent) {
        RECT parentRect;
        GetWindowRect(this->parent, &parentRect);
        this->scPosition.bottom += parentRect.top;
        this->scPosition.top += parentRect.top;
        this->scPosition.left += parentRect.left;
        this->scPosition.right += parentRect.left;
    }

    DWORD exStyle = WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW;
    DWORD style = WS_POPUP;

    if (this->drawingSettings->alwaysOnTop) exStyle |= WS_EX_TOPMOST;

    this->window = CreateWindowEx(exStyle, pszWndClass, "", style,
        this->scPosition.left, this->scPosition.top, this->scPosition.right - this->scPosition.left,
        this->scPosition.bottom - this->scPosition.top, this->parent, NULL, hInst, NULL);
    
    if (!this->window) {
        return false;
    }

    D2D1_SIZE_U size = D2D1::SizeU(
        this->scPosition.right - this->scPosition.left,
        this->scPosition.bottom - this->scPosition.top
    );

    this->backArea.top = 0;
    this->backArea.left = 0;
    this->backArea.right = (float)size.width;
    this->backArea.bottom = (float)size.height;
    this->textArea = this->backArea;
    this->textArea.bottom -= this->drawingSettings->textOffsetBottom;
    this->textArea.top += this->drawingSettings->textOffsetTop;
    this->textArea.left += this->drawingSettings->textOffsetLeft;
    this->textArea.right -= this->drawingSettings->textOffsetRight;

    // Create Direct2D objects
    ID2D1Factory *pD2DFactory = NULL;
    Factories::GetD2DFactory(reinterpret_cast<LPVOID*>(&pD2DFactory));
    pD2DFactory->CreateHwndRenderTarget(
        RenderTargetProperties(
            D2D1_RENDER_TARGET_TYPE_DEFAULT,
            PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)
        ),
        HwndRenderTargetProperties(this->window, size),
        &this->renderTarget
    );

    this->renderTarget->CreateSolidColorBrush(Color::ARGBToD2D(this->drawingSettings->color), (ID2D1SolidColorBrush**)&this->backBrush);
    this->renderTarget->CreateSolidColorBrush(Color::ARGBToD2D(this->drawingSettings->fontColor), (ID2D1SolidColorBrush**)&this->textBrush);

    // Create DirectWrite objects
    IDWriteFactory *pDWFactory = NULL;
    Factories::GetDWriteFactory(reinterpret_cast<LPVOID*>(&pDWFactory));
    pDWFactory->CreateTextFormat(
        this->drawingSettings->font,
        NULL,
        DWRITE_FONT_WEIGHT_REGULAR,
        DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        this->drawingSettings->fontSize,
        L"en-US",
        &this->textFormat);

    this->textFormat->SetTextAlignment(this->textAlignment);
    this->textFormat->SetParagraphAlignment(this->textVerticalAlignment);

    SetWindowPos(this->window, 0, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);

    MARGINS m;
    ZeroMemory(&m, sizeof(m));
    m.cyTopHeight = this->scPosition.bottom - this->scPosition.top;
    m.cyTopHeight = INT_MAX;
    
    DwmExtendFrameIntoClientArea(this->window, &m);

    SetTimer(this->window, TIMER_UPDATE_TEXT, 1000, NULL);

    return true;
}


/// <summary>
/// Shows the window.
/// </summary>
void DrawableWindow::Show() {
    ShowWindow(this->window, SW_SHOWNOACTIVATE);
}


/// <summary>
/// Causes the window to move to the position specified in its paintsettings.
/// </summary>
void DrawableWindow::UpdatePosition() {
    this->scPosition.left = this->drawingSettings->x;
    this->scPosition.top = this->drawingSettings->y;
    this->scPosition.right = this->scPosition.left + this->drawingSettings->width;
    this->scPosition.bottom = this->scPosition.top + this->drawingSettings->height;

    // If this is a child window we need to make its position relative to its parent.
    if (this->parent) {
        RECT parentRect;
        GetWindowRect(this->parent, &parentRect);
        this->scPosition.bottom += parentRect.top;
        this->scPosition.top += parentRect.top;
        this->scPosition.left += parentRect.left;
        this->scPosition.right += parentRect.left;
    }

    D2D1_SIZE_U size = D2D1::SizeU(
        this->scPosition.right - this->scPosition.left,
        this->scPosition.bottom - this->scPosition.top
    );

    this->backArea.top = 0;
    this->backArea.left = 0;
    this->backArea.right = (float)size.width;
    this->backArea.bottom = (float)size.height;

    this->textArea = this->backArea;
    this->textArea.bottom -= this->drawingSettings->textOffsetBottom;
    this->textArea.top += this->drawingSettings->textOffsetTop;
    this->textArea.left += this->drawingSettings->textOffsetLeft;
    this->textArea.right -= this->drawingSettings->textOffsetRight;

    SetWindowPos(this->window, 0, this->scPosition.left, this->scPosition.top, size.width, size.height, SWP_NOZORDER | SWP_FRAMECHANGED);
    this->renderTarget->Resize(size);

    InvalidateRect(this->window, NULL, TRUE);
    UpdateWindow(this->window);
}


/// <summary>
/// Handles window messages for this drawablewindow.
/// </summary>
LRESULT WINAPI DrawableWindow::HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_ERASEBKGND:
        return 1;

    case WM_PAINT:
        WCHAR wszText[MAX_LINE_LENGTH];
        this->renderTarget->BeginDraw();
        this->renderTarget->Clear();
        this->renderTarget->FillRectangle(this->backArea, this->backBrush);
        for (vector<Overlay>::iterator iter = this->overlays.begin(); iter != this->overlays.end(); iter++) {
            this->renderTarget->FillRectangle(iter->position, iter->brush);
        }
        nCore::System::FormatText(this->drawingSettings->text, sizeof(wszText)/sizeof(WCHAR), wszText);
        this->renderTarget->DrawText(wszText, lstrlenW(wszText), this->textFormat, this->textArea, this->textBrush, D2D1_DRAW_TEXT_OPTIONS_CLIP);
        this->renderTarget->EndDraw();
        
        ValidateRect(this->window, NULL);

        return 0;

    case WM_SIZE:
        return 0;

    case WM_TIMER:
        switch (wParam) {
        case TIMER_UPDATE_TEXT:
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
    return DefWindowProc(this->window, msg, wParam, lParam);
}
