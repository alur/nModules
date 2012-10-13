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

    // The background color of the window. Default: 0xFF000000 (black)
    ARGB color;

    // The default font to use. Default: Arial
    WCHAR font[MAX_PATH];

    // The default font color to use. Default: 0xFFFFFFFF (white)
    ARGB fontColor;

    // The default font size. Default: 12
    float fontSize;

    // The height of the window. Default: 100
    int height;

    // The image to use for this window. Default: "" (blank)
    char image[MAX_PATH];

    // The text to display. Default: "" (blank)
    WCHAR text[MAX_LINE_LENGTH];

    // The horizontal alignment of the text. Left, Center, Right. Default: Left
    char textAlign[8];

    // Text offset from the bottom. Default: 0
    float textOffsetBottom;

    // Text offset from the left. Default: 0
    float textOffsetLeft;

    // Text offset from the right. Default: 0
    float textOffsetRight;

    // Text offset from the top. Default: 0
    float textOffsetTop;

    // Text rotation. Default: 0
    float textRotation;

    // The vertical alignment of the text. Bottom, Middle, Top. Default: 0
    char textVerticalAlign[8];

    // The width of the window. Default: 100
    int width;

    // Wordwrap. Default: False
    bool wordWrap;

    // The x position of the window, relative to the parent. Default: 0
    int x;

    // The y position of the window, relative to the parent. Default: 0
    int y;
};
