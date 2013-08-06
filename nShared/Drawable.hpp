/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Drawable.hpp
 *  The nModules Project
 *
 *  Deals with all basic LiteStep module functionality.
 *   
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "Settings.hpp"
#include "DrawableWindow.hpp"
#include "MessageHandler.hpp"
#include "EventHandler.hpp"

class Drawable : public MessageHandler
{
public:
    explicit Drawable(LPCTSTR prefix);
    explicit Drawable(Drawable *parent, LPCTSTR prefix, bool independent = false);
    explicit Drawable(LPCTSTR prefix, LPSettings parentSettings);
    virtual ~Drawable();

public:
    DrawableWindow *GetWindow();
    EventHandler *GetEventHandler();

protected:
    // Used for reading litestep settings.
    LPSettings mSettings;

    // The object which does actual drawing.
    DrawableWindow *mWindow;

    // The parent drawable, if there is one.
    Drawable *mParent;

    // Handles events.
    EventHandler *mEventHandler;
};
