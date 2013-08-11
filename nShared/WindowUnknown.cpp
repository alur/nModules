/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  WindowUnknown.cpp
 *  The nModules Project
 *
 *  
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "Window.hpp"


ULONG Window::AddRef()
{
    return 0;
}


HRESULT Window::QueryInterface(REFIID /* riid */, void ** /* ppvObject */)
{
    return E_NOINTERFACE;
}


ULONG Window::Release()
{
    return 0;
}
