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
Drawable::Drawable(LPCSTR prefix) {
    this->initialized = false;
    this->settings = new Settings(prefix);
    
    char parentPrefix[64];
    this->settings->GetString("Parent", parentPrefix, sizeof(parentPrefix), "");
    DrawableWindow* parentWindow = (DrawableWindow*)nCore::System::FindRegisteredWindow(parentPrefix);
    if (parentWindow) {
        this->window = parentWindow->CreateChild(this->settings, this);
    }
    else {
        this->window = gLSModule.CreateDrawableWindow(this->settings, this);
    }

    this->eventHandler = new EventHandler(this->settings);
    this->parent = nullptr;
    this->initialized = true;
}


/// <summary>
/// Creates a new top-level drawable, derived from the specified settings.
/// </summary>
Drawable::Drawable(LPCSTR prefix, Settings* parentSettings) {
    this->initialized = false;
    this->settings = parentSettings->CreateChild(prefix);
    this->window = gLSModule.CreateDrawableWindow(this->settings, this);
    this->eventHandler = new EventHandler(this->settings);
    this->parent = nullptr;
    this->initialized = true;
}


/// <summary>
/// Creates a new child drawable.
/// </summary>
Drawable::Drawable(Drawable* parent, LPCSTR prefix, bool independent) {
    this->initialized = false;
    this->settings = independent ? new Settings(prefix) : parent->settings->CreateChild(prefix);
    this->window = parent->window->CreateChild(this->settings, this);
    this->eventHandler = new EventHandler(this->settings);
    this->parent = parent;
    this->initialized = true;
}


/// <summary>
/// Lets go of all drawable member variables.
/// </summary>
Drawable::~Drawable() {
    this->initialized = false;
    SAFEDELETE(this->window);
    SAFEDELETE(this->settings);
    SAFEDELETE(this->eventHandler);
}


/// <summary>
/// Retrives this drawables window.
/// </summary>
DrawableWindow *Drawable::GetWindow() {
    return this->window;
}


/// <summary>
/// Retrives this drawables event handler.
/// </summary>
EventHandler* Drawable::GetEventHandler() {
    return this->eventHandler;
}
