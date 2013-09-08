/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Label.cpp
 *  The nModules Project
 *
 *  Implementation of the Label class.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../nShared/LiteStep.h"
#include "../nShared/LSModule.hpp"
#include "Label.hpp"

extern map<wstring, Label*> gAllLabels;


Label::Label(LPCTSTR name) : Drawable(name)
{
    Initalize();
}


Label::Label(LPCTSTR name, Drawable* parent) : Drawable(parent, name, true)
{
    Initalize();
}


Label::~Label()
{
    // Remove all overlays
    for (Label * label : mOverlays)
    {
        delete label;
    }

    gAllLabels.erase(mAllLabelsIter);
}


void Label::Initalize()
{
    mAllLabelsIter = gAllLabels.insert(gAllLabels.begin(), pair<wstring, Label*>(wstring(mSettings->GetPrefix()), this));
    
    StateRender<States>::InitData initData;
    initData[States::Hover].prefix = _T("Hover");
    mStateRender.Load(initData, mSettings);

    WindowSettings defaults;
    WindowSettings windowSettings;
    defaults.evaluateText = true;
    defaults.registerWithCore = true;
    windowSettings.Load(mSettings, &defaults);
    mWindow->Initialize(windowSettings, &mStateRender);

    LoadSettings();

    if (!mWindow->GetDrawingSettings()->hidden)
    {
        mWindow->Show();
    }
}


void Label::LoadSettings(bool /* isRefresh */)
{
    TCHAR prefix[MAX_RCCOMMAND];
    StringCchPrintf(prefix, _countof(prefix), _T("*%sOverlayLabel"), mSettings->GetPrefix());

    LiteStep::IterateOverLineTokens(prefix, [this] (LPCTSTR token) -> void
    {
        mOverlays.push_back(new Label(token, this));
    });
}


LRESULT WINAPI Label::HandleMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LPVOID)
{
    //DbgTraceWindowMessage("Label ", uMsg, wParam, lParam);
    if (uMsg == WM_MOUSEMOVE)
    {
        mStateRender.ActivateState(States::Hover, mWindow);
    }
    else if (uMsg == WM_MOUSELEAVE)
    {
        mStateRender.ClearState(States::Hover, mWindow);
    }

    mEventHandler->HandleMessage(hWnd, uMsg, wParam, lParam);
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
