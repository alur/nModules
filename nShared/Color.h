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
typedef DWORD ARGB, *LPARGB;

//
typedef struct {
    int alpha;
    int hue;
    float saturation;
    union {
        float lightness;
        float value;
    };
} AHSL, AHSV;

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
    ARGB ARGBfToARGB(int alpha, float red, float green, float blue);

    //
    ARGB HSLToARGB(int hue, float saturation, float lightness);

    //
    ARGB AHSLToARGB(int alpha, int hue, float saturation, float lightness);
    ARGB AHSLToARGB(AHSL color);

    //
    ARGB HSVToARGB(int hue, int saturation, int value);

    // 
    ARGB AHSVToARGB(int alpha, int hue, int saturation, int value);

    //
    AHSL ARGBToAHSL(ARGB color);

    //
    AHSV ARGBToAHSV(ARGB color);

    // Converts ARGB to D2D_COLOR_F
    D2D_COLOR_F ARGBToD2D(ARGB argb);

    // Converts D2D_COLOR_F to ARGB
    ARGB D2DToARGB(D2D_COLOR_F d2d);

    //
    ARGB Mix(ARGB color1, ARGB color2, float weight);

    // Holds a known color
    typedef struct KnownColor {
        LPCSTR name;
        ARGB color;
    } *LPKNOWNCOLOR;

    // Returns the null-terminated list of known colors
    LPKNOWNCOLOR GetKnownColors();
}
