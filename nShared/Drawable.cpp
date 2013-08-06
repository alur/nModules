/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Drawable.cpp
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
Drawable::Drawable(LPCTSTR prefix)
{
    mInitialized = false;
    mSettings = new Settings(prefix);
    
    TCHAR parentPrefix[MAX_RCCOMMAND];
    mSettings->GetString(_T("Parent"), parentPrefix, _countof(parentPrefix), _T(""));

    if (*parentPrefix != _T('\0'))
    {
        mWindow = new DrawableWindow(parentPrefix, mSettings, this);
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
/// </summary>
Drawable::Drawable(LPCTSTR prefix, Settings* parentSettings) {
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
Drawable::Drawable(Drawable* parent, LPCTSTR prefix, bool independent) {
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
Drawable::~Drawable() {
    mInitialized = false;
    SAFEDELETE(mWindow);
    SAFEDELETE(mSettings);
    SAFEDELETE(mEventHandler);
}


/// <summary>
/// Retrives this drawables window.
/// </summary>
DrawableWindow *Drawable::GetWindow() {
    return mWindow;
}


/// <summary>
/// Retrives this drawables event handler.
/// </summary>
EventHandler* Drawable::GetEventHandler() {
    return mEventHandler;
}
