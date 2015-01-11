/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  ContainerItem.cpp
 *  The nModules Project
 *
 *  Represents a popup item which can hold child windows.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../nShared/LiteStep.h"
#include <strsafe.h>
#include "ContainerItem.hpp"
#include "../nShared/LSModule.hpp"
#include "Popup.hpp"
#include "../Utilities/Math.h"
#include <algorithm>


using std::min;


ContainerItem::ContainerItem(Drawable* parent, LPCTSTR prefix)
    : PopupItem(parent, prefix, PopupItem::Type::Container, true)
{
    WindowSettings defaults, windowSettings;
    defaults.registerWithCore = true;
    windowSettings.Load(mSettings, &defaults);

    StateRender<State>::InitData initData;
    for (auto &stateInitData : initData)
    {
        stateInitData.defaults.brushSettings[::State::BrushType::Background].color = Color::Create(0xAAFFFFFF);
        stateInitData.defaults.brushSettings[::State::BrushType::Text].color = Color::Create(0xFF000000);
    }
    mStateRender.Load(initData, mSettings);

    mWindow->Initialize(windowSettings, &mStateRender);

    mWindow->Show();
}


ContainerItem::~ContainerItem()
{
}


int ContainerItem::GetDesiredWidth(int maxWidth)
{
    return min<int>(maxWidth, (int)mWindow->GetSize().width);
}


LRESULT ContainerItem::HandleMessage(HWND window, UINT msg, WPARAM wParam, LPARAM lParam, LPVOID)
{
    switch (msg)
    {
    case WM_MOUSEMOVE:
        {
            mStateRender.ActivateState(State::Hover, mWindow);
            ((Popup*)mParent)->CloseChild();
        }
        return 0;

    case WM_MOUSELEAVE:
        {
            mStateRender.ClearState(State::Hover, mWindow);
        }
        return 0;
    }
    return DefWindowProc(window, msg, wParam, lParam);
}
