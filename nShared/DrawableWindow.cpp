/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  DrawableWindow.cpp
 *  The nModules Project
 *
 *  A generic drawable window.
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "LiteStep.h"
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
/// Constructor used to create a DrawableWindow for a pre-existing window. Used by nDesk.
/// </summary>
/// <param name="drawable">Pointer to the drawable which should be updated.</param>
void DrawableWindow::TextChangeHandler(LPVOID drawable) {
    ((DrawableWindow*)drawable)->UpdateText();
}


/// <summary>
/// Constructor used to create a DrawableWindow for a pre-existing window. Used by nDesk.
/// </summary>
/// <param name="window">The window to draw to.</param>
/// <param name="prefix">The settings prefix to use.</param>
/// <param name="msgHandler">The default message handler for this window.</param>
DrawableWindow::DrawableWindow(HWND window, LPCSTR prefix, MessageHandler* msgHandler) {
    this->monitorInfo = new MonitorInfo();
    this->parent = NULL;
    this->timerIDs = new UIDGenerator<UINT_PTR>(1);
    this->userMsgIDs = new UIDGenerator<UINT>(WM_USER);
    this->window = window;

    Settings* settings = new Settings(prefix);
    ConstructorCommon(settings, msgHandler);
    SAFEDELETE(settings);
    this->initialized = true;
    this->visible = true;

    // Configure the mouse tracking struct
    ZeroMemory(&this->trackMouseStruct, sizeof(TRACKMOUSEEVENT));
    this->trackMouseStruct.cbSize = sizeof(TRACKMOUSEEVENT);
    this->trackMouseStruct.hwndTrack = NULL;
    this->trackMouseStruct.dwFlags = NULL;
    this->trackMouseStruct.dwHoverTime = 200;
}


/// <summary>
/// Constructor used by LSModule to create a top-level window.
/// </summary>
/// <param name="parent">The window to use as a parent for this top-level window.</param>
/// <param name="windowClass">The windowclass to use for the top-level window.</param>
/// <param name="instance">Used for creating the window.</param>
/// <param name="settings">The settings to use.</param>
/// <param name="msgHandler">The default message handler for this window.</param>
DrawableWindow::DrawableWindow(HWND /* parent */, LPCSTR windowClass, HINSTANCE instance, Settings* settings, MessageHandler* msgHandler) {
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
}


/// <summary>
/// Constructor used by CreateChild to create a child window.
/// </summary>
/// <param name="parent">The parent of this window.</param>
/// <param name="settings">The settings to use.</param>
/// <param name="msgHandler">The default message handler for this window.</param>
DrawableWindow::DrawableWindow(DrawableWindow* parent, Settings* settings, MessageHandler* msgHandler) {
    this->monitorInfo = parent->monitorInfo;
    this->parent = parent;
    this->timerIDs = NULL;
    this->userMsgIDs = NULL;
    this->window = parent->window;

    ConstructorCommon(settings, msgHandler);
}


/// <summary>
/// Called by the constructors, initializes variables.
/// </summary>
/// <param name="settings">The settings to use.</param>
/// <param name="msgHandler">The default message handler for this window.</param>
void DrawableWindow::ConstructorCommon(Settings* settings, MessageHandler* msgHandler) {
    this->activeChild = NULL;
    this->animating = false;
    ZeroMemory(&this->drawingArea, sizeof(this->drawingArea));
    this->initialized = false;
    this->isTrackingMouse = false;
    this->msgHandler = msgHandler;
    this->parsedText = NULL;
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

    // Register with the core
    if (this->baseState->drawingSettings->registerWithCore) {
        nCore::System::UnRegisterWindow(this->baseState->settings->prefix);
    }

    if (!this->parent && this->window) {
        DestroyWindow(this->window);
    }

    SAFERELEASE(this->parsedText);

    DiscardDeviceResources();

    // Delete all states
    for (STATE iter = this->states.begin(); iter != this->states.end(); iter++) {
        SAFEDELETE(iter->defaultSettings);
        SAFEDELETE(iter->drawingSettings);
        SAFEDELETE(iter->settings);

        SAFERELEASE(iter->textFormat);
    }
    this->states.clear();

    // Delete all overlays
    ClearOverlays();

    if (!this->parent) {
        SAFERELEASE(this->renderTarget);
        SAFEDELETE(this->monitorInfo);
    }
}


/// <summary>
/// Adds an overlay icon.
/// </summary>
/// <param name="position">Where to place the overlay, relative to the parent.</param>
/// <param name="icon">The icon to use as an overlay.</param>
/// <returns>An object which can be used to modify/remove this overlay.</returns>
DrawableWindow::OVERLAY DrawableWindow::AddOverlay(D2D1_RECT_F position, HICON icon) {
    IWICBitmap* source = NULL;
    IWICImagingFactory* factory = NULL;

    Factories::GetWICFactory(reinterpret_cast<LPVOID*>(&factory));

    // Generate a WIC bitmap and call the overloaded AddOverlay function
    factory->CreateBitmapFromHICON(icon, &source);
    return AddOverlay(position, source);
}


/// <summary>
/// Adds an overlay image.
/// </summary>
/// <param name="position">Where to place the overlay, relative to the parent.</param>
/// <param name="bitmap">The bitmap to use as an overlay.</param>
/// <returns>An object which can be used to modify/remove this overlay.</returns>
DrawableWindow::OVERLAY DrawableWindow::AddOverlay(D2D1_RECT_F position, HBITMAP bitmap) {
    IWICBitmap* source = NULL;
    IWICImagingFactory* factory = NULL;
    
    Factories::GetWICFactory(reinterpret_cast<LPVOID*>(&factory));

    // Generate a WIC bitmap and call the overloaded AddOverlay function
    factory->CreateBitmapFromHBITMAP(bitmap, NULL, WICBitmapUseAlpha, &source);
    return AddOverlay(position, source);
}


/// <summary>
/// Adds an overlay image.
/// </summary>
/// <param name="position">Where to place the overlay, relative to the parent.</param>
/// <param name="source">The bitmap to use as an overlay.</param>
/// <returns>An object which can be used to modify/remove this overlay.</returns>
DrawableWindow::OVERLAY DrawableWindow::AddOverlay(D2D1_RECT_F position, IWICBitmapSource* source) {
    // Add the overlays to the overlay list{
    this->overlays.push_back(new Overlay(position, this->drawingArea.rect, source));
    OVERLAY overlayOut = this->overlays.end();
    (*--overlayOut)->ReCreateDeviceResources(this->renderTarget);

    return overlayOut;
}


/// <summary>
/// Adds a custom painter which is called after children and overlays.
/// </summary>
/// <param name="painter">The painter.</param>
/// <returns>An object which can be used to modify/remove this painter.</returns>
DrawableWindow::PAINTER DrawableWindow::AddPostPainter(IPainter* painter) {
    this->postPainters.push_back(painter);
    PAINTER ret = this->postPainters.end();
    (*--ret)->ReCreateDeviceResources(this->renderTarget);

    return ret;
}


/// <summary>
/// Adds a custom painter which is called before children and overlays.
/// </summary>
/// <param name="painter">The painter.</param>
/// <returns>An object which can be used to modify/remove this painter.</returns>
DrawableWindow::PAINTER DrawableWindow::AddPrePainter(IPainter* painter) {
    this->prePainters.push_back(painter);
    PAINTER ret = this->prePainters.end();
    (*--ret)->ReCreateDeviceResources(this->renderTarget);

    return ret;
}


/// <summary>
/// Adds a new state.
/// </summary>
/// <param name="prefix">The prefix for this state. This is appended to the prefix of this window.</param>
/// <param name="defaultSettings">The default settings for this state.</param>
/// <param name="defaultPriority">The default priority for this state. Higher priority states take precedence over lower priority states.</param>
/// <returns>An object which can be used to activate/clear this state.</returns>
DrawableWindow::STATE DrawableWindow::AddState(LPCSTR prefix, DrawableSettings* defaultSettings, int defaultPriority) {
    State state;
    state.defaultSettings = defaultSettings;
    state.settings = this->baseState->settings->CreateChild(prefix);
    state.settings->AppendGroup(this->baseState->settings);
    state.priority = state.settings->GetInt("Priority", defaultPriority);
    state.active = false;
    state.drawingSettings = new DrawableSettings();

    state.drawingSettings->Load(state.settings, defaultSettings);

    state.textArea = this->drawingArea.rect;
    state.textArea.bottom -= state.drawingSettings->textOffsetBottom;
    state.textArea.top += state.drawingSettings->textOffsetTop;
    state.textArea.left += state.drawingSettings->textOffsetLeft;
    state.textArea.right -= state.drawingSettings->textOffsetRight;

    CreateTextFormat(state.drawingSettings, &state.textFormat);
    CreateBrushes(&state);

    // Insert the state based on its priority.
    list<State>::iterator iter;
    for (iter = this->states.begin(); iter != this->states.end() && iter->priority > state.priority; ++iter);
    return this->states.insert(iter, state);
}


/// <summary>
/// Actives a certain state.
/// </summary>
/// <param name="state">The state to activate.</param>
void DrawableWindow::ActivateState(DrawableWindow::STATE state) {
    state->active = true;
    if (this->activeState == this->states.end() || this->activeState->priority < state->priority) {
        this->activeState = state;
        Repaint();
    }
}


/// <summary>
/// Performs an animation step.
/// </summary>
void DrawableWindow::Animate() {
    float progress = Easing::Transform(min(1.0f,
        float(GetTickCount() - this->animationStartTime)/(this->animationEndTime - this->animationStartTime)),
        this->animationEasing);

    if (progress >= 1.0f) {
        this->animating = false;
    }

    RECT step;
    step.left = this->animationStart.left + long(progress*(this->animationTarget.left - this->animationStart.left));
    step.top = this->animationStart.top + long(progress*(this->animationTarget.top - this->animationStart.top));
    step.right = this->animationStart.right + long(progress*(this->animationTarget.right - this->animationStart.right));
    step.bottom = this->animationStart.bottom + long(progress*(this->animationTarget.bottom - this->animationStart.bottom));

    SetPosition(step.left, step.top, step.right - step.left, step.bottom - step.top);

    Repaint();
}


/// <summary>
/// Clears the specified callback timer, stoping the timer and unregistering the timer ID from the handler that owns it.
/// </summary>
/// <param name="timer">The ID of the timer to clear.</param>
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


/// <summary>
/// Removes all overlays from the window.
/// </summary>
void DrawableWindow::ClearOverlays() {
    for (OVERLAY overlay = this->overlays.begin(); overlay != this->overlays.end(); ++overlay) {
        delete *overlay;
    }
    this->overlays.clear();
}


/// <summary>
/// Clears a certain state.
/// </summary>
/// <param name="state">The state to clear.</param>
void DrawableWindow::ClearState(DrawableWindow::STATE state) {
    state->active = false;
    if (state == this->activeState) {
        // We just cleared the active state, find the highest priority next active state.
        for (state++; state != this->states.end() && !state->active; state++);
        this->activeState = state;
        Repaint();
    }
}


/// <summary>
/// Creates the brushes for the specified state.
/// </summary>
/// <param name="state">The state to create brushes for.</param>
HRESULT DrawableWindow::CreateBrushes(State* state) {
    state->imageBrush = NULL;

    // Create the background brush
    if (state->drawingSettings->image[0] != 0) {
        IWICImagingFactory* factory = NULL;
        IWICBitmap* wicBitmap = NULL;
        IWICFormatConverter* converter = NULL;
        ID2D1BitmapBrush* brush = NULL;
        ID2D1Bitmap* bitmap = NULL;
        Factories::GetWICFactory(reinterpret_cast<LPVOID*>(&factory));
        
        HBITMAP hBitmap = LiteStep::LoadLSImage(state->drawingSettings->image, NULL);
        if (hBitmap) {
            HRESULT hr;

            hr = factory->CreateFormatConverter(&converter);
            if (SUCCEEDED(hr)) {
                hr = factory->CreateBitmapFromHBITMAP(hBitmap, NULL, WICBitmapUseAlpha, &wicBitmap);
            }
            if (SUCCEEDED(hr)) {
                hr = converter->Initialize(wicBitmap, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.f, WICBitmapPaletteTypeMedianCut);
            }
            if (SUCCEEDED(hr)) {
                hr = this->renderTarget->CreateBitmapFromWicBitmap(converter, NULL, &bitmap);
            }
            if (SUCCEEDED(hr)) {
                hr = this->renderTarget->CreateBitmapBrush(bitmap, &brush);
            }

            if (SUCCEEDED(hr)) {
                brush->SetTransform(Matrix3x2F::Translation(this->drawingArea.rect.left, this->drawingArea.rect.top));
                state->imageBrush = brush;
            }

            DeleteObject(hBitmap);
            SAFERELEASE(wicBitmap);
            SAFERELEASE(bitmap);
            SAFERELEASE(converter);
        }
    }

    this->renderTarget->CreateSolidColorBrush(Color::ARGBToD2D(state->drawingSettings->color), (ID2D1SolidColorBrush**)&state->backBrush);
    this->renderTarget->CreateSolidColorBrush(Color::ARGBToD2D(state->drawingSettings->fontColor), (ID2D1SolidColorBrush**)&state->textBrush);
    this->renderTarget->CreateSolidColorBrush(Color::ARGBToD2D(state->drawingSettings->outlineColor), (ID2D1SolidColorBrush**)&state->outlineBrush);

    return S_OK;
}


/// <summary>
/// Creates a child window.
/// </summary>
/// <param name="childSettings">The settings the child window should use.</param>
/// <param name="msgHandler">The default message handler for the child window.</param>
/// <returns>The child window.</returns>
DrawableWindow* DrawableWindow::CreateChild(Settings* childSettings, MessageHandler* msgHandler) {
    DrawableWindow* child = new DrawableWindow(this, childSettings, msgHandler);
    children.push_back(child);
    return child;
}


/// <summary>
/// Creates a textFormat based on the specified drawingSettings.
/// </summary>
/// <param name="drawingSettings">The settings to create the textformat with.</param>
/// <param name="textFormat">Out. The textformat.</param>
/// <returns>S_OK</returns>
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


/// <summary>
/// Discards all device dependent resources.
/// </summary>
void DrawableWindow::DiscardDeviceResources() {
    if (!this->parent) {
        SAFERELEASE(this->renderTarget);
    }
    else {
        this->renderTarget = NULL;
    }
    
    for (OVERLAY overlay = this->overlays.begin(); overlay != this->overlays.end(); ++overlay) {
        (*overlay)->DiscardDeviceResources();
    }

    for (list<State>::iterator state = this->states.begin(); state != this->states.end(); ++state) {
        SAFERELEASE(state->backBrush);
        SAFERELEASE(state->imageBrush);
        SAFERELEASE(state->outlineBrush);
        SAFERELEASE(state->textBrush);
    }
    
    for (PAINTER painter = this->prePainters.begin(); painter != this->prePainters.end(); ++painter) {
        (*painter)->DiscardDeviceResources();
    }
    
    for (PAINTER painter = this->postPainters.begin(); painter != this->postPainters.end(); ++painter) {
        (*painter)->DiscardDeviceResources();
    }

    // Discard resources for all children as well.
    for (list<DrawableWindow*>::const_iterator child = this->children.begin(); child != this->children.end(); ++child) {
        (*child)->DiscardDeviceResources();
    }
}


/// <summary>
/// Gets the "Desired" size of the window, given the specified constraints.
/// </summary>
/// <param name="maxWidth">Out. The maximum width to return.</param>
/// <param name="maxHeight">Out. The maximum height to return.</param>
/// <param name="size">Out. The desired size will be placed in this SIZE.</param>
void DrawableWindow::GetDesiredSize(int maxWidth, int maxHeight, LPSIZE size) {
    IDWriteFactory* factory = NULL;
    IDWriteTextLayout* textLayout = NULL;
    DWRITE_TEXT_METRICS metrics;
    maxWidth -= int(this->baseState->drawingSettings->textOffsetLeft + this->baseState->drawingSettings->textOffsetRight);
    maxHeight -= int(this->baseState->drawingSettings->textOffsetTop + this->baseState->drawingSettings->textOffsetBottom);

    Factories::GetDWriteFactory(reinterpret_cast<LPVOID*>(&factory));
    factory->CreateTextLayout(this->text, lstrlenW(this->text), this->baseState->textFormat, (float)maxWidth, (float)maxHeight, &textLayout);
    textLayout->GetMetrics(&metrics);
    SAFERELEASE(textLayout);

    size->cx = long(metrics.width + this->baseState->drawingSettings->textOffsetLeft + this->baseState->drawingSettings->textOffsetRight) + 1;
    size->cy = long(metrics.height + this->baseState->drawingSettings->textOffsetTop + this->baseState->drawingSettings->textOffsetBottom) + 1;
}


/// <summary>
/// Returns the drawing settings for the default state.
/// </summary>
/// <returns>The drawing settings for the default state.</returns>
DrawableSettings* DrawableWindow::GetDrawingSettings() {
    return this->baseState->drawingSettings;
}


/// <summary>
/// Returns an up-to-date MonitorInfo class.
/// </summary>
/// <returns>An up-to-date MonitorInfo class.</returns>
MonitorInfo* DrawableWindow::GetMonitorInformation() {
    return this->monitorInfo;
}


/// <summary>
/// Gets the screen position of the window.
/// </summary>
/// <param name="rect">Out. The screen position of this window will be placed in this rect.</param>
void DrawableWindow::GetScreenRect(LPRECT rect) {
    RECT r;
    GetWindowRect(this->window, &r);
    rect->left = r.left + (LONG)this->drawingArea.rect.left;
    rect->top = r.top + (LONG)this->drawingArea.rect.top;
    rect->right = r.left + (LONG)this->drawingArea.rect.right;
    rect->bottom = r.top + (LONG)this->drawingArea.rect.bottom;
}


/// <summary>
/// Returns the window handle of the top-level window this window belongs to.
/// </summary>
/// <returns>the window handle.</returns>
HWND DrawableWindow::GetWindowHandle() {
    return this->window;
}


/// <summary>
/// Handles window messages for this drawablewindow. Any messages forwarded from here will have the extra parameter set to this.
/// </summary>
/// <param name="window">The handle of the window this message was sent to.</param>
/// <param name="msg">The message.</param>
/// <param name="wParam">Message data.</param>
/// <param name="lParam">Message data</param>
/// <param name="extra">Message data. Not used.</param>
/// <returns>Something</returns>
LRESULT WINAPI DrawableWindow::HandleMessage(HWND window, UINT msg, WPARAM wParam, LPARAM lParam, LPVOID /* extra */) {
    // Forward mouse messages to the lowest level child window which the mouse is over.
    if (msg >= WM_MOUSEFIRST && msg <= WM_MOUSELAST) {
        int xPos = GET_X_LPARAM(lParam); 
        int yPos = GET_Y_LPARAM(lParam);
        MessageHandler* handler = NULL;

        for (list<DrawableWindow*>::const_iterator iter = this->children.begin(); iter != this->children.end(); ++iter) {
            D2D1_RECT_F* pos = &(*iter)->drawingArea.rect;
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
                    activeChild->HandleMessage(window, WM_MOUSELEAVE, NULL, NULL, this);
                }
                activeChild = (DrawableWindow*)handler;
            }
        }

        if (handler == NULL) {
            handler = this->msgHandler;
        }

        // Let our messagehandler deal with it.
        return handler->HandleMessage(window, msg, wParam, lParam, this);
    }

    // Handle DrawableWindow messages.
    switch (msg) {
    case WM_MOUSELEAVE:
        {
            isTrackingMouse = false;
            if (activeChild != NULL) {
                activeChild->HandleMessage(window, WM_MOUSELEAVE, NULL, NULL, this);
                activeChild = NULL;
            }
        }
        break;

    case WM_ERASEBKGND:
        return 1;

    case WM_PAINT:
        {
            if (SUCCEEDED(ReCreateDeviceResources())) {
                this->renderTarget->BeginDraw();
                this->renderTarget->Clear();
                Paint();

                // If EndDraw fails we need to recreate all device-dependent resources
                if (this->renderTarget->EndDraw() == D2DERR_RECREATE_TARGET) {
                    DiscardDeviceResources();
                }
            }

            ValidateRect(this->window, NULL);

            if (this->animating) {
                Animate();
            }
        }
        return 0;

    case WM_TIMER:
        {
            map<UINT_PTR, MessageHandler*>::const_iterator iter = timers.find(wParam);
            if (iter != timers.end()) {
                return iter->second->HandleMessage(window, msg, wParam, lParam, this);
            }
        }
        return 0;

    case WM_SETTINGCHANGE:
        {
            switch (wParam) {
            case SPI_SETWORKAREA:
                {
                    this->monitorInfo->Update();    
                }
                break;
            }
        }
        return 0;

    case WM_DISPLAYCHANGE:
        {
            this->monitorInfo->Update();
        }
        return 0;
    }

    // Forward registered user messages.
    if (msg >= WM_USER) {
        map<UINT,MessageHandler*>::const_iterator handler = this->userMessages.find(msg);
        if (handler != this->userMessages.end()) {
            return handler->second->HandleMessage(window, msg, wParam, lParam, this);
        }
    }

    // Let the default messagehandler deal with anything else, if it is initialized.
    if (this->msgHandler && this->msgHandler->initialized) {
        return this->msgHandler->HandleMessage(window, msg, wParam, lParam, this);
    }
    else {
        return DefWindowProc(window, msg, wParam, lParam);
    }
}


/// <summary>
/// Hides the window.
/// </summary>
void DrawableWindow::Hide() {
    this->visible = false;
    if (!this->parent) {
        ShowWindow(this->window, SW_HIDE);
    }
    else {
        this->parent->Repaint();
    }
}


/// <summary>
/// Initalizes this window.
/// </summary>
/// <param name="defaultSettings">The default settings for this window.</param>
void DrawableWindow::Initialize(DrawableSettings* defaultSettings) {
    // Load the base state's settings.
    this->baseState->defaultSettings = defaultSettings;
    this->baseState->drawingSettings->Load(this->baseState->settings, this->baseState->defaultSettings);

    // Register with the core.
    if (this->baseState->drawingSettings->registerWithCore) {
        nCore::System::RegisterWindow(this->baseState->settings->prefix, this);
    }

    // Put the window in its correct position.
    SetPosition(this->baseState->drawingSettings->x, this->baseState->drawingSettings->y,
        this->baseState->drawingSettings->width, this->baseState->drawingSettings->height);
    this->drawingArea.radiusX = this->baseState->drawingSettings->cornerRadiusX;
    this->drawingArea.radiusY = this->baseState->drawingSettings->cornerRadiusY;

    // Create D2D resources.
    ReCreateDeviceResources();

    // AlwaysOnTop... TODO::Fix this!
    if (!this->parent && this->baseState->drawingSettings->alwaysOnTop) {
        SetParent(this->window, NULL);
        SetWindowPos(this->window, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);
    }

    // Create the text format.
    CreateTextFormat(this->baseState->drawingSettings, &this->baseState->textFormat);
    
    // Set the text.
    SetText(this->baseState->drawingSettings->text);

    this->initialized = true;
}


/// <summary>
/// Check if this window is currently visible.
/// </summary>
/// <returns>True if this window and all its ancestors are visible.</returns>
bool DrawableWindow::IsVisible() {
    if (this->parent) {
        return this->visible && this->parent->IsVisible();
    }
    return this->visible;
}


/// <summary>
/// Moves the window.
/// </summary>
/// <param name="x">The x coordinate to move the window to. Relative to the parent.</param>
/// <param name="y">The y coordinate to move the window to. Relative to the parent.</param>
void DrawableWindow::Move(int x, int y) {
    SetPosition(x, y, this->baseState->drawingSettings->width, this->baseState->drawingSettings->height);
}


/// <summary>
/// Removes the specified child.
/// </summary>
void DrawableWindow::Paint() {
    if (this->visible) {
        this->renderTarget->SetTransform(Matrix3x2F::Identity());

        this->renderTarget->FillRoundedRectangle(this->drawingArea, this->activeState->backBrush);
        this->renderTarget->DrawRoundedRectangle(this->drawingArea, this->activeState->outlineBrush, this->activeState->drawingSettings->outlineWidth);

        if (this->activeState->imageBrush != NULL) {
            this->renderTarget->FillRoundedRectangle(this->drawingArea, this->activeState->imageBrush);
        }
    
        this->renderTarget->SetTransform(
            Matrix3x2F::Rotation(this->activeState->drawingSettings->textRotation,
            Point2F(this->activeState->drawingSettings->width/2.0f,this->activeState->drawingSettings->height/2.0f)));

        this->renderTarget->DrawText(this->text, lstrlenW(this->text), this->activeState->textFormat, 
            this->activeState->textArea, this->activeState->textBrush, D2D1_DRAW_TEXT_OPTIONS_CLIP);
        
        // Pre painters
        for (PAINTER painter = this->prePainters.begin(); painter != this->prePainters.end(); ++painter) {
            (*painter)->Paint(this->renderTarget);
        }

        // Paint all overlays
        PaintOverlays();

        // Paint all children
        PaintChildren();

        // Post painters
        for (PAINTER painter = this->postPainters.begin(); painter != this->postPainters.end(); ++painter) {
            (*painter)->Paint(this->renderTarget);
        }
    }
}


/// <summary>
/// Paints all child windows.
/// </summary>
void DrawableWindow::PaintChildren() {
    for (list<DrawableWindow*>::const_iterator child = this->children.begin(); child != this->children.end(); ++child) {
        (*child)->Paint();
    }
}


/// <summary>
/// Paints all overlays.
/// </summary>
void DrawableWindow::PaintOverlays() {
    for (OVERLAY overlay = this->overlays.begin(); overlay != this->overlays.end(); ++overlay) {
        (*overlay)->Paint(this->renderTarget);
    }
}


/// <summary>
/// Registers an user message (>= WM_USER) which will be forwarded to the specified handler.
/// </summary>
/// <param name="msgHandler">The handler which will receive the message.</param>
/// <returns>The assigned message ID.</returns>
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


/// <summary>
/// Releases a user message. It will no longer be forwarded to the specified handler if received.
/// </summary>
/// <param name="message">The ID of the message to release.</param>
void DrawableWindow::ReleaseUserMessage(UINT message) {
    if (!this->parent) {
        this->userMessages.erase(message);
        this->userMsgIDs->ReleaseID(message);
    }
    else {
        this->parent->ReleaseUserMessage(message);
    }
}


/// <summary>
/// Resize the window.
/// </summary>
/// <param name="width">The width to resize the window to.</param>
/// <param name="height">The height to resize the window to.</param>
void DrawableWindow::Resize(int width, int height) {
    SetPosition(this->baseState->drawingSettings->x, this->baseState->drawingSettings->y, width, height);
}


/// <summary>
/// (Re)Creates all device-dependent resources.
/// </summary>
/// <returns>S_OK if successful, an error code otherwise.</returns>
HRESULT DrawableWindow::ReCreateDeviceResources() {
    HRESULT hr = S_OK;

    if (!this->renderTarget) {
        if (!this->parent) {
            ID2D1Factory *pD2DFactory = NULL;
            hr = Factories::GetD2DFactory(reinterpret_cast<LPVOID*>(&pD2DFactory));

            // Create the render target
            if (SUCCEEDED(hr)) {
                D2D1_SIZE_U size = D2D1::SizeU(this->baseState->drawingSettings->width, this->baseState->drawingSettings->height);
                hr = pD2DFactory->CreateHwndRenderTarget(
                    RenderTargetProperties(
                        D2D1_RENDER_TARGET_TYPE_DEFAULT,
                        PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)
                    ),
                    HwndRenderTargetProperties(this->window, size),
                    &this->renderTarget
                );
            }
        }
        else {
            this->renderTarget = this->parent->renderTarget;
        }

        if (SUCCEEDED(hr)) {
            for (STATE state = this->states.begin(); state != this->states.end(); ++state) {
                CreateBrushes(&(*state));
            }

            for (OVERLAY overlay = this->overlays.begin(); overlay != this->overlays.end(); ++overlay) {
                (*overlay)->ReCreateDeviceResources(this->renderTarget);
            }
    
            for (PAINTER painter = this->prePainters.begin(); painter != this->prePainters.end(); ++painter) {
                (*painter)->ReCreateDeviceResources(this->renderTarget);
            }
    
            for (PAINTER painter = this->postPainters.begin(); painter != this->postPainters.end(); ++painter) {
                (*painter)->ReCreateDeviceResources(this->renderTarget);
            }

            // Recreate resources for all children as well.
            for (list<DrawableWindow*>::const_iterator child = this->children.begin(); child != this->children.end(); ++child) {
                (*child)->ReCreateDeviceResources();
            }
        }
    }

    return hr;
}


/// <summary>
/// Removes the specified child.
/// </summary>
/// <param name="child">The child to remove.</param>
void DrawableWindow::RemoveChild(DrawableWindow* child) {
    this->children.remove(child);
    if (child == this->activeChild) {
        this->activeChild = NULL;
    }
}


/// <summary>
/// Repaints the window.
/// </summary>
/// <param name="region">The area of the window to repaint. If NULL, the whole window is repainted.</param>
void DrawableWindow::Repaint(LPRECT region) {
    if (this->initialized && this->visible) {
        if (this->parent) {
            if (region) {
                this->parent->Repaint(region);
            }
            else {
                RECT r = { (int)drawingArea.rect.left, (int)drawingArea.rect.top, (int)drawingArea.rect.right, (int)drawingArea.rect.bottom };
                this->parent->Repaint(&r);
            }
        }
        else {
            InvalidateRect(this->window, region, TRUE);
            UpdateWindow(this->window);
        }
    }
}


/// <summary>
/// Starts a new animation, or updates the parameters of the current one.
/// </summary>
/// <param name="x">The x coordinate to animate to.</param>
/// <param name="y">The y coordinate to animate to.</param>
/// <param name="width">The width to animate to.</param>
/// <param name="height">The height to animate to.</param>
/// <param name="duration">The number of milliseconds to complete the animation in.</param>
/// <param name="easing">The easing to use.</param>
void DrawableWindow::SetAnimation(int x, int y, int width, int height, int duration, Easing::EasingType easing) {
    RECT target = { x, y, x + width, y + height };
    this->animationTarget = target;
    this->animationStart.top = this->baseState->drawingSettings->y;
    this->animationStart.left = this->baseState->drawingSettings->x;
    this->animationStart.bottom = this->baseState->drawingSettings->y + this->baseState->drawingSettings->height;
    this->animationStart.right = this->baseState->drawingSettings->x + this->baseState->drawingSettings->width;
    this->animationEasing = easing;

    // TODO::Not too fond of using GetTickCount.
    this->animationStartTime = GetTickCount();
    this->animationEndTime = this->animationStartTime + duration;

    this->animating = true;

    Repaint();
}


/// <summary>
/// Creates a new timer which is forwarded to the specified handler.
/// </summary>
/// <param name="elapse">The uElapse parameter of SetTimer.</param>
/// <param name="msgHandler">The handler WM_TIMER messags with this ID are sent to.</param>
/// <returns>The assigned timer ID.</returns>
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
/// Moves and resizes the window.
/// </summary>
/// <param name="x">The x coordinate to move the window to. Relative to the parent.</param>
/// <param name="y">The y coordinate to move the window to. Relative to the parent.</param>
/// <param name="width">The width to resize the window to.</param>
/// <param name="height">The height to resize the window to.</param>
void DrawableWindow::SetPosition(int x, int y, int width, int height) {
    // Update the drawing settings.
    this->baseState->drawingSettings->x = x;
    this->baseState->drawingSettings->y = y;
    this->baseState->drawingSettings->width = width;
    this->baseState->drawingSettings->height = height;

    // Position the window and/or set the backarea.
    if (!this->parent) {
        SetWindowPos(this->window, 0, x, y, width, height, SWP_NOZORDER);
        this->drawingArea.rect = D2D1::RectF(0, 0, (float)width, (float)height);
        if (this->renderTarget) {
            D2D1_SIZE_U size = D2D1::SizeU(width, height);
            this->renderTarget->Resize(size);
        }
    }
    else {
        this->drawingArea.rect = D2D1::RectF(
            this->parent->drawingArea.rect.left + x,
            this->parent->drawingArea.rect.top + y,
            this->parent->drawingArea.rect.left + x + width,
            this->parent->drawingArea.rect.top + y + height
        );
    }

    // The text area is offset from the drawing area.
    for (STATE state = this->states.begin(); state != this->states.end(); ++state) {
        state->textArea = this->drawingArea.rect;
        state->textArea.bottom -= state->drawingSettings->textOffsetBottom;
        state->textArea.top += state->drawingSettings->textOffsetTop;
        state->textArea.left += state->drawingSettings->textOffsetLeft;
        state->textArea.right -= state->drawingSettings->textOffsetRight;
    }

    // Update all overlays
    for (OVERLAY overlay = overlays.begin(); overlay != overlays.end(); ++overlay) {
        (*overlay)->UpdatePosition(this->drawingArea.rect);
    }

    // Update all children
    for (list<DrawableWindow*>::const_iterator child = this->children.begin(); child != this->children.end(); ++child) {
        (*child)->Move((*child)->baseState->drawingSettings->x, (*child)->baseState->drawingSettings->y);
    }
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
/// Sizes the window to fit its current text.
/// </summary>
/// <param name="maxWidth">The maximum width to size the window to.</param>
/// <param name="maxHeight">The maximum height to size the window to.</param>
/// <param name="minWidth">The minimum width to size the window to.</param>
/// <param name="minHeight">The minimum height to size the window to.</param>
void DrawableWindow::SizeToText(int maxWidth, int maxHeight, int minWidth, int minHeight) {
    SIZE s;
    GetDesiredSize(maxWidth, maxHeight, &s);
    s.cx = max(s.cx, minWidth);
    s.cy = max(s.cy, minHeight);
    this->SetPosition(this->baseState->drawingSettings->x, this->baseState->drawingSettings->y, s.cx, s.cy);
}


/// <summary>
/// Sets the text for this window.
/// </summary>
/// <param name="text">The text for this window.</param>
void DrawableWindow::SetText(LPCWSTR text) {
    if (this->baseState->drawingSettings->evaluateText) {
        SAFEDELETE(this->parsedText);
        this->parsedText = (IParsedText*)nCore::System::ParseText(text);
        this->parsedText->SetChangeHandler(TextChangeHandler, this);
        UpdateText();
    }
    else {
        StringCchCopyW(this->text, sizeof(this->text)/sizeof(this->text[0]), text);
    }
}


/// <summary>
/// Sets the text offsets for all states.
/// </summary>
/// <param name="left">The text offset from the left.</param>
/// <param name="top">The text offset from the top.</param>
/// <param name="right">The text offset from the right.</param>
/// <param name="bottom">The text offset from the bottom.</param>
void DrawableWindow::SetTextOffsets(float left, float top, float right, float bottom) {
    for (STATE state = this->states.begin(); state != this->states.end(); ++state) {
        SetTextOffsets(left, top, right, bottom, state);
    }
}


/// <summary>
/// Sets the text offsets for the specified state.
/// </summary>
/// <param name="left">The text offset from the left.</param>
/// <param name="top">The text offset from the top.</param>
/// <param name="right">The text offset from the right.</param>
/// <param name="bottom">The text offset from the bottom.</param>
/// <param name="state">The state to set the offsets for.</param>
void DrawableWindow::SetTextOffsets(float left, float top, float right, float bottom, STATE state) {
    state->drawingSettings->textOffsetBottom = bottom;
    state->drawingSettings->textOffsetLeft = left;
    state->drawingSettings->textOffsetRight = right;
    state->drawingSettings->textOffsetTop = top;

    state->textArea = this->drawingArea.rect;
    state->textArea.bottom -= state->drawingSettings->textOffsetBottom;
    state->textArea.top += state->drawingSettings->textOffsetTop;
    state->textArea.left += state->drawingSettings->textOffsetLeft;
    state->textArea.right -= state->drawingSettings->textOffsetRight;
}


/// <summary>
/// Toggles the specified state.
/// </summary>
/// <param name="state">The state to toggle</param>
void DrawableWindow::ToggleState(STATE state) {
    if (state->active) {
        ClearState(state);
    }
    else {
        ActivateState(state);
    }
}


/// <summary>
/// Forcibly updates the text.
/// </summary>
void DrawableWindow::UpdateText() {
    if (this->baseState->drawingSettings->evaluateText) {
        this->parsedText->Evaluate(this->text, sizeof(this->text)/sizeof(this->text[0]));
    }
    else {
        StringCchCopyW(this->text, sizeof(this->text)/sizeof(this->text[0]), this->baseState->drawingSettings->text);
    }
    Repaint();
}
