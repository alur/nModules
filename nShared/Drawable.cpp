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


extern LSModule* g_LSModule;


/// <summary>
/// Creates a new top-level drawable.
/// </summary>
Drawable::Drawable(LPCSTR prefix) {
    this->initialized = false;
    this->settings = new Settings(prefix);
    this->window = g_LSModule->CreateDrawableWindow(this->settings, this);
    this->initialized = true;
    this->parent = NULL;
}


/// <summary>
/// Creates a new top-level drawable, derived from the specified settings.
/// </summary>
Drawable::Drawable(LPCSTR prefix, Settings* parentSettings) {
    this->initialized = false;
    this->settings = parentSettings->CreateChild(prefix);
    this->window = g_LSModule->CreateDrawableWindow(this->settings, this);
    this->initialized = true;
    this->parent = NULL;
}


/// <summary>
/// Creates a new child drawable.
/// </summary>
Drawable::Drawable(Drawable* parent, LPCSTR prefix) {
    this->initialized = false;
    this->settings = parent->settings->CreateChild(prefix);
    this->window = parent->window->CreateChild(this->settings, this);
    this->initialized = true;
    this->parent = parent;
}


/// <summary>
/// Lets go of all drawable member variables.
/// </summary>
Drawable::~Drawable() {
    this->initialized = false;
    SAFEDELETE(this->window);
    SAFEDELETE(this->settings);
}
