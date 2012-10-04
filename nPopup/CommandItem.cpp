/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  CommandItem.cpp
 *  The nModules Project
 *
 *  Represents a popup item which executes a command.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../headers/lsapi.h"
#include "CommandItem.hpp"


CommandItem::CommandItem(LPCSTR title, LPCSTR command, LPCSTR customIcon) {

}


CommandItem::~CommandItem() {

}


LRESULT CommandItem::HandleMessage(HWND window, UINT msg, WPARAM wParam, LPARAM lParam) {
    return this->window->HandleMessage(window, msg, wParam, lParam);
}
