/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  DrawableWindowUnknown.cpp
 *  The nModules Project
 *
 *  
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "DrawableWindow.hpp"


ULONG DrawableWindow::AddRef() {
    return 0;
}


HRESULT DrawableWindow::QueryInterface(REFIID /* riid */, void ** /* ppvObject */) {
    return E_NOINTERFACE;
}


ULONG DrawableWindow::Release() {
    return 0;
}
