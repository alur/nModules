/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  SeparatorItem.cpp
 *  The nModules Project
 *
 *  Represents a separator.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../headers/lsapi.h"
#include "SeparatorItem.hpp"


SeparatorItem::SeparatorItem() {

}


SeparatorItem::~SeparatorItem() {

}


LRESULT SeparatorItem::HandleMessage(HWND window, UINT msg, WPARAM wParam, LPARAM lParam) {
    return this->window->HandleMessage(window, msg, wParam, lParam);
}
