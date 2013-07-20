/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  DrawableSettings.hpp
 *  The nModules Project
 *
 *  Settings used by DrawableWindow.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include <d2d1.h>
#include <dwrite.h>
#include "Settings.hpp"

class DrawableSettings {
public:
    explicit DrawableSettings();
    virtual ~DrawableSettings();

    // Loads the actual settings.
    void Load(Settings* settings, DrawableSettings* defaults);

    // True if the window should be topmost. Default: false
    bool alwaysOnTop;

    // True if the window background should be blured. Default: false
    bool blurBehind;

    // True in order to let the
    bool clickThrough;

    // True to evaluate text strings. Not read from RC files. Default: false
    bool evaluateText;

    // The height of the window. Default: 100
    int height;

    // True if the window should start hidden. Default: false
    bool hidden;

    // True to register this drawable with the core. Ignores .RC settings. Default: False
    bool registerWithCore;

    // The text to display. Default: "" (blank)
    LPWSTR text;

    // The width of the window. Default: 100
    int width;

    // The x position of the window, relative to the parent. Default: 0
    int x;

    // The y position of the window, relative to the parent. Default: 0
    int y;
};
