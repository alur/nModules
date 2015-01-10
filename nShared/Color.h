/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Color.h
 *  The nModules Project
 *
 *  Functions for dealing with colors.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "IColorVal.hpp"

#include "../Utilities/CommonD2D.h"

#include <memory>

// Color of the format AARRGGBB.
typedef DWORD ARGB, *LPARGB;

// Alpha, Hue, Saturation, Lightness
typedef struct AHSL
{
    int alpha;
    int hue;
    float saturation;
    float lightness;
} *LPAHSL;

// Alpha, Hue, Saturation, Value
typedef struct AHSV
{
    int alpha;
    int hue;
    float saturation;
    float value;
} *LPAHSV;

// HSL/HSV max hue
#define COLOR_MAX_HUE 359

// HSL/HSV max saturation
#define COLOR_MAX_SATURATION 100

// HSL max lightness
#define COLOR_MAX_LIGHTNESS 100

// HSV max value
#define COLOR_MAX_VALUE 100

//
bool ParseColor(LPCTSTR color, IColorVal **target);

namespace Color
{
    // Conversion functions
    ARGB RGBToARGB(int red, int green, int blue);
    ARGB ARGBToARGB(int alpha, int red, int green, int blue);
    ARGB ARGBfToARGB(int alpha, float red, float green, float blue);
    ARGB HSLToARGB(int hue, float saturation, float lightness);
    ARGB AHSLToARGB(int alpha, int hue, float saturation, float lightness);
    ARGB AHSLToARGB(AHSL color);
    ARGB HSVToARGB(int hue, int saturation, int value);
    ARGB AHSVToARGB(int alpha, int hue, int saturation, int value);
    AHSL ARGBToAHSL(ARGB color);
    AHSV ARGBToAHSV(ARGB color);
    D2D_COLOR_F ARGBToD2D(ARGB argb);
    ARGB D2DToARGB(D2D_COLOR_F d2d);

    // Color functions
    ARGB Mix(ARGB color1, ARGB color2, float weight);

    // Parsing
    IColorVal* Parse(LPCTSTR colorString, const IColorVal* defaultValue);
    std::unique_ptr<IColorVal> Create(ARGB value);

    //
    bool GetNamedColor(LPCTSTR name, LPARGB color);

    // Returns the null-terminated list of known colors
    // GetKnownColors();
}
