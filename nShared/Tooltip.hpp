/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Tooltip.hpp
 *  The nModules Project
 *
 *  A general tooltip window.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "Drawable.hpp"
#include "StateRender.hpp"
#include "Window.hpp"

class Tooltip : public Drawable
{
public:
    enum class States
    {
        Base = 0,
        Count
    };

public:
    explicit Tooltip(LPCTSTR prefix, Settings* parentSettings);
    virtual ~Tooltip();

public:
    void Show(LPCWSTR text, LPRECT position);
    void Hide();

private:
    void LoadSettings();

    // MessageHandler
public:
    LRESULT WINAPI HandleMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam, LPVOID);

private:
    int mMaxWidth;
    int mMaxHeight;

    StateRender<States> mStateRender;
};
