/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  DrawableWindow.hpp
 *  The nModules Project
 *
 *  Essentially, anything that needs to draw.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "DrawableSettings.hpp"
#include "MessageHandler.hpp"
#include <d2d1.h>
#include <vector>
#include <list>
#include <map>
#include "UIDGenerator.hpp"
#include "MonitorInfo.hpp"


using std::vector;
using std::list;
using std::map;


class DrawableWindow : MessageHandler {
public:
    // Defines a "State" which the window can be in.
    typedef struct {
        int priority;
        bool active;
        Settings* settings;
        DrawableSettings* drawingSettings;
        DrawableSettings* defaultSettings;

        // The brush we are currently painting the background with.
        ID2D1Brush* backBrush;

        // The brush we are currently painting the text with.
        ID2D1Brush* textBrush;

        // The area we draw text in
        D2D1_RECT_F textArea;

        // Defines how the text is formatted.
        IDWriteTextFormat* textFormat;
    } State;
    typedef list<State>::iterator STATE;

    // Defines an overlay. A bitmap, or icon, or something. To pounder::All these could be their own full-blown DrawableWindow.
    typedef struct {
        D2D1_RECT_F position;
        D2D1_RECT_F drawingPosition;
        ID2D1Brush* brush;
    } Overlay;
    typedef list<Overlay>::iterator OVERLAY;

    // Constructor used for top-level windows.
    explicit DrawableWindow(HWND parent, LPCSTR windowClass, HINSTANCE instance, Settings* settings, MessageHandler* msgHandler);

    // Destructor
    virtual ~DrawableWindow();

    // Adds an icon as an overlay.
    HRESULT AddOverlay(D2D1_RECT_F position, HICON icon);

    // Adds a new state.
    STATE AddState(LPCSTR prefix, DrawableSettings* defaultSettings, int defaultPriority);

    // Marks a particular state as active.
    void ActivateState(STATE state);

    // Stops a timer.
    void ClearCallbackTimer(UINT_PTR);

    // Removes all overlays.
    void ClearOverlays();

    // Clears the active flag of a particular state.
    void ClearState(STATE state);

    // Creates a new child window.
    DrawableWindow* CreateChild(Settings* childSettings, MessageHandler* msgHandler);

    // Returns the current drawing settings.
    DrawableSettings* GetDrawingSettings();

    // Returns a MonitorInfo class which will be kept up-to-date for the duration of this windows lifetime.
    MonitorInfo* GetMonitorInformation();

    // Returns the screen-coordinate position of this window.
    void GetScreenRect(LPRECT rect);

    // Returns the handle to the top-level window in this window stack.
    HWND GetWindow();

    // Handles window messages.
    LRESULT WINAPI HandleMessage(HWND, UINT, WPARAM, LPARAM);

    // Hides this window.
    void Hide();

    // Initializes the DrawableWindow.
    void Initialize(DrawableSettings* defaultSettings);

    // Returns whether or not this window is visible.
    bool IsVisible();

    // Moves this window.
    void Move(int x, int y);

    // Registers a user message
    UINT RegisterUserMessage(MessageHandler* msgHandler);

    // Unregisters a user message
    void ReleaseUserMessage(UINT message);

    // Forces this window to redraw.
    void Repaint(LPRECT region = NULL);

    // Resizes the window.
    void Resize(int width, int height);

    // Registers a timer
    UINT_PTR SetCallbackTimer(UINT elapse, MessageHandler* msgHandler);

    // Sets the message handler for this window.
    void SetMessageHandler(MessageHandler* msgHandler);

    // Sets the paragraph alignment of this drawablewindow.
    void SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT alignment);

    // Sets the position of this drawablewindow, relative to its parent.
    void SetPosition(int x, int y, int width, int height);

    // Sets the text of this window.
    void SetText(LPCWSTR text);

    // Sets the text alignment of this drawablewindow.
    void SetTextAlignment(DWRITE_TEXT_ALIGNMENT alignment);

    // Shows this window.
    void Show();

    // Sizes the window to fit the text.
    void SizeToText(int maxWidth, int maxHeight);

    // Forcibly updates the text.
    void UpdateText();

protected:
    // Paints this window.
    void Paint();

private:
    // Called by the constructors, intializes variables.
    void ConstructorCommon(Settings* settings, MessageHandler* msgHandler);

    // Creates an IDWriteTextFormat based on a DrawableSettings.
    HRESULT CreateTextFormat(DrawableSettings* drawingSettings, IDWriteTextFormat** textFormat);

    // Creates the brushes for a particular state.
    HRESULT CreateBrushes(State* state);

    // Constructor used by CreateChild to create a child window.
    explicit DrawableWindow(DrawableWindow* parent, Settings* settings, MessageHandler* msgHandler);

    // Should be called when the active state has changed.
    void HandleActiveStateChange();

    // (Re)Creates D2D device-dependent stuff.
    HRESULT ReCreateDeviceResources();

    // Removes the specified child.
    void RemoveChild(DrawableWindow* child);

    // The child window the mouse is currently over.
    DrawableWindow* activeChild;

    // The currently active state, or states.end().
    list<State>::iterator activeState;

    // The base state -- the one to use when no others are active.
    list<State>::iterator baseState;

    // The children of this drawablewindow.
    list<DrawableWindow*> children;

    // The area we draw in.
    D2D1_RECT_F drawingArea;

    // If Initalize has been called.
    bool initialized;

    // If the mouse is above the top-level window.
    bool isTrackingMouse;

    // The object which should handle mouse event messages.
    MessageHandler* msgHandler;

    // MonitorInfo...
    MonitorInfo* monitorInfo;

    // All current overlays.
    list<Overlay> overlays;

    // The DrawableWindow which is this windows parent.
    DrawableWindow* parent;

    // The render target to draw to
    ID2D1HwndRenderTarget* renderTarget;

    // All current states.
    list<State> states;

    // Timer ID generator.
    UIDGenerator<UINT_PTR>* timerIDs;

    // Registered timer ID's.
    map<UINT_PTR, MessageHandler*> timers;

    // Used by the top-level window to track the mouse.
    TRACKMOUSEEVENT trackMouseStruct;

    // Timer for re-evaulating the text.
    UINT_PTR updateTextTimer;

    // User msg ID generator.
    UIDGenerator<UINT>* userMsgIDs;

    // Registered user messages.
    map<UINT, MessageHandler*> userMessages;

    // Whether or not we are visible.
    bool visible;

    // The HWND we are rendering to.
    HWND window;

    // The text we are currently drawing
    WCHAR text[MAX_LINE_LENGTH];
};
