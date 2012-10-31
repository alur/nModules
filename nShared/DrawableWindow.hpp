/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  DrawableWindow.hpp
 *  The nModules Project
 *
 *  Essentially, anything that needs to draw.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "DrawableSettings.hpp"
#include "StateSettings.hpp"
#include "State.hpp"
#include "Overlay.hpp"
#include "MessageHandler.hpp"
#include <d2d1.h>
#include <vector>
#include <list>
#include <map>
#include "UIDGenerator.hpp"
#include "MonitorInfo.hpp"
#include "Easing.h"
#include "../nCore/IParsedText.hpp"
#include "IPainter.hpp"
#include "BrushSettings.hpp"


using std::vector;
using std::list;
using std::map;


class DrawableWindow : MessageHandler {
public:
    typedef list<State*>::iterator STATE;
    typedef list<Overlay*>::iterator OVERLAY;
    typedef list<IPainter*>::iterator PAINTER;

    // Constructor used for top-level windows.
    explicit DrawableWindow(HWND parent, LPCSTR windowClass, HINSTANCE instance, Settings* settings, MessageHandler* msgHandler);

    // Destructor
    virtual ~DrawableWindow();

    // Adds an overlay.
    OVERLAY AddOverlay(D2D1_RECT_F position, HBITMAP image);
    OVERLAY AddOverlay(D2D1_RECT_F position, HICON icon);
    OVERLAY AddOverlay(D2D1_RECT_F position, IWICBitmapSource* source);

    // Adds custom painters
    PAINTER AddPrePainter(IPainter* painter);
    PAINTER AddPostPainter(IPainter* painter);

    // Adds a new state.
    STATE AddState(LPCSTR prefix, int defaultPriority, StateSettings* defaultSettings = NULL);

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

    // Gets the "desired" size for a given width and height.
    void GetDesiredSize(int maxWidth, int maxHeight, LPSIZE size);

    // Returns the screen-coordinate position of this window.
    void GetScreenRect(LPRECT rect);

    // Returns the handle to the top-level window in this window stack.
    HWND GetWindowHandle();

    // Handles window messages.
    LRESULT WINAPI HandleMessage(HWND, UINT, WPARAM, LPARAM, LPVOID);

    // Hides this window.
    void Hide();

    // Initializes the DrawableWindow.
    void Initialize(DrawableSettings* defaultSettings = NULL, StateSettings* baseStateDefaults = NULL);

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

    // Performs an animation.
    void SetAnimation(int x, int y, int width, int height, int duration, Easing::EasingType easing);

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

    // Sets the text offsets for all states.
    void SetTextOffsets(float left, float top, float right, float bottom);

    // Shows this window.
    void Show();

    // Sizes the window to fit the text.
    void SizeToText(int maxWidth, int maxHeight, int minWidth = 0, int minHeight = 0);

    // Toggles the specified state.
    void ToggleState(STATE state);

    // Forcibly updates the text.
    void UpdateText();

protected:
    // Used by nDesk.
    DrawableWindow::DrawableWindow(HWND window, LPCSTR prefix, MessageHandler* msgHandler);

    // Paints this window.
    void Paint();

    // Paints all overlays.
    void PaintOverlays();

    // Paints all children.
    void PaintChildren();

    // The render target to draw to.
    ID2D1HwndRenderTarget* renderTarget;

    // Discards device-dependent stuff.
    void DiscardDeviceResources();

    // (Re)Creates D2D device-dependent stuff.
    HRESULT ReCreateDeviceResources();

private:
    // Called by IParsedText objects when we should update the text.
    static void TextChangeHandler(LPVOID drawable);

    //
    void Animate();

    // Called by the constructors, intializes variables.
    void ConstructorCommon(Settings* settings, MessageHandler* msgHandler);

    // Constructor used by CreateChild to create a child window.
    explicit DrawableWindow(DrawableWindow* parent, Settings* settings, MessageHandler* msgHandler);

    // Removes the specified child.
    void RemoveChild(DrawableWindow* child);

    // The child window the mouse is currently over.
    DrawableWindow* activeChild;

    // The currently active state, or states.end().
    STATE activeState;

    // True if we are currently animating.
    bool animating;

    // The easing we are using for the current animation.
    Easing::EasingType animationEasing;

    // The time when the animation should end.
    DWORD animationEndTime;

    // If we are currently doing an animation, the position at the start of the animation.
    RECT animationStart;

    // If we are currently doing an animation, how far along we are.
    DWORD animationStartTime;

    // If we are currently doing an animation, the position target of the animation.
    RECT animationTarget;

    // The base state -- the one to use when no others are active.
    STATE baseState;

    // The children of this drawablewindow.
    list<DrawableWindow*> children;

    // The area we draw in.
    D2D1_RECT_F drawingArea;

    // The drawing settings.
    DrawableSettings* drawingSettings;

    // If Initalize has been called.
    bool initialized;

    // If the mouse is above the top-level window.
    bool isTrackingMouse;

    // The object which should handle mouse event messages.
    MessageHandler* msgHandler;

    // MonitorInfo...
    MonitorInfo* monitorInfo;

    // All current overlays.
    list<Overlay*> overlays;

    // The DrawableWindow which is this windows parent.
    DrawableWindow* parent;

    // The current text of this window.
    IParsedText* parsedText;

    // Painters called after children and overlays are painted.
    list<IPainter*> postPainters;

    // Painters called before children and overlays are painted.
    list<IPainter*> prePainters;

    // Settings.
    Settings* settings;

    // All current states.
    list<State*> states;

    // Timer ID generator.
    UIDGenerator<UINT_PTR>* timerIDs;

    // Registered timer ID's.
    map<UINT_PTR, MessageHandler*> timers;

    // Used by the top-level window to track the mouse.
    TRACKMOUSEEVENT trackMouseStruct;

    // User msg ID generator.
    UIDGenerator<UINT>* userMsgIDs;

    // Registered user messages.
    map<UINT, MessageHandler*> userMessages;

    // Whether or not we are visible.
    bool visible;

    // The HWND we are rendering to.
    HWND window;

    // The text we are currently drawing
    LPCWSTR text;
};
