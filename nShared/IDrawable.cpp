/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  IDrawable.cpp
 *  The nModules Project
 *
 *  An object which has a drawable window.
 *   
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../nShared/LiteStep.h"
#include "Drawable.hpp"
#include "LSModule.hpp"
#include "../nCoreCom/Core.h"


extern LSModule gLSModule;


/// <summary>
/// Creates a new top-level drawable.
/// </summary>
IDrawable::IDrawable(LPCTSTR prefix)
{
    mInitialized = false;
    mSettings = new Settings(prefix);
    //mWindowClass = new WindowClass(mSettings);
    
    TCHAR parentPrefix[MAX_RCCOMMAND];
    mSettings->GetString(_T("Parent"), parentPrefix, _countof(parentPrefix), _T(""));

    if (*parentPrefix != _T('\0'))
    {
        mWindow = new Window(parentPrefix, mSettings, this);
    }
    else
    {
        mWindow = gLSModule.CreateDrawableWindow(mSettings, this);
    }

    mEventHandler = new EventHandler(mSettings);
    mParent = nullptr;
    mInitialized = true;
}


/// <summary>
/// Creates a new top-level drawable, derived from the specified settings.
/// E.g. tooltips, balloons, ... These drawables can not have parents.
/// </summary>
IDrawable::IDrawable(LPCTSTR prefix, Settings* parentSettings)
{
    mInitialized = false;
    mSettings = parentSettings->CreateChild(prefix);
    mWindow = gLSModule.CreateDrawableWindow(mSettings, this);
    mEventHandler = new EventHandler(mSettings);
    mParent = nullptr;
    mInitialized = true;
}


/// <summary>
/// Creates a new child drawable.
/// </summary>
/// <param name="parent">The Drawable which this Drawable will be a child of.</param>
/// <param name="windowClass">The window class of this drawable.</param>
/// <param name="prefix">The prefix to append to the parents group.</param>
/// <param name="independent">If true, this Drawable's settings will not be a child of its parent's settings.</param>
IDrawable::IDrawable(IDrawable* parent, LPCTSTR prefix, bool independent)
{
    mInitialized = false;
    mSettings = independent ? new Settings(prefix) : parent->mSettings->CreateChild(prefix);
    mWindow = parent->mWindow->CreateChild(mSettings, this);
    mEventHandler = new EventHandler(mSettings);
    mParent = parent;
    mInitialized = true;
}


/// <summary>
/// Lets go of all drawable member variables.
/// </summary>
IDrawable::~IDrawable()
{
    mInitialized = false;
    SAFEDELETE(mWindow);
    SAFEDELETE(mSettings);
    SAFEDELETE(mEventHandler);
}


/// <summary>
/// Retrives this drawables window.
/// </summary>
Window *IDrawable::GetWindow()
{
    return mWindow;
}


/// <summary>
/// Retrives this drawables event handler.
/// </summary>
EventHandler* IDrawable::GetEventHandler()
{
    return mEventHandler;
}
