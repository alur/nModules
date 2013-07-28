/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  StateSettings.hpp
 *  The nModules Project
 *
 *  Settings used by states.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include <d2d1.h>
#include <dwrite.h>
#include "Settings.hpp"
#include "BrushSettings.hpp"

class StateSettings {
public:
    explicit StateSettings();

    // Loads the actual settings.
    void Load(Settings* settings, StateSettings* defaults);
    
    static DWRITE_FONT_STRETCH ParseFontStretch(LPCSTR string);
    static DWRITE_FONT_STYLE ParseFontStyle(LPCSTR fontStyle);
    static DWRITE_FONT_WEIGHT ParseFontWeight(LPCSTR fontWeight);
    static DWRITE_TEXT_ALIGNMENT ParseTextAlignment(LPCSTR textAlignment);
    static DWRITE_PARAGRAPH_ALIGNMENT ParseParagraphAlignment(LPCSTR paragraphAlignment);
    static DWRITE_TRIMMING_GRANULARITY ParseTrimmingGranularity(LPCSTR trimmingGranularity);
    static DWRITE_READING_DIRECTION ParseReadingDirection(LPCSTR readingDirection);
    static DWRITE_WORD_WRAPPING ParseWordWrapping(LPCSTR wordWrapping);

public:
    // The settings to use for the background brush.
    BrushSettings backgroundBrush;
    
    // The settings to use for the text brush.
    BrushSettings textBrush;

    // The settings to use for the outline brush.
    BrushSettings outlineBrush;

    // The settings to use for the text's dropshadow brush.
    BrushSettings textDropShadowBrush;

    // The x corner radius. Default: 0
    float cornerRadiusX;

    // The y corner radius. Default: 0
    float cornerRadiusY;

    // The default font to use. Default: Arial
    WCHAR font[MAX_PATH];

    // The default font size. Default: 12
    float fontSize;

    // The default font stretch. Ultra Condensed, Extra Condensed, Condensed, 
    // Semi Condensed, Normal, Medium, Semi Expanded, Expanded, Extra Expanded,
    // Ultra Expanded. Default: Normal
    DWRITE_FONT_STRETCH fontStretch;

    // The default font style. Normal, Oblique, Italic. Default: Normal
    DWRITE_FONT_STYLE fontStyle;

    // The default font weight. Thin, Extra Light, Ultra Light, Light,
    // Semi Light, Normal, Regular, Medium, Semi Bold, Bold, Extra Bold, 
    // Ultra Bold, Black, Heavy, Extra Black, Ultra Black. Default: Normal
    DWRITE_FONT_WEIGHT fontWeight;

    // The width of the outline. Default: 0
    float outlineWidth;

    //
    DWRITE_READING_DIRECTION readingDirection;

    // The horizontal alignment of the text. Left, Center, Right. Default: Left
    DWRITE_TEXT_ALIGNMENT textAlign;

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
    DWRITE_TRIMMING_GRANULARITY textTrimmingGranularity;

    // The vertical alignment of the text. Bottom, Middle, Top. Default: Top
    DWRITE_PARAGRAPH_ALIGNMENT textVerticalAlign;

    // 
    DWRITE_WORD_WRAPPING wordWrapping;
};
