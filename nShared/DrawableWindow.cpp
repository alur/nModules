/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  DrawableWindow.cpp
 *  The nModules Project
 *
 *  A generic drawable window.
 *  
 *  TODO::Possibly implement the default settings as a state. 
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../nShared/LiteStep.h"
#include <windowsx.h>
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


/// <summary>
/// Constructor used by LSModule to create a top-level window.
/// </summary>
DrawableWindow::DrawableWindow(HWND parent, LPCSTR windowClass, HINSTANCE instance, Settings* settings, MessageHandler* msgHandler) {
    this->activeChild = NULL;
    this->activeState = this->states.end();
    this->backBrush = NULL;
    this->defaultDrawingSettings = NULL;
    ZeroMemory(&this->drawingArea, sizeof(this->drawingArea));
    this->drawingSettings = new DrawableSettings();
    this->initialized = false;
    this->isTrackingMouse = false;
    this->msgHandler = msgHandler;
    this->monitorInfo = new MonitorInfo();
    this->parent = NULL;
    this->renderTarget = NULL;
    this->settings = settings;
    this->text[0] = '\0';
    ZeroMemory(&this->textArea, sizeof(this->textArea));
    this->textBrush = NULL;
    this->textFormat = NULL;
    this->timerIDs = new UIDGenerator<UINT_PTR>(1);
    this->userMsgIDs = new UIDGenerator<UINT>(WM_USER);
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

    this->updateTextTimer = SetCallbackTimer(1000, this);
}


/// <summary>
/// Constructor used by CreateChild to create a child window.
/// </summary>
DrawableWindow::DrawableWindow(DrawableWindow* parent, Settings* settings, MessageHandler* msgHandler) {
    this->activeChild = NULL;
    this->activeState = this->states.end();
    this->backBrush = NULL;
    this->defaultDrawingSettings = NULL;
    ZeroMemory(&this->drawingArea, sizeof(this->drawingArea));
    this->drawingSettings = new DrawableSettings();
    this->initialized = false;
    this->isTrackingMouse = false;
    this->msgHandler = msgHandler;
    this->monitorInfo = parent->monitorInfo;
    this->parent = parent;
    this->renderTarget = NULL;
    this->settings = settings;
    this->text[0] = '\0';
    ZeroMemory(&this->textArea, sizeof(this->textArea));
    this->textBrush = NULL;
    this->textFormat = NULL;
    this->timerIDs = NULL;
    this->userMsgIDs = NULL;
    this->updateTextTimer = NULL;
    this->visible = false;
    this->window = parent->window;
}


/// <summary>
/// Destroys all children and frees allocated resources.
/// </summary>
DrawableWindow::~DrawableWindow() {
    this->initialized = false;

    if (this->parent) {
        this->parent->RemoveChild(this);
    }

    if (!this->parent && this->window) {
        ClearCallbackTimer(this->updateTextTimer);
        DestroyWindow(this->window);
    }

    // Delete all states
    for (STATE iter = this->states.begin(); iter != this->states.end(); iter++) {
        SAFEDELETE(iter->defaultSettings);
        SAFEDELETE(iter->drawingSettings);
        SAFEDELETE(iter->settings);
    }
    this->states.clear();

    // Delete all overlays
    ClearOverlays();

    SAFEDELETE(this->drawingSettings);
    SAFEDELETE(this->defaultDrawingSettings);

    if (!this->parent) {
        SAFERELEASE(this->renderTarget);
    }

    if (!this->parent) {
        SAFEDELETE(this->monitorInfo);
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

    // Configure the mouse tracking struct
    ZeroMemory(&this->trackMouseStruct, sizeof(TRACKMOUSEEVENT));
    this->trackMouseStruct.cbSize = sizeof(TRACKMOUSEEVENT);
    this->trackMouseStruct.hwndTrack = this->window;
    this->trackMouseStruct.dwFlags = TME_LEAVE;
    this->trackMouseStruct.dwHoverTime = 200;

    // Create D2D resources
    ReCreateDeviceResources();

    //
    SetPosition(this->drawingSettings->x, this->drawingSettings->y, this->drawingSettings->width, this->drawingSettings->height);

    if (!this->parent && this->drawingSettings->alwaysOnTop) {
        SetParent(this->window, NULL);
        SetWindowPos(this->window, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);
    }

    // Create the text format
    ReCreateTextFormat(this->drawingSettings);
    
    // Set the text
    SetText(this->drawingSettings->text);

    this->initialized = true;
}


/// <summary>
/// (Re)Creates the textFormat based on the specified drawingSettings.
/// </summary>
void DrawableWindow::ReCreateTextFormat(DrawableSettings* drawingSettings) {
    // Drop the old textformat
    SAFERELEASE(this->textFormat);

    // Font weight
    DWRITE_FONT_WEIGHT fontWeight;
    if (_stricmp(drawingSettings->fontWeight, "Thin") == 0)
        fontWeight = DWRITE_FONT_WEIGHT_THIN;
    else if (_stricmp(drawingSettings->fontWeight, "Extra Light") == 0)
        fontWeight = DWRITE_FONT_WEIGHT_EXTRA_LIGHT;
    else if (_stricmp(drawingSettings->fontWeight, "Ultra Light") == 0)
        fontWeight = DWRITE_FONT_WEIGHT_ULTRA_LIGHT;
    else if (_stricmp(drawingSettings->fontWeight, "Light") == 0)
        fontWeight = DWRITE_FONT_WEIGHT_LIGHT;
    else if (_stricmp(drawingSettings->fontWeight, "Semi Light") == 0)
        fontWeight = DWRITE_FONT_WEIGHT_SEMI_LIGHT;
    else if (_stricmp(drawingSettings->fontWeight, "Regular") == 0)
        fontWeight = DWRITE_FONT_WEIGHT_REGULAR;
    else if (_stricmp(drawingSettings->fontWeight, "Medium") == 0)
        fontWeight = DWRITE_FONT_WEIGHT_MEDIUM;
    else if (_stricmp(drawingSettings->fontWeight, "Semi Bold") == 0)
        fontWeight = DWRITE_FONT_WEIGHT_SEMI_BOLD;
    else if (_stricmp(drawingSettings->fontWeight, "Bold") == 0)
        fontWeight = DWRITE_FONT_WEIGHT_BOLD;
    else if (_stricmp(drawingSettings->fontWeight, "Extra Bold") == 0)
        fontWeight = DWRITE_FONT_WEIGHT_EXTRA_BOLD;
    else if (_stricmp(drawingSettings->fontWeight, "Ultra Bold") == 0)
        fontWeight = DWRITE_FONT_WEIGHT_ULTRA_BOLD;
    else if (_stricmp(drawingSettings->fontWeight, "Black") == 0)
        fontWeight = DWRITE_FONT_WEIGHT_BLACK;
    else if (_stricmp(drawingSettings->fontWeight, "Heavy") == 0)
        fontWeight = DWRITE_FONT_WEIGHT_HEAVY;
    else if (_stricmp(drawingSettings->fontWeight, "Extra Black") == 0)
        fontWeight = DWRITE_FONT_WEIGHT_EXTRA_BLACK;
    else if (_stricmp(drawingSettings->fontWeight, "Ultra Black") == 0)
        fontWeight = DWRITE_FONT_WEIGHT_ULTRA_BLACK;
    else
        fontWeight = DWRITE_FONT_WEIGHT_NORMAL;

    // Font style
    DWRITE_FONT_STYLE fontStyle;
    if (_stricmp(drawingSettings->fontStyle, "Oblique") == 0)
        fontStyle = DWRITE_FONT_STYLE_OBLIQUE;
    else if (_stricmp(drawingSettings->fontStyle, "Italic") == 0)
        fontStyle = DWRITE_FONT_STYLE_ITALIC;
    else
        fontStyle = DWRITE_FONT_STYLE_NORMAL;

    // Font stretch
    DWRITE_FONT_STRETCH fontStretch;
    if (_stricmp(drawingSettings->fontStretch, "Ultra Condensed") == 0)
        fontStretch = DWRITE_FONT_STRETCH_ULTRA_CONDENSED;
    else if (_stricmp(drawingSettings->fontStretch, "Extra Condensed") == 0)
        fontStretch = DWRITE_FONT_STRETCH_EXTRA_CONDENSED;
    else if (_stricmp(drawingSettings->fontStretch, "Condensed") == 0)
        fontStretch = DWRITE_FONT_STRETCH_CONDENSED;
    else if (_stricmp(drawingSettings->fontStretch, "Semi Condensed") == 0)
        fontStretch = DWRITE_FONT_STRETCH_SEMI_CONDENSED;
    else if (_stricmp(drawingSettings->fontStretch, "Medium") == 0)
        fontStretch = DWRITE_FONT_STRETCH_MEDIUM;
    else if (_stricmp(drawingSettings->fontStretch, "Semi Expanded") == 0)
        fontStretch = DWRITE_FONT_STRETCH_SEMI_EXPANDED;
    else if (_stricmp(drawingSettings->fontStretch, "Expanded") == 0)
        fontStretch = DWRITE_FONT_STRETCH_EXPANDED;
    else if (_stricmp(drawingSettings->fontStretch, "Extra Expanded") == 0)
        fontStretch = DWRITE_FONT_STRETCH_EXTRA_EXPANDED;
    else if (_stricmp(drawingSettings->fontStretch, "Ultra Expanded") == 0)
        fontStretch = DWRITE_FONT_STRETCH_ULTRA_EXPANDED;
    else
        fontStretch = DWRITE_FONT_STRETCH_NORMAL;

    // Create the text format
    IDWriteFactory *pDWFactory = NULL;
    Factories::GetDWriteFactory(reinterpret_cast<LPVOID*>(&pDWFactory));
    pDWFactory->CreateTextFormat(
        drawingSettings->font,
        NULL,
        fontWeight,
        fontStyle,
        fontStretch,
        drawingSettings->fontSize,
        L"en-US",
        &this->textFormat);

    // Set the horizontal text alignment
    if (_stricmp(drawingSettings->textAlign, "Center") == 0)
        this->textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
    else if (_stricmp(drawingSettings->textAlign, "Right") == 0)
        this->textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
    else
        this->textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);

    // Set the vertical text alignment
    if (_stricmp(drawingSettings->textVerticalAlign, "Middle") == 0)
        this->textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    else if (_stricmp(drawingSettings->textVerticalAlign, "Bottom") == 0)
        this->textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_FAR);
    else
        this->textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);

    // Set the trimming method
    DWRITE_TRIMMING trimmingOptions;
    trimmingOptions.delimiter = 0;
    trimmingOptions.delimiterCount = 0;
    if (_stricmp(drawingSettings->textTrimmingGranularity, "None") == 0)
        trimmingOptions.granularity = DWRITE_TRIMMING_GRANULARITY_NONE;
    else if (_stricmp(drawingSettings->textVerticalAlign, "Word") == 0)
        trimmingOptions.granularity = DWRITE_TRIMMING_GRANULARITY_WORD;
    else
        trimmingOptions.granularity = DWRITE_TRIMMING_GRANULARITY_CHARACTER;

    this->textFormat->SetTrimming(&trimmingOptions, NULL);

    // Set word wrapping
    this->textFormat->SetWordWrapping(drawingSettings->wordWrap ? DWRITE_WORD_WRAPPING_WRAP : DWRITE_WORD_WRAPPING_NO_WRAP);

    // Set reading direction
    this->textFormat->SetReadingDirection(drawingSettings->rightToLeft ? DWRITE_READING_DIRECTION_RIGHT_TO_LEFT : DWRITE_READING_DIRECTION_LEFT_TO_RIGHT);
}


DrawableSettings* DrawableWindow::GetDrawingSettings() {
    return this->drawingSettings;
}


void DrawableWindow::Move(int x, int y) {
    SetPosition(x, y, this->drawingSettings->width, this->drawingSettings->height);
}


void DrawableWindow::Resize(int width, int height) {
    SetPosition(this->drawingSettings->x, this->drawingSettings->y, width, height);
}


bool DrawableWindow::IsVisible() {
    return this->visible;
}


void DrawableWindow::SetPosition(int x, int y, int width, int height) {
    //
    this->drawingSettings->x = x;
    this->drawingSettings->y = y;
    this->drawingSettings->width = width;
    this->drawingSettings->height = height;

    // Position the window and/or set the backarea.
    if (!this->parent) {
        SetWindowPos(this->window, 0, this->drawingSettings->x, this->drawingSettings->y,
            this->drawingSettings->width, this->drawingSettings->height, SWP_NOZORDER);
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
DrawableWindow* DrawableWindow::CreateChild(Settings* childSettings, MessageHandler* msgHandler) {
    DrawableWindow* child = new DrawableWindow(this, childSettings, msgHandler);
    children.push_back(child);
    return child;
}


MonitorInfo* DrawableWindow::GetMonitorInformation() {
    return this->monitorInfo;
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

HWND DrawableWindow::GetWindow() {
    return this->window;
}


void DrawableWindow::ClearCallbackTimer(UINT_PTR timer) {
    if (!this->parent) {
        KillTimer(this->window, timer);
        this->timers.erase(timer);
        this->timerIDs->ReleaseID(timer);
    }
    else {
        this->parent->ClearCallbackTimer(timer);
    }
}


UINT DrawableWindow::RegisterUserMessage(MessageHandler* msgHandler) {
    if (!this->parent) {
        UINT ret = this->userMsgIDs->GetNewID();
        this->userMessages.insert(std::pair<UINT, MessageHandler*>(ret, msgHandler));
        return ret;
    }
    else {
        return this->parent->RegisterUserMessage(msgHandler);
    }
}


void DrawableWindow::ReleaseUserMessage(UINT message) {
    if (!this->parent) {
        this->userMessages.erase(message);
        this->userMsgIDs->ReleaseID(message);
    }
    else {
        this->parent->ReleaseUserMessage(message);
    }
}


UINT_PTR DrawableWindow::SetCallbackTimer(UINT elapse, MessageHandler* msgHandler) {
    if (!this->parent) {
        UINT_PTR ret = SetTimer(this->window, this->timerIDs->GetNewID(), elapse, NULL);
        this->timers.insert(std::pair<UINT_PTR, MessageHandler*>(ret, msgHandler));
        return ret;
    }
    else {
        return this->parent->SetCallbackTimer(elapse, msgHandler);
    }
}


/// <summary>
/// Repaints the entire window.
/// </summary>
void DrawableWindow::Repaint(LPRECT region) {
    if (this->initialized && this->visible) {
        if (this->parent) {
            this->parent->Repaint(region);
        }
        else {
            InvalidateRect(this->window, region, TRUE);
            UpdateWindow(this->window);
        }
    }
}


/// <summary>
/// Sizes the window to fit its current text.
/// </summary>
void DrawableWindow::SizeToText(int maxWidth, int maxHeight) {
    IDWriteFactory* factory;
    IDWriteTextLayout* textLayout;
    DWRITE_TEXT_METRICS metrics;

    Factories::GetDWriteFactory(reinterpret_cast<LPVOID*>(&factory));
    factory->CreateTextLayout(this->text, lstrlenW(this->text), this->textFormat, (float)maxWidth, (float)maxHeight, &textLayout);
    textLayout->GetMetrics(&metrics);
    textLayout->Release();

    metrics.width += this->drawingSettings->textOffsetLeft + this->drawingSettings->textOffsetRight + 1;
    metrics.height += this->drawingSettings->textOffsetTop + this->drawingSettings->textOffsetBottom + 1;

    this->SetPosition(this->drawingSettings->x, this->drawingSettings->y, (int)metrics.width, (int)metrics.height);
}


/// <summary>
/// Adds a new state.
/// </summary>
DrawableWindow::STATE DrawableWindow::AddState(LPCSTR prefix, DrawableSettings* defaultSettings, int defaultPriority) {
    State state;
    state.defaultSettings = defaultSettings;
    state.settings = this->settings->CreateChild(prefix);
    state.settings->AppendGroup(this->settings);
    state.priority = state.settings->GetInt("Priority", defaultPriority);
    state.active = false;
    state.drawingSettings = new DrawableSettings();

    state.drawingSettings->Load(state.settings, defaultSettings);

    // Insert the state based on its priority.
    list<State>::iterator iter;
    for (iter = this->states.begin(); iter != this->states.end() && iter->priority > state.priority; iter++);
    return this->states.insert(iter, state);
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
    reinterpret_cast<ID2D1SolidColorBrush*>(this->backBrush)->SetColor(Color::ARGBToD2D(drawSettings->color));
    reinterpret_cast<ID2D1SolidColorBrush*>(this->textBrush)->SetColor(Color::ARGBToD2D(drawSettings->fontColor));

    //
    ReCreateTextFormat(drawSettings);

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
    // Forward mouse messages to the lowest level child window which the mouse is over.
    if (msg >= WM_MOUSEFIRST && msg <= WM_MOUSELAST) {
        int xPos = GET_X_LPARAM(lParam); 
        int yPos = GET_Y_LPARAM(lParam);
        MessageHandler* handler = NULL;

        for (list<DrawableWindow*>::const_iterator iter = this->children.begin(); iter != this->children.end(); ++iter) {
            D2D1_RECT_F* pos = &(*iter)->drawingArea;
            if (xPos >= pos->left && xPos <= pos->right && yPos >= pos->top && yPos <= pos->bottom) {
                handler = *iter;
                break;
            }
        }

        if (msg == WM_MOUSEMOVE) {
            if (!this->parent && !isTrackingMouse) {
                isTrackingMouse = true;
                TrackMouseEvent(&this->trackMouseStruct);
            }
            if (handler != activeChild) {
                if (activeChild != NULL) {
                    activeChild->HandleMessage(window, WM_MOUSELEAVE, NULL, NULL);
                }
                activeChild = (DrawableWindow*)handler;
            }
        }

        if (handler == NULL) {
            handler = this->msgHandler;
        }

        // Let our messagehandler deal with it.
        return handler->HandleMessage(window, msg, wParam, lParam);
    }

    // Handle DrawableWindow messages.
    switch (msg) {
    case WM_MOUSELEAVE:
        {
            isTrackingMouse = false;
            if (activeChild != NULL) {
                activeChild->HandleMessage(window, WM_MOUSELEAVE, NULL, NULL);
                activeChild = NULL;
            }
        }
        break;

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
        {
            if (wParam == this->updateTextTimer) {
                this->UpdateText();
            }
            else {
                map<UINT_PTR, MessageHandler*>::const_iterator iter = timers.find(wParam);
                if (iter != timers.end()) {
                    return iter->second->HandleMessage(window, msg, wParam, lParam);
                }
            }
        }
        return 0;

    case WM_DISPLAYCHANGE:
        this->monitorInfo->Update();
        return 0;
    }

    // Forward registered user messages.
    if (msg >= WM_USER) {
        map<UINT,MessageHandler*>::const_iterator handler = this->userMessages.find(msg);
        if (handler != this->userMessages.end()) {
            return handler->second->HandleMessage(window, msg, wParam, lParam);
        }
    }

    // Let the default messagehandler deal with anything else, if it is initialized.
    if (this->msgHandler->initialized) {
        return this->msgHandler->HandleMessage(window, msg, wParam, lParam);
    }
    else {
        return DefWindowProc(window, msg, wParam, lParam);
    }
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
    if (child == this->activeChild) {
        this->activeChild = NULL;
    }
}


/// <summary>
/// Forcibly updates the text.
/// </summary>
void DrawableWindow::UpdateText() {
    nCore::System::FormatText(this->drawingSettings->text, sizeof(this->text)/sizeof(WCHAR), this->text);
    Repaint();
}
