/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  DrawableWindow.hpp
 *  The nModules Project
 *
 *  Function declarations for the DrawableWindow class.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "DrawableSettings.hpp"
#include "IDrawableMessageHandler.hpp"
#include <d2d1.h>
#include <vector>
#include <list>

using std::vector;
using std::list;

class DrawableWindow {
public:
    static ATOM RegisterWindowClass(LPCSTR className, HINSTANCE DLLInstance);
    static LRESULT __stdcall MessageHandler(HWND window, UINT msg, WPARAM wParam, LPARAM lParam);

    // 
    typedef struct {
        DWORD state;
        int priority;
        bool active;
        Settings* settings;
        DrawableSettings* drawingSettings;
        DrawableSettings* defaultSettings;
    } State;

    typedef list<State>::iterator STATE;

    // Constructor
    explicit DrawableWindow(
        HWND parent,
        LPCSTR windowClass,
        HINSTANCE instance,
        Settings *settings,
        DrawableSettings* defaultSettings,
        IDrawableMessageHandler* messageHandler);

    // Destructor
    virtual ~DrawableWindow();

    // Returns the window handle.
    HWND GetWindow();

    // Handles window messages.
    LRESULT WINAPI HandleMessage(HWND, UINT, WPARAM, LPARAM);

    // Returns the 
    DrawableSettings* GetSettings();

    // Returns the settings for the specified state.
    DrawableSettings* GetSettings(DWORD state);

    // 
    void UpdatePosition();

    //
    STATE AddState(LPCSTR prefix, DrawableSettings* defaultSettings, int defaultPriority);

    //
    void ActivateState(STATE state);

    //
    void ClearState(STATE state);

    //
    HRESULT AddOverlay(D2D1_RECT_F, HICON);

    //
    void PurgeOverlays();

    //
    void UpdateOverlay(LPVOID, HICON);

    //
    void UpdateBrushes();

    //
    void Repaint();

    //
    void Show();

private:
    // All we need in order to paint some type of overlay
    typedef struct {
        D2D1_RECT_F position;
        ID2D1Brush* brush;
    } Overlay;

    // Creates the window
    bool CreateWnd(LPCSTR, HINSTANCE, IDrawableMessageHandler*);

    // Loads RC settings
    void LoadSettings();

    // 
    void HandleActiveStateChange();

    // The screen position of the window
    RECT scPosition;

    // All current overlays
    vector<Overlay> overlays;

    // All current states
    list<State> states;

    // The currently active state, or states.end()
    list<State>::iterator activeState;

    // The next state to be returned from AddState
    DWORD nextState;

    // The HWND we are rendering to
    HWND window;

    // The parent of this window
    HWND parent;

    // Settings for this window
    Settings* settings;

    // The current drawing settings we are using
    DrawableSettings* drawingSettings;

    // The default drawing settings
    DrawableSettings* defaultDrawingSettings;

    // 
    ID2D1HwndRenderTarget* renderTarget;

    // The brush we are currently painting the background with.
    ID2D1Brush* backBrush;

    // The brush we are currently painting the text with.
    ID2D1Brush* textBrush;

    // 
    IDWriteTextFormat* textFormat;

    // The text we are currently drawing
    LPCWSTR text;

    // 
    D2D1_RECT_F backArea;

    // 
    D2D1_RECT_F textArea;

    // 
    DWRITE_TEXT_ALIGNMENT textAlignment;

    // 
    DWRITE_PARAGRAPH_ALIGNMENT textVerticalAlignment;
};
