/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  DrawableWindowDropTarget.cpp
 *  The nModules Project
 *
 *  
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "DrawableWindow.hpp"
#include "../nShared/Debugging.h"


HRESULT DrawableWindow::DragEnter(IDataObject *dataObj, DWORD keyState, POINTL point, DWORD *effect) {
    *effect = DROPEFFECT_COPY;
    return S_OK;
}


HRESULT DrawableWindow::DragOver(DWORD keyState, POINTL point, DWORD *effect) {
    *effect = DROPEFFECT_COPY;
    return S_OK;
}


HRESULT DrawableWindow::DragLeave() {
    return S_OK;
}


HRESULT DrawableWindow::Drop(IDataObject *dataObj, DWORD keyState, POINTL point, DWORD *effect) {
    TRACEW(L"ShellWindow: %p %p", GetShellWindow(), FindWindow("Progman", NULL));
    *effect = DROPEFFECT_COPY;
    return S_OK;
}
