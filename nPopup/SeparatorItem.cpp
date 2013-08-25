/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  SeparatorItem.cpp
 *  The nModules Project
 *
 *  Represents a separator.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../nShared/LiteStep.h"
#include <strsafe.h>
#include "SeparatorItem.hpp"
#include "../nShared/LSModule.hpp"
#include "Popup.hpp"


SeparatorItem::SeparatorItem(Drawable* parent)
    : PopupItem(parent, L"SeparatorItem", PopupItem::Type::Separator)
{
    mWindow->Initialize(((Popup*)mParent)->mPopupSettings.mSeparatorWindowSettings, &((Popup*)mParent)->mPopupSettings.mSeparatorStateRender);

    mWindow->Show();
}


SeparatorItem::~SeparatorItem() {
}


int SeparatorItem::GetDesiredWidth(int /* maxWidth */) {
    return 0;
}


LRESULT SeparatorItem::HandleMessage(HWND window, UINT msg, WPARAM wParam, LPARAM lParam, LPVOID) {
    return DefWindowProc(window, msg, wParam, lParam);
}
