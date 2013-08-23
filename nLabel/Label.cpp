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
    for (Drawable * label : mOverlays)
    {
        delete label;
    }

    gAllLabels.erase(mAllLabelsIter);
}


void Label::Initalize()
{
    mAllLabelsIter = gAllLabels.insert(gAllLabels.begin(), pair<wstring, Label*>(wstring(mSettings->GetPrefix()), this));
    
    WindowSettings defaults;
    defaults.evaluateText = true;
    defaults.registerWithCore = true;

    mWindow->Initialize(&defaults);

    LoadSettings();

    this->stateHover = mWindow->AddState(L"Hover", 100);
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
    if (uMsg == WM_MOUSEMOVE)
    {
        mStateRender.ActivateState(States::Hover);
    }
    else if (uMsg == WM_MOUSELEAVE)
    {
        mStateRender.ClearState(States::Hover);
    }

    mEventHandler->HandleMessage(hWnd, uMsg, wParam, lParam);
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
