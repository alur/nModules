/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  WindowSettings.hpp
 *  The nModules Project
 *
 *  Settings used by Window.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include <d2d1.h>
#include <dwrite.h>
#include "Settings.hpp"

class WindowSettings
{
public:
    WindowSettings();
    WindowSettings(void init(WindowSettings &));

public:
    // Loads the actual settings.
    void Load(const Settings *settings, const WindowSettings *defaults = nullptr);

    D2D1_TEXT_ANTIALIAS_MODE ParseAntiAliasMode(LPCTSTR mode);

    // True if the window should be topmost. Default: false
    bool alwaysOnTop;

    // True if the window background should be blured. Default: false
    bool blurBehind;

    // True in order to let the
    bool clickThrough;

    // True to evaluate text strings. Not read from RC files. Default: false
    bool evaluateText;

    // The height of the window. Default: 100
    RelatedNumber height;

    // True if the window should start hidden. Default: false
    bool hidden;

    // True to register this drawable with the core. Not read from RC files. Default: false
    bool registerWithCore;

    // The text to display. Default: "" (blank)
    TCHAR text[MAX_LINE_LENGTH];

    // How to anti-alias the text.
    D2D1_TEXT_ANTIALIAS_MODE textAntiAliasMode;

    // The width of the window. Default: 100
    RelatedNumber width;

    // The x position of the window, relative to the parent. Default: 0
    RelatedNumber x;

    // The y position of the window, relative to the parent. Default: 0
    RelatedNumber y;
};
