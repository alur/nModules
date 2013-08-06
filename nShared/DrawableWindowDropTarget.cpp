/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  DrawableWindowDropTarget.cpp
 *  The nModules Project
 *
 *  
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "DrawableWindow.hpp"


HRESULT DrawableWindow::DragEnter(IDataObject *dataObj, DWORD keyState, POINTL point, DWORD *effect) {
    *effect = DROPEFFECT_COPY;
    return S_OK;
}


HRESULT DrawableWindow::DragOver(DWORD keyState, POINTL point, DWORD *effect) {
    if (point.x > 500)
        *effect = DROPEFFECT_COPY;
    else
        *effect = DROPEFFECT_NONE;
    return S_OK;
}


HRESULT DrawableWindow::DragLeave() {
    return S_OK;
}


HRESULT DrawableWindow::Drop(IDataObject *dataObj, DWORD keyState, POINTL point, DWORD *effect) {
    *effect = DROPEFFECT_COPY;
    return S_OK;
}


void DrawableWindow::AddDropRegion(LPRECT rect, IDropTarget *handler) {
    RegisterDragDrop(GetWindowHandle(), (IDropTarget*)this);
}
