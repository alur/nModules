/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Window.cpp
 *  The nModules Project
 *
 *  A generic drawable window.
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "Window.hpp"


Window::UpdateLock::UpdateLock(Window *window)
    : mWindow(window)
    , mLocked(true)
{
    mWindow->PushUpdateLock(this);
}


Window::UpdateLock::~UpdateLock()
{
    Unlock();
}


void Window::UpdateLock::Unlock()
{
    if (mLocked)
    {
        mLocked = false;
        mWindow->PopUpdateLock(this);
    }
}
