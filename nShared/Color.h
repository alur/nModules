/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Color.h
 *  The nModules Project
 *
 *  Functions for dealing with colors.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include <d2d1.h>

// Color of the format AARRGGBB.
typedef DWORD ARGB;

// HSL/HSV max hue
#define COLOR_MAX_HUE 359

// HSL/HSV max saturation
#define COLOR_MAX_SATURATION 100

// HSL max lightness
#define COLOR_MAX_LIGHTNESS 100

// HSV max value
#define COLOR_MAX_VALUE 100

namespace Color {
    // Converts red, green, blue values to ARGB.
    ARGB RGBToARGB(int red, int green, int blue);

    // Converts ARGB values to an ARGB.
    ARGB ARGBToARGB(int alpha, int red, int green, int blue);

    //
    ARGB HSLToARGB(int hue, int saturation, int lightness);

    //
    ARGB AHSLToARGB(int alpha, int hue, int saturation, int lightness);

    //
    ARGB HSVToARGB(int hue, int saturation, int value);

    // 
    ARGB AHSVToARGB(int alpha, int hue, int saturation, int value);

    // Converts ARGB to D2D_COLOR_F
    D2D_COLOR_F ARGBToD2D(ARGB argb);

    // Converts D2D_COLOR_F to ARGB
    ARGB D2DToARGB(D2D_COLOR_F d2d);

    // Holds a known color
    typedef struct {
        LPCSTR name;
        ARGB color;
    } KnownColor;

    // Returns the null-terminated list of known colors
    KnownColor* GetKnownColors();
}
