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

namespace Color {
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
