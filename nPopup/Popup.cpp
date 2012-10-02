/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Popup.cpp
 *  The nModules Project
 *
 *  Represents a popup box.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../headers/lsapi.h"
#include "Popup.hpp"


Popup::Popup(LPCSTR title, LPCSTR bang, LPCSTR prefix) {

}


Popup::~Popup() {

}


void Popup::AddItem(PopupItem* item) {
    delete item;
}
