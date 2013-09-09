/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  IDrawable.hpp
 *  The nModules Project
 *
 *  Deals with all basic LiteStep module functionality.
 *   
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "Settings.hpp"
#include "Window.hpp"
#include "MessageHandler.hpp"
#include "EventHandler.hpp"

class IDrawable : public MessageHandler
{
public:
    explicit IDrawable(LPCTSTR prefix);
    explicit IDrawable(IDrawable *parent, LPCTSTR prefix, bool independent = false);
    explicit IDrawable(LPCTSTR prefix, LPSettings parentSettings);

private:
    explicit IDrawable(const IDrawable &) /* = deleted */;
    IDrawable & operator = (const IDrawable &) /* = deleted */;

protected:
    ~IDrawable();

public:
    Window *GetWindow();
    //ClassWindow *GetClass();
    EventHandler *GetEventHandler();

protected:
    // Used for reading litestep settings.
    LPSettings mSettings;

    // The object which does actual drawing.
    Window *mWindow;

    // The parent drawable, if there is one.
    IDrawable *mParent;

    // Handles events.
    EventHandler *mEventHandler;
};
