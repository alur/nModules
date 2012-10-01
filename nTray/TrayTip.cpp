/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  TrayTip.cpp
 *  The nModules Project
 *
 *  
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../headers/lsapi.h"
#include "TrayTip.hpp"


TrayTip::TrayTip(LPCSTR prefix) {

}


TrayTip::~TrayTip() {

}


void TrayTip::Show() {

}


void TrayTip::Hide() {

}


void TrayTip::LoadSettings(bool /*bIsRefresh*/) {

}


LRESULT WINAPI TrayTip::HandleMessage(HWND wnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    return m_pWindow->HandleMessage(wnd, uMsg, wParam, lParam);
}


HWND TrayTip::GetHWND() {
    return m_pWindow->GetWindow();
}
