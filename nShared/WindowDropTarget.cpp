/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  WindowDropTarget.cpp
 *  The nModules Project
 *
 *  
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "Window.hpp"


HRESULT Window::DragEnter(IDataObject *dataObj, DWORD keyState, POINTL point, DWORD *effect)
{
    *effect = DROPEFFECT_COPY;
    return S_OK;
}


HRESULT Window::DragOver(DWORD keyState, POINTL point, DWORD *effect)
{
    if (point.x > 500)
        *effect = DROPEFFECT_COPY;
    else
        *effect = DROPEFFECT_NONE;
    return S_OK;
}


HRESULT Window::DragLeave()
{
    return S_OK;
}


HRESULT Window::Drop(IDataObject *dataObj, DWORD keyState, POINTL point, DWORD *effect)
{
    *effect = DROPEFFECT_COPY;
    return S_OK;
}


void Window::AddDropRegion(LPRECT rect, IDropTarget *handler)
{
    RegisterDragDrop(GetWindowHandle(), (IDropTarget*)this);
}
