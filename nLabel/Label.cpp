/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Label.cpp
 *  The nModules Project
 *
 *  Implementation of the Label class.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "Label.hpp"

#include "../nShared/LiteStep.h"
#include "../nShared/LSModule.hpp"

#include "../Utilities/StringUtils.h"

#include <unordered_map>


using std::pair;
using std::tstring;
using std::unordered_map;


extern StringKeyedMaps<tstring, Label*>::UnorderedMap gAllLabels;


// Default settings for labels.
static const WindowSettings gLabelDefaults([] (WindowSettings & defaults)
{
    defaults.evaluateText = true;
    defaults.registerWithCore = true;
});

// StateRender initialization information.
static const StateRender<Label::States>::InitData gInitData([] (StateRender<Label::States>::InitData & initData)
{
    initData[Label::States::Hover].prefix = _T("Hover");
});


/// <summary>
/// Constructor for top-level labels.
/// </summary>
/// <param name="name">The RC settings prefix of the label.</param>
Label::Label(LPCTSTR name)
    : Drawable(name)
{
    Initalize();
}


/// <summary>
/// Constructor for overlays.
/// </summary>
/// <param name="name">The RC settings prefix of the label.</param>
/// <param name="parent">The overlay label's parent.</param>
Label::Label(LPCTSTR name, Drawable * parent)
    : Drawable(parent, name, true)
{
    Initalize();
}


/// <summary>
/// Destructor
/// </summary>
Label::~Label()
{
    mOverlays.clear();
    gAllLabels.erase(mSettings->GetPrefix());
}


/// <summary>
/// Handles messages to the label window.
/// </summary>
/// <param name="window">The window the message is for.</param>
/// <param name="message">The type of message.</param>
/// <param name="wParam">wParam</param>
/// <param name="lParam">lParam</param>
/// <param name="extra">Extra data</param>
LRESULT WINAPI Label::HandleMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam, LPVOID extra)
{
    UNREFERENCED_PARAMETER(extra);

    if (message == WM_MOUSEMOVE)
    {
        mStateRender.ActivateState(States::Hover, mWindow);
    }
    else if (message == WM_MOUSELEAVE)
    {
        mStateRender.ClearState(States::Hover, mWindow);
    }

    mEventHandler->HandleMessage(window, message, wParam, lParam);
    return DefWindowProc(window, message, wParam, lParam);
}


/// <summary>
/// Initializes the label.
/// </summary>
void Label::Initalize()
{
    gAllLabels.emplace(mSettings->GetPrefix(), this);
    
    mStateRender.Load(gInitData, mSettings);

    WindowSettings windowSettings;
    windowSettings.Load(mSettings, &gLabelDefaults);
    mWindow->Initialize(windowSettings, &mStateRender);
    
    LiteStep::IterateOverCommandLineTokens(mSettings->GetPrefix(), TEXT("OverlayLabel"), [this] (LPCTSTR name) -> void
    {
        mOverlays.emplace_front(name, this);
    });

    if (!mWindow->GetDrawingSettings()->hidden)
    {
        mWindow->Show();
    }
}
