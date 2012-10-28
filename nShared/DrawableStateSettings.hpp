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

class DrawableStateSettings {
public:
    explicit DrawableStateSettings();
    virtual ~DrawableStateSettings();

    // Loads the actual settings.
    void Load(Settings* settings, DrawableStateSettings* defaults);

    // The background color of the window. Default: 0xFF000000 (black)
    ARGB color;

    // The x corner radius. Default: 0
    float cornerRadiusX;

    // The y corner radius. Default: 0
    float cornerRadiusY;

    // The default font to use. Default: Arial
    WCHAR font[MAX_PATH];

    // The default font color to use. Default: 0xFFFFFFFF (white)
    ARGB fontColor;

    // The default font size. Default: 12
    float fontSize;

    // The default font stretch. Ultra Condensed, Extra Condensed, Condensed, 
    // Semi Condensed, Normal, Medium, Semi Expanded, Expanded, Extra Expanded,
    // Ultra Expanded. Default: Normal
    char fontStretch[16];

    // The default font style. Normal, Oblique, Italic. Default: Normal
    char fontStyle[8];

    // The default font weight. Thin, Extra Light, Ultra Light, Light,
    // Semi Light, Normal, Regular, Medium, Semi Bold, Bold, Extra Bold, 
    // Ultra Bold, Black, Heavy, Extra Black, Ultra Black. Default: Normal
    char fontWeight[16];

    // The image to use for this window. Default: "" (blank)
    char image[MAX_PATH];

    // The maximum opacity of the image. 0 to 1. Default: 1
    float imageOpacity;

    // The rotation of the image, in degrees. Default: 0
    float imageRotation;

    // How to position the image. Fit, Fill, Center, Stretch, or Tile. Default: Tile
    char imageScalingMode[32];

    // The color of the outline. Default: #0000
    ARGB outlineColor;

    // The width of the outline. Default: 0
    float outlineWidth;

    // Right-to-Left. Default: False
    bool rightToLeft;

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

    // The trimming setting. None, Character, Word. Default: Character
    char textTrimmingGranularity[16];

    // The vertical alignment of the text. Bottom, Middle, Top. Default: 0
    char textVerticalAlign[8];

    // Wordwrap. Default: False
    bool wordWrap;
};
