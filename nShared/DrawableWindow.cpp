/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  DrawableWindow.cpp
 *  The nModules Project
 *
 *  A generic drawable window.
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
    this->monitorInfo = new MonitorInfo();
    this->parent = NULL;
    this->timerIDs = new UIDGenerator<UINT_PTR>(1);
    this->userMsgIDs = new UIDGenerator<UINT>(WM_USER);

    ConstructorCommon(settings, msgHandler);

    this->window = MessageHandler::CreateMessageWindowEx(WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW | WS_EX_COMPOSITED,
        windowClass, settings->prefix, WS_POPUP, 0, 0, 0, 0, NULL, NULL, instance, this);
    SetWindowPos(this->window, 0, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
    SetWindowLongPtr(this->window, GWLP_USERDATA, MAGIC_DWORD);

    // Configure the mouse tracking struct
    ZeroMemory(&this->trackMouseStruct, sizeof(TRACKMOUSEEVENT));
    this->trackMouseStruct.cbSize = sizeof(TRACKMOUSEEVENT);
    this->trackMouseStruct.hwndTrack = this->window;
    this->trackMouseStruct.dwFlags = TME_LEAVE;
    this->trackMouseStruct.dwHoverTime = 200;

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
    this->monitorInfo = parent->monitorInfo;
    this->parent = parent;
    this->timerIDs = NULL;
    this->userMsgIDs = NULL;
    this->updateTextTimer = NULL;
    this->window = parent->window;

    ConstructorCommon(settings, msgHandler);
}


/// <summary>
/// Called by the constructors, intializes variables.
/// </summary>
void DrawableWindow::ConstructorCommon(Settings* settings, MessageHandler* msgHandler) {
    this->activeChild = NULL;
    ZeroMemory(&this->drawingArea, sizeof(this->drawingArea));
    this->initialized = false;
    this->isTrackingMouse = false;
    this->msgHandler = msgHandler;
    this->renderTarget = NULL;
    this->text[0] = '\0';
    this->visible = false;

    // Create the base state
    State state = {0};
    state.active = true;
    state.settings = new Settings(settings);
    state.drawingSettings = new DrawableSettings();
    this->activeState = this->baseState = this->states.insert(this->states.begin(), state);
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

        SAFERELEASE(iter->backBrush);
        SAFERELEASE(iter->textBrush);
        SAFERELEASE(iter->textFormat);
    }
    this->states.clear();

    // Delete all overlays
    ClearOverlays();

    if (!this->parent) {
        SAFERELEASE(this->renderTarget);
    }

    if (!this->parent) {
        SAFEDELETE(this->monitorInfo);
    }
}


/// <summary>
/// Adds an overlay icon.
/// </summary>
HRESULT DrawableWindow::AddOverlay(D2D1_RECT_F position, HICON icon) {
    IWICBitmap* source = NULL;
    IWICImagingFactory* factory = NULL;

    HRESULT hr = S_OK;
    
    //
    CHECKHR(hr, Factories::GetWICFactory(reinterpret_cast<LPVOID*>(&factory)));

    // Generate a WIC bitmap and call the overloaded AddOverlay function
    CHECKHR(hr, factory->CreateBitmapFromHICON(icon, &source));
    CHECKHR(hr, AddOverlay(position, source));

    // Transfer control here if CHECKHR failed
    CHECKHR_END();

    //
    SAFERELEASE(source);

    return hr;
}


/// <summary>
/// Adds an overlay image.
/// </summary>
HRESULT DrawableWindow::AddOverlay(D2D1_RECT_F position, HBITMAP bitmap) {
    IWICBitmap* source = NULL;
    IWICImagingFactory* factory = NULL;

    HRESULT hr = S_OK;
    
    //
    CHECKHR(hr, Factories::GetWICFactory(reinterpret_cast<LPVOID*>(&factory)));

    // Generate a WIC bitmap and call the overloaded AddOverlay function
    CHECKHR(hr, factory->CreateBitmapFromHBITMAP(bitmap, NULL, WICBitmapUseAlpha, &source));
    CHECKHR(hr, AddOverlay(position, source));

    // Transfer control here if CHECKHR failed
    CHECKHR_END();

    //
    SAFERELEASE(source);

    return hr;
}


/// <summary>
/// Adds an overlay icon.
/// </summary>
HRESULT DrawableWindow::AddOverlay(D2D1_RECT_F position, IWICBitmap* source) {
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
    // Load the base state's settings.
    this->baseState->defaultSettings = defaultSettings;
    this->baseState->drawingSettings->Load(this->baseState->settings, this->baseState->defaultSettings);

    // Create D2D resources
    ReCreateDeviceResources();

    // Put the window in its correct position.
    SetPosition(this->baseState->drawingSettings->x, this->baseState->drawingSettings->y,
        this->baseState->drawingSettings->width, this->baseState->drawingSettings->height);

    // AlwaysOnTop... TODO::Fix this!
    if (!this->parent && this->baseState->drawingSettings->alwaysOnTop) {
        SetParent(this->window, NULL);
        SetWindowPos(this->window, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);
    }

    // Create the text format
    CreateTextFormat(this->baseState->drawingSettings, &this->baseState->textFormat);
    CreateBrushes(&(*this->baseState));
    
    // Set the text
    SetText(this->baseState->drawingSettings->text);

    this->initialized = true;
}


/// <summary>
/// Creates the brushes for the specified state.
/// </summary>
HRESULT DrawableWindow::CreateBrushes(State* state) {
    IWICImagingFactory* factory = NULL;
    Factories::GetWICFactory(reinterpret_cast<LPVOID*>(&factory));

    // Create the background brush
    if (state->drawingSettings->image[0] != 0) {
        
        //factory->
    }

    this->renderTarget->CreateSolidColorBrush(Color::ARGBToD2D(state->drawingSettings->color), (ID2D1SolidColorBrush**)&state->backBrush);


    this->renderTarget->CreateSolidColorBrush(Color::ARGBToD2D(state->drawingSettings->fontColor), (ID2D1SolidColorBrush**)&state->textBrush);

    return S_OK;
}


/// <summary>
/// Creates a textFormat based on the specified drawingSettings.
/// </summary>
HRESULT DrawableWindow::CreateTextFormat(DrawableSettings* drawingSettings, IDWriteTextFormat** textFormat) {
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
        textFormat);

    // Set the horizontal text alignment
    if (_stricmp(drawingSettings->textAlign, "Center") == 0)
        (*textFormat)->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
    else if (_stricmp(drawingSettings->textAlign, "Right") == 0)
        (*textFormat)->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
    else
        (*textFormat)->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);

    // Set the vertical text alignment
    if (_stricmp(drawingSettings->textVerticalAlign, "Middle") == 0)
        (*textFormat)->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    else if (_stricmp(drawingSettings->textVerticalAlign, "Bottom") == 0)
        (*textFormat)->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_FAR);
    else
        (*textFormat)->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);

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

    (*textFormat)->SetTrimming(&trimmingOptions, NULL);

    // Set word wrapping
    (*textFormat)->SetWordWrapping(drawingSettings->wordWrap ? DWRITE_WORD_WRAPPING_WRAP : DWRITE_WORD_WRAPPING_NO_WRAP);

    // Set reading direction
    (*textFormat)->SetReadingDirection(drawingSettings->rightToLeft ? DWRITE_READING_DIRECTION_RIGHT_TO_LEFT : DWRITE_READING_DIRECTION_LEFT_TO_RIGHT);

    return S_OK;
}


DrawableSettings* DrawableWindow::GetDrawingSettings() {
    return this->baseState->drawingSettings;
}


void DrawableWindow::Move(int x, int y) {
    SetPosition(x, y, this->baseState->drawingSettings->width, this->baseState->drawingSettings->height);
}


void DrawableWindow::Resize(int width, int height) {
    SetPosition(this->baseState->drawingSettings->x, this->baseState->drawingSettings->y, width, height);
}


bool DrawableWindow::IsVisible() {
    return this->visible;
}


void DrawableWindow::SetPosition(int x, int y, int width, int height) {
    //
    this->baseState->drawingSettings->x = x;
    this->baseState->drawingSettings->y = y;
    this->baseState->drawingSettings->width = width;
    this->baseState->drawingSettings->height = height;

    // Position the window and/or set the backarea.
    if (!this->parent) {
        SetWindowPos(this->window, 0, x, y, width, height, SWP_NOZORDER);
        this->drawingArea = D2D1::RectF(0, 0, (float)width, (float)height);
        D2D1_SIZE_U size = D2D1::SizeU(width, height);
        this->renderTarget->Resize(size);
    }
    else {
        this->drawingArea = D2D1::RectF(
            this->parent->drawingArea.left + x,
            this->parent->drawingArea.top + y,
            this->parent->drawingArea.left + x + width,
            this->parent->drawingArea.top + y + height
        );
    }

    // The text area is offset from the drawing area.
    for (STATE state = this->states.begin(); state != this->states.end(); ++state) {
        state->textArea = this->drawingArea;
        state->textArea.bottom -= state->drawingSettings->textOffsetBottom;
        state->textArea.top += state->drawingSettings->textOffsetTop;
        state->textArea.left += state->drawingSettings->textOffsetLeft;
        state->textArea.right -= state->drawingSettings->textOffsetRight;
    }

    // Update all overlays
    for (OVERLAY overlay = overlays.begin(); overlay != overlays.end(); ++overlay) {
        overlay->drawingPosition = overlay->position;
        overlay->drawingPosition.left += this->drawingArea.left;
        overlay->drawingPosition.right += this->drawingArea.left;
        overlay->drawingPosition.top += this->drawingArea.top;
        overlay->drawingPosition.bottom += this->drawingArea.top;

        // Move the origin of the brush to match the overlay position
        overlay->brush->SetTransform(Matrix3x2F::Identity());
        overlay->brush->SetTransform(Matrix3x2F::Translation(overlay->drawingPosition.left, overlay->drawingPosition.top));
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
            D2D1_SIZE_U size = D2D1::SizeU(this->baseState->drawingSettings->width, this->baseState->drawingSettings->height);
            pD2DFactory->CreateHwndRenderTarget(
                RenderTargetProperties(
                    D2D1_RENDER_TARGET_TYPE_DEFAULT,
                    PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)
                ),
                HwndRenderTargetProperties(this->window, size),
                &this->renderTarget
            );
        }
        else {
            this->renderTarget = this->parent->renderTarget;
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
    rect->right = r.left + (LONG)this->drawingArea.right;
    rect->bottom = r.top + (LONG)this->drawingArea.bottom;
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
/// Gets the "Desired" size of the window, given the specified constraints.
/// </summary>
void DrawableWindow::GetDesiredSize(int maxWidth, int maxHeight, LPSIZE size) {
    IDWriteFactory* factory = NULL;
    IDWriteTextLayout* textLayout = NULL;
    DWRITE_TEXT_METRICS metrics;

    Factories::GetDWriteFactory(reinterpret_cast<LPVOID*>(&factory));
    factory->CreateTextLayout(this->text, lstrlenW(this->text), this->baseState->textFormat, (float)maxWidth, (float)maxHeight, &textLayout);
    textLayout->GetMetrics(&metrics);
    SAFERELEASE(textLayout);

    size->cx = long(metrics.width + this->baseState->drawingSettings->textOffsetLeft + this->baseState->drawingSettings->textOffsetRight) + 1;
    size->cy = long(metrics.height + this->baseState->drawingSettings->textOffsetTop + this->baseState->drawingSettings->textOffsetBottom) + 1;
}


/// <summary>
/// Sizes the window to fit its current text.
/// </summary>
void DrawableWindow::SizeToText(int maxWidth, int maxHeight) {
    SIZE s;
    GetDesiredSize(maxWidth, maxHeight, &s);
    this->SetPosition(this->baseState->drawingSettings->x, this->baseState->drawingSettings->y, s.cx, s.cy);
}


/// <summary>
/// Adds a new state.
/// </summary>
DrawableWindow::STATE DrawableWindow::AddState(LPCSTR prefix, DrawableSettings* defaultSettings, int defaultPriority) {
    State state;
    state.defaultSettings = defaultSettings;
    state.settings = this->baseState->settings->CreateChild(prefix);
    state.settings->AppendGroup(this->baseState->settings);
    state.priority = state.settings->GetInt("Priority", defaultPriority);
    state.active = false;
    state.drawingSettings = new DrawableSettings();

    state.drawingSettings->Load(state.settings, defaultSettings);

    state.textArea = this->drawingArea;
    state.textArea.bottom -= state.drawingSettings->textOffsetBottom;
    state.textArea.top += state.drawingSettings->textOffsetTop;
    state.textArea.left += state.drawingSettings->textOffsetLeft;
    state.textArea.right -= state.drawingSettings->textOffsetRight;

    CreateTextFormat(state.drawingSettings, &state.textFormat);
    CreateBrushes(&state);

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
/// 
/// </summary>
void DrawableWindow::HandleActiveStateChange() {
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
    this->visible = false;
    if (!this->parent) {
        ShowWindow(this->window, SW_HIDE);
    }
}


void DrawableWindow::SetText(LPCWSTR text) {
    StringCchCopyW(this->baseState->drawingSettings->text, sizeof(this->baseState->drawingSettings->text)/sizeof(this->baseState->drawingSettings->text[0]), text);
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

        this->renderTarget->FillRectangle(this->drawingArea, this->activeState->backBrush);
    
        this->renderTarget->SetTransform(
            Matrix3x2F::Rotation(this->activeState->drawingSettings->textRotation,
            Point2F(this->activeState->drawingSettings->width/2.0f,this->activeState->drawingSettings->height/2.0f)));

        this->renderTarget->DrawText(this->text, lstrlenW(this->text), this->activeState->textFormat, 
            this->activeState->textArea, this->activeState->textBrush, D2D1_DRAW_TEXT_OPTIONS_CLIP);
        
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
    nCore::System::FormatText(this->baseState->drawingSettings->text, sizeof(this->text)/sizeof(WCHAR), this->text);
    Repaint();
}
