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

extern map<wstring, Label*> g_AllLabels;


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
    for (list<Drawable*>::const_iterator iter = this->overlays.begin(); iter != this->overlays.end(); iter++)
    {
        delete *iter;
    }
    this->overlays.clear();

    g_AllLabels.erase(this->allLabelsIter);
}


void Label::Initalize()
{
    this->allLabelsIter = g_AllLabels.insert(g_AllLabels.begin(), pair<wstring, Label*>(wstring(mSettings->GetPrefix()), this));
    
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
        this->overlays.push_back(new Label(token, this));
    });
}


LRESULT WINAPI Label::HandleMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LPVOID)
{
    if (uMsg == WM_MOUSEMOVE)
    {
        mWindow->ActivateState(this->stateHover);
    }
    else if (uMsg == WM_MOUSELEAVE)
    {
        mWindow->ClearState(this->stateHover);
    }
    mEventHandler->HandleMessage(hWnd, uMsg, wParam, lParam);
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
