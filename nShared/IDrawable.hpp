/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  IDrawable.hpp
 *  The nModules Project
 *
 *  Deals with all basic LiteStep module functionality.
 *   
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "EventHandler.hpp"
#include "MessageHandler.hpp"
#include "Settings.hpp"
#include "Window.hpp"

class IDrawable : public MessageHandler
{
public:
    explicit IDrawable(LPCTSTR prefix);
    explicit IDrawable(IDrawable *parent, LPCTSTR prefix, bool independent = false);
    explicit IDrawable(LPCTSTR prefix, LPSettings parentSettings);

private:
    explicit IDrawable(const IDrawable &) = delete;
    IDrawable & operator = (const IDrawable &) = delete;

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
