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

class Drawable : public MessageHandler {
public:
    explicit Drawable(LPCSTR prefix);
    explicit Drawable(Drawable* parent, LPCSTR prefix);
    virtual ~Drawable();

protected:
    // Used for reading litestep settings.
    Settings* settings;

    // The object which does actual drawing.
    DrawableWindow* window;
};
