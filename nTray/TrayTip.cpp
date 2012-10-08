/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  TrayTip.cpp
 *  The nModules Project
 *
 *  
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../headers/lsapi.h"
#include "TrayTip.hpp"


TrayTip::TrayTip(LPCSTR prefix) : Drawable(prefix) {

}


TrayTip::~TrayTip() {

}


void TrayTip::Show(LPCWSTR text) {
    this->window->SetText(text);
    this->window->Show();
}


void TrayTip::Hide() {

}


void TrayTip::LoadSettings(bool /*bIsRefresh*/) {

}


LRESULT WINAPI TrayTip::HandleMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
    return DefWindowProc(window, message, wParam, lParam);
}
