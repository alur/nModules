/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  FolderItem.cpp
 *  The nModules Project
 *
 *  Represents a popup item which executes a command.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../headers/lsapi.h"
#include "FolderItem.hpp"


FolderItem::FolderItem(LPCSTR title, Popup* popup, LPCSTR customIcon) {
    this->popup = popup;
}


FolderItem::~FolderItem() {
    if (this->popup != NULL) {
        delete this->popup;
    }
}


LRESULT FolderItem::HandleMessage(HWND window, UINT msg, WPARAM wParam, LPARAM lParam) {
    return this->window->HandleMessage(window, msg, wParam, lParam);
}
