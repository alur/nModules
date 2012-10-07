/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Drawable.cpp
 *  The nModules Project
 *
 *  An object which has a drawable window.
 *   
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../headers/lsapi.h"
#include "Drawable.hpp"
#include "LSModule.hpp"


extern LSModule* g_LSModule;


/// <summary>
/// Creates a new top-level drawable.
/// </summary>
Drawable::Drawable(LPCSTR prefix) {
    this->settings = new Settings(prefix);
    this->window = g_LSModule->CreateDrawableWindow(this->settings, this);
}


/// <summary>
/// Creates a new child drawable.
/// </summary>
Drawable::Drawable(Drawable* parent, LPCSTR prefix) {
    this->settings = parent->settings->CreateChild(prefix);
    this->window = parent->window->CreateChild(this->settings, this);
}


/// <summary>
/// Lets go of all drawable member variables.
/// </summary>
Drawable::~Drawable() {
    SAFEDELETE(this->window);
    SAFEDELETE(this->settings);
}
