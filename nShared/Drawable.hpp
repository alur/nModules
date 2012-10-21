/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Drawable.hpp
 *  The nModules Project
 *
 *  Deals with all basic LiteStep module functionality.
 *   
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include <list>
#include "Settings.hpp"
#include "DrawableWindow.hpp"
#include "MessageHandler.hpp"
#include "EventHandler.hpp"

class Drawable : public MessageHandler {
public:
    explicit Drawable(LPCSTR prefix);
    explicit Drawable(Drawable* parent, LPCSTR prefix);
    explicit Drawable(LPCSTR prefix, Settings* parentSettings);
    virtual ~Drawable();

protected:
    // Used for reading litestep settings.
    Settings* settings;

    // The object which does actual drawing.
    DrawableWindow* window;

    // The parent drawable, if there is one.
    Drawable* parent;

    // Handles events.
    EventHandler* eventHandler;
};
