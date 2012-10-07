/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  DrawableWindow.cpp
 *  The nModules Project
 *
 *  A generic drawable window.
 *  
 *  TODO::Possibly implement the default settings as a state. 
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../headers/lsapi.h"
#include <strsafe.h>
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
#include "MessageHandler.hpp"

using namespace D2D1;


#define TIMER_UPDATE_TEXT 1


/// <summary>
/// Constructor used by LSModule to create a top-level window.
/// </summary>
DrawableWindow::DrawableWindow(HWND parent, LPCSTR windowClass, HINSTANCE instance, Settings* settings) {
    this->activeState = this->states.end();
    this->backBrush = NULL;
    this->defaultDrawingSettings = NULL;
    ZeroMemory(&this->drawingArea, sizeof(this->drawingArea));
    this->drawingSettings = new DrawableSettings();
    this->parent = NULL;
    this->renderTarget = NULL;
    this->settings = settings;
    this->text[0] = '\0';
    ZeroMemory(&this->textArea, sizeof(this->textArea));
    this->textBrush = NULL;
    this->textFormat = NULL;
    this->visible = false;
    this->window = MessageHandler::CreateMessageWindowEx(WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW | WS_EX_COMPOSITED,
        windowClass, "", WS_POPUP, 0, 0, 0, 0, parent, NULL, instance, this);
    SetWindowPos(this->window, 0, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
    SetWindowLongPtr(this->window, GWLP_USERDATA, MAGIC_DWORD);

    // All frame :p
    MARGINS m;
    ZeroMemory(&m, sizeof(m));
    m.cyTopHeight = INT_MAX;
    DwmExtendFrameIntoClientArea(this->window, &m);

    SetTimer(this->window, TIMER_UPDATE_TEXT, 1000, NULL);
}


/// <summary>
/// Constructor used by CreateChild to create a child window.
/// </summary>
DrawableWindow::DrawableWindow(DrawableWindow* parent, Settings* settings) {
    this->activeState = this->states.end();
    this->backBrush = NULL;
    this->defaultDrawingSettings = NULL;
    ZeroMemory(&this->drawingArea, sizeof(this->drawingArea));
    this->drawingSettings = new DrawableSettings();
    this->parent = parent;
    this->renderTarget = NULL;
    this->settings = settings;
    this->text[0] = '\0';
    ZeroMemory(&this->textArea, sizeof(this->textArea));
    this->textBrush = NULL;
    this->textFormat = NULL;
    this->visible = false;
    this->window = NULL;
}


/// <summary>
/// Destroys all children and frees allocated resources.
/// </summary>
DrawableWindow::~DrawableWindow() {
    if (this->parent) {
        this->parent->RemoveChild(this);
    }

    if (!this->parent && this->window) {
        KillTimer(this->window, TIMER_UPDATE_TEXT);
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

    if (!this->parent) {
        SAFERELEASE(this->renderTarget);
    }

    SAFERELEASE(this->backBrush);
    SAFERELEASE(this->textBrush);
    SAFERELEASE(this->textFormat);
}


/// <summary>
/// Adds an overlay icon.
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
    
    // Make the position relative to the drawing area.
    overlay.position = position;
    overlay.drawingPosition = overlay.position;
    overlay.drawingPosition.left += this->drawingArea.left;
    overlay.drawingPosition.right += this->drawingArea.left;
    overlay.drawingPosition.top += this->drawingArea.top;
    overlay.drawingPosition.bottom += this->drawingArea.top;

    // Move the origin of the brush to match the overlay position
    overlay.brush->SetTransform(Matrix3x2F::Translation(overlay.drawingPosition.left, overlay.drawingPosition.top));

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
        Repaint();
    }
    else {
        TRACE("DrawableWindow::AddOverlay failed!");
    }

    return hr;
}


/// <summary>
/// Removes all overlays from the window.
/// </summary>
void DrawableWindow::ClearOverlays() {
    for (list<Overlay>::iterator iter = this->overlays.begin(); iter != this->overlays.end(); iter++) {
        SAFERELEASE(iter->brush);
    }
    this->overlays.clear();
}


/// <summary>
/// Initalizes this window.
/// </summary>
void DrawableWindow::Initialize(DrawableSettings* defaultSettings) {
    this->defaultDrawingSettings = defaultSettings;
    this->drawingSettings->Load(this->settings, this->defaultDrawingSettings);

    // Create D2D resources
    ReCreateDeviceResources();

    //
    SetPosition(this->drawingSettings->x, this->drawingSettings->y, this->drawingSettings->width, this->drawingSettings->height);

    // Create the text format
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

    // Set the horizontal text alignment
    if (_stricmp(this->drawingSettings->textAlign, "Center") == 0)
        this->textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
    else if (_stricmp(this->drawingSettings->textAlign, "Right") == 0)
        this->textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
    else
        this->textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);

    // Set the vertical text alignment
    if (_stricmp(this->drawingSettings->textVerticalAlign, "Middle") == 0)
        this->textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    else if (_stricmp(this->drawingSettings->textVerticalAlign, "Bottom") == 0)
        this->textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_FAR);
    else
        this->textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);

    SetText(this->drawingSettings->text);
}


DrawableSettings* DrawableWindow::GetDrawingSettings() {
    return this->drawingSettings;
}


void DrawableWindow::Move(int x, int y) {
    SetPosition(x, y, this->drawingSettings->width, this->drawingSettings->height);
}


void DrawableWindow::SetPosition(int x, int y, int width, int height) {
    //
    this->drawingSettings->x = x;
    this->drawingSettings->y = y;
    this->drawingSettings->width = width;
    this->drawingSettings->height = height;

    // Position the window and/or set the backarea. TODO::Should call resize instead.
    if (!this->parent) {
        SetWindowPos(this->window, 0, this->drawingSettings->x, this->drawingSettings->y,
            this->drawingSettings->width, this->drawingSettings->height, SWP_NOZORDER | SWP_FRAMECHANGED);
        this->drawingArea = D2D1::RectF(0, 0, (float)this->drawingSettings->width, (float)this->drawingSettings->height);
        D2D1_SIZE_U size = D2D1::SizeU(width, height);
        this->renderTarget->Resize(size);
    }
    else {
        this->drawingArea = D2D1::RectF(
                this->parent->drawingArea.left + this->drawingSettings->x,
                this->parent->drawingArea.top + this->drawingSettings->y,
                this->parent->drawingArea.left + this->drawingSettings->x + this->drawingSettings->width,
                this->parent->drawingArea.top + this->drawingSettings->y + this->drawingSettings->height
            );
    }

    // The text area is offset from the drawing area.
    this->textArea = this->drawingArea;
    this->textArea.bottom -= this->drawingSettings->textOffsetBottom;
    this->textArea.top += this->drawingSettings->textOffsetTop;
    this->textArea.left += this->drawingSettings->textOffsetLeft;
    this->textArea.right -= this->drawingSettings->textOffsetRight;

    // Update all overlays
    for (list<Overlay>::iterator iter = overlays.begin(); iter != overlays.end(); ++iter) {
        iter->drawingPosition = iter->position;
        iter->drawingPosition.left += this->drawingArea.left;
        iter->drawingPosition.right += this->drawingArea.left;
        iter->drawingPosition.top += this->drawingArea.top;
        iter->drawingPosition.bottom += this->drawingArea.top;

        // Move the origin of the brush to match the overlay position
        iter->brush->SetTransform(Matrix3x2F::Identity());
        iter->brush->SetTransform(Matrix3x2F::Translation(iter->drawingPosition.left, iter->drawingPosition.top));
    }
}

/// <summary>
/// (Re)Creates all device dependent resources
/// </summary>
HRESULT DrawableWindow::ReCreateDeviceResources() {
    HRESULT hr = S_OK;

    if (!this->renderTarget) {
        if (!this->parent) {
            ID2D1Factory *pD2DFactory = NULL;
            Factories::GetD2DFactory(reinterpret_cast<LPVOID*>(&pD2DFactory));

            // Create the render target
            D2D1_SIZE_U size = D2D1::SizeU(this->drawingSettings->width, this->drawingSettings->height);
            pD2DFactory->CreateHwndRenderTarget(
                RenderTargetProperties(
                    D2D1_RENDER_TARGET_TYPE_DEFAULT,
                    PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)
                ),
                HwndRenderTargetProperties(this->window, size),
                &this->renderTarget
            );
        
            // And the brushes
            this->renderTarget->CreateSolidColorBrush(Color::ARGBToD2D(this->drawingSettings->color), (ID2D1SolidColorBrush**)&this->backBrush);
            this->renderTarget->CreateSolidColorBrush(Color::ARGBToD2D(this->drawingSettings->fontColor), (ID2D1SolidColorBrush**)&this->textBrush);
        }
        else {
            this->renderTarget = this->parent->renderTarget;
            this->renderTarget->CreateSolidColorBrush(Color::ARGBToD2D(this->drawingSettings->color), (ID2D1SolidColorBrush**)&this->backBrush);
            this->renderTarget->CreateSolidColorBrush(Color::ARGBToD2D(this->drawingSettings->fontColor), (ID2D1SolidColorBrush**)&this->textBrush);
        }
    }

    return hr;
}


/// <summary>
/// Creates a child window.
/// </summary>
DrawableWindow* DrawableWindow::CreateChild(Settings* childSettings) {
    DrawableWindow* child = new DrawableWindow(this, childSettings);
    children.push_back(child);
    return child;
}


/// <summary>
/// Gets the screen position of the window.
/// </summary>
void DrawableWindow::GetScreenRect(LPRECT rect) {
    RECT r;
    GetWindowRect(this->window, &r);
    rect->left = r.left + (LONG)this->drawingArea.left;
    rect->top = r.top + (LONG)this->drawingArea.top;
    rect->right = rect->left + this->drawingSettings->width;
    rect->bottom = rect->top + this->drawingSettings->height;
}


/// <summary>
/// Repaints the entire window.
/// </summary>
void DrawableWindow::Repaint() {
    if (this->parent) {
        this->parent->Repaint();
    }
    else {
        InvalidateRect(this->window, NULL, TRUE);
        UpdateWindow(this->window);
    }
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
    state.active = false;
    state.drawingSettings = new DrawableSettings();

    state.drawingSettings->Load(state.settings, defaultSettings);

    // Insert the state based on its priority.
    list<State>::iterator iter;
    for (iter = states.begin(); iter != states.end() && iter->priority > state.priority; iter++);
    return states.insert(iter, state);
}


/// <summary>
/// Actives a certain state.
/// </summary>
void DrawableWindow::ActivateState(DrawableWindow::STATE state) {
    state->active = true;
    if (this->activeState == this->states.end() || this->activeState->priority < state->priority) {
        this->activeState = state;
        HandleActiveStateChange();
    }
}


/// <summary>
/// Clears a certain state.
/// </summary>
void DrawableWindow::ClearState(DrawableWindow::STATE state) {
    state->active = false;
    if (state == this->activeState) {
        // We just cleared the active state, find the highest priority next active state.
        for (state++; state != this->states.end() && !state->active; state++);
        this->activeState = state;
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
/// Shows the window.
/// </summary>
void DrawableWindow::Show() {
    if (!this->parent) {
        ShowWindow(this->window, SW_SHOWNOACTIVATE);
    }
    this->visible = true;
}


/// <summary>
/// Hides the window.
/// </summary>
void DrawableWindow::Hide() {
    if (!this->parent) {
        ShowWindow(this->window, SW_HIDE);
    }
    this->visible = false;
}


void DrawableWindow::SetText(LPCWSTR text) {
    StringCchCopyW(this->drawingSettings->text, sizeof(this->drawingSettings->text)/sizeof(this->defaultDrawingSettings->text[0]), text);
    UpdateText();
}


/// <summary>
/// Handles window messages for this drawablewindow.
/// </summary>
LRESULT WINAPI DrawableWindow::HandleMessage(HWND window, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CTLCOLORSTATIC:
        return 1;

    case WM_ERASEBKGND:
        return 1;

    case WM_PAINT:
        {
            this->renderTarget->BeginDraw();
            this->renderTarget->Clear();
            Paint();
            this->renderTarget->EndDraw();

            ValidateRect(this->window, NULL);
        }
        return 0;

    case WM_SIZE:
        return 0;

    case WM_TIMER:
        switch (wParam) {
        case TIMER_UPDATE_TEXT:
            UpdateText();
        }
        break;

    case WM_DISPLAYCHANGE:
        return 0;

    case WM_LBUTTONDOWN:
        return 0;

    case WM_RBUTTONUP:
        return 0;

    }
    return DefWindowProc(window, msg, wParam, lParam);
}


/// <summary>
/// Removes the specified child.
/// </summary>
void DrawableWindow::Paint() {
    if (this->visible) {
        this->renderTarget->SetTransform(Matrix3x2F::Identity());

        this->renderTarget->FillRectangle(this->drawingArea, this->backBrush);
    
        this->renderTarget->SetTransform(Matrix3x2F::Rotation(this->drawingSettings->textRotation, Point2F(this->drawingSettings->width/2.0f,this->drawingSettings->height/2.0f)));
        this->renderTarget->DrawText(this->text, lstrlenW(this->text), this->textFormat, this->textArea, this->textBrush, D2D1_DRAW_TEXT_OPTIONS_CLIP);
        
        // Paint all overlays
        for (list<Overlay>::iterator iter = this->overlays.begin(); iter != this->overlays.end(); iter++) {
            this->renderTarget->FillRectangle(iter->drawingPosition, iter->brush);
        }

        // Paint all children
        for (list<DrawableWindow*>::const_iterator iter = this->children.begin(); iter != this->children.end(); ++iter) {
            (*iter)->Paint();
        }
    }
}


/// <summary>
/// Removes the specified child.
/// </summary>
void DrawableWindow::RemoveChild(DrawableWindow* child) {
    this->children.remove(child);
}


/// <summary>
/// Forcibly updates the text.
/// </summary>
void DrawableWindow::UpdateText() {
    nCore::System::FormatText(this->drawingSettings->text, sizeof(this->text)/sizeof(WCHAR), this->text);
    Repaint();
}
