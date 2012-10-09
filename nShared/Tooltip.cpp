/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Tooltip.cpp
 *  The nModules Project
 *
 *  
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../headers/lsapi.h"
#include "Tooltip.hpp"


Tooltip::Tooltip(LPCSTR prefix) : Drawable(prefix) {

}


Tooltip::~Tooltip() {
}


void Tooltip::Show(LPCWSTR text, int x, int y, int duration) {
    this->window->SetText(text);
    this->window->Show();
}


void Tooltip::Hide() {

}


void Tooltip::LoadSettings(bool /*bIsRefresh*/) {

}


LRESULT WINAPI Tooltip::HandleMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
    return DefWindowProc(window, message, wParam, lParam);
}
