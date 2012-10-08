/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  PopupItem.hpp
 *  The nModules Project
 *
 *  Represents an item in a popup.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "PopupItem.hpp"
#include "../nShared/Macros.h"
#include "../nShared/LSModule.hpp"

extern LSModule* g_LSModule;


PopupItem::PopupItem(Drawable* parent) : Drawable(parent, "Item") {
}


PopupItem::~PopupItem() {
}


void PopupItem::Position(int x, int y) {
    this->window->Move(x, y);
}


int PopupItem::GetHeight() {
    return this->window->GetDrawingSettings()->height;
}


bool PopupItem::ParseDotIcon(LPCSTR dotIcon) {
    // TODO::May cause problems with paths which include a comma.
    LPSTR index = (LPSTR)strrchr(dotIcon, ',');
    int nIndex;
    if (index != NULL) {
        index++[0] = NULL;
        nIndex = atoi(index);
    }
    else {
        nIndex = 0;
    }
    
    HICON icon = ExtractIcon(g_LSModule->GetInstance(), dotIcon, nIndex);

    if (icon == NULL) {
        return false;
    }

    D2D1_RECT_F f;
    f.bottom = 18; f.top = 2; f.left = 2; f.right = 18;
    this->window->AddOverlay(f, icon);
    DestroyIcon(icon);
    return true;
}
