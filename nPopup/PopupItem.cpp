/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  PopupItem.hpp
 *  The nModules Project
 *
 *  Represents an item in a popup.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "PopupItem.hpp"
#include "../nShared/Macros.h"


PopupItem::PopupItem() {
    this->settings = NULL;
    this->window = NULL;
}


void PopupItem::Position(int x, int y) {
    DrawableSettings* drawingSettings = this->window->GetSettings();
    drawingSettings->x = x;
    drawingSettings->y = y;

    this->window->UpdatePosition();
}


PopupItem::~PopupItem() {
}

int PopupItem::GetHeight() {
    return this->window->GetSettings()->height;
}