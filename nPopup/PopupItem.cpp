/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  PopupItem.hpp
 *  The nModules Project
 *
 *  Represents an item in a popup.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "PopupItem.hpp"
#include "../nShared/Macros.h"


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
