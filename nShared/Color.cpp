/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Color.h
 *  The nModules Project
 *
 *  Functions for dealing with colors.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "Color.h"
#include <math.h>

namespace Color {

    // Predefined colors. These match the CSS3 named colors.
    Color::KnownColor knownColors[] = {
        { "AliceBlue",            0xFFF0F8FF }, { "AntiqueWhite",         0xFFFAEBD7 },
        { "Aqua",                 0xFF00FFFF }, { "Aquamarine",           0xFF7FFFD4 },
        { "Azure",                0xFFF0FFFF }, { "Beige",                0xFFF5F5DC },
        { "Bisque",               0xFFFFE4C4 }, { "Black",                0xFF000000 },
        { "BlanchedAlmond",       0xFFFFEBCD }, { "Blue",                 0xFF0000FF },
        { "BlueViolet",           0xFF8A2BE2 }, { "Brown",                0xFFA52A2A },
        { "BurlyWood",            0xFFDEB887 }, { "CadetBlue",            0xFF5F9EA0 },
        { "Chartreuse",           0xFF7FFF00 }, { "Chocolate",            0xFFD2691E },
        { "Coral",                0xFFFF7F50 }, { "CornflowerBlue",       0xFF6495ED },
        { "Cornsilk",             0xFFFFF8DC }, { "Crimson",              0xFFDC143C },
        { "Cyan",                 0xFF00FFFF }, { "DarkBlue",             0xFF00008B },
        { "DarkCyan",             0xFF008B8B }, { "DarkGoldenrod",        0xFFB8860B },
        { "DarkGray",             0xFFA9A9A9 }, { "DarkGreen",            0xFF006400 },
        { "DarkGrey",             0xFFA9A9A9 }, { "DarkKhaki",            0xFFBDB76B },
        { "DarkMagenta",          0xFF8B008B }, { "DarkOliveGreen",       0xFF556B2F },
        { "DarkOrange",           0xFFFF8C00 }, { "DarkOrchid",           0xFF9932CC },
        { "DarkRed",              0xFF8B0000 }, { "DarkSalmon",           0xFFE9967A },
        { "DarkSeaGreen",         0xFF8FBC8B }, { "DarkSlateBlue",        0xFF483D8B },
        { "DarkSlateGray",        0xFF2F4F4F }, { "DarkSlateGrey",        0xFF2F4F4F },
        { "DarkTurquoise",        0xFF00CED1 }, { "DarkViolet",           0xFF9400D3 },
        { "DeepPink",             0xFFFF1493 }, { "DeepSkyBlue",          0xFF00BFFF },
        { "DimGray",              0xFF696969 }, { "DimGrey",              0xFF696969 },
        { "DodgerBlue",           0xFF1E90FF }, { "Firebrick",            0xFFB22222 },
        { "FloralWhite",          0xFFFFFAF0 }, { "ForestGreen",          0xFF228B22 },
        { "Fuchsia",              0xFFFF00FF }, { "Gainsboro",            0xFFDCDCDC },
        { "GhostWhite",           0xFFF8F8FF }, { "Gold",                 0xFFFFD700 },
        { "Goldenrod",            0xFFDAA520 }, { "Gray",                 0xFF808080 },
        { "Green",                0xFF008000 }, { "GreenYellow",          0xFFADFF2F },
        { "Grey",                 0xFF808080 }, { "Honeydew",             0xFFF0FFF0 },
        { "HotPink",              0xFFFF69B4 }, { "IndianRed",            0xFFCD5C5C },
        { "Indigo",               0xFF4B0082 }, { "Ivory",                0xFFFFFFF0 },
        { "Khaki",                0xFFF0E68C }, { "Lavender",             0xFFE6E6FA },
        { "LavenderBlush",        0xFFFFF0F5 }, { "LawnGreen",            0xFF7CFC00 },
        { "LemonChiffon",         0xFFFFFACD }, { "LightBlue",            0xFFADD8E6 },
        { "LightCoral",           0xFFF08080 }, { "LightCyan",            0xFFE0FFFF },
        { "LightGoldenrodYellow", 0xFFFAFAD2 }, { "LightGray",            0xFFD3D3D3 },
        { "LightGreen",           0xFF90EE90 }, { "LightGrey",            0xFFD3D3D3 },
        { "LightPink",            0xFFFFB6C1 }, { "LightSalmon",          0xFFFFA07A },
        { "LightSeaGreen",        0xFF20B2AA }, { "LightSkyBlue",         0xFF87CEFA },
        { "LightSlateGray",       0xFF778899 }, { "LightSlateGrey",       0xFF778899 },
        { "LightSteelBlue",       0xFFB0C4DE }, { "LightYellow",          0xFFFFFFE0 },
        { "Lime",                 0xFF00FF00 }, { "LimeGreen",            0xFF32CD32 },
        { "Linen",                0xFFFAF0E6 }, { "Magenta",              0xFFFF00FF },
        { "Maroon",               0xFF800000 }, { "MediumAquamarine",     0xFF66CDAA },
        { "MediumBlue",           0xFF0000CD }, { "MediumOrchid",         0xFFBA55D3 },
        { "MediumPurple",         0xFF9370DB }, { "MediumSeaGreen",       0xFF3CB371 },
        { "MediumSlateBlue",      0xFF7B68EE }, { "MediumSpringGreen",    0xFF00FA9A },
        { "MediumTurquoise",      0xFF48D1CC }, { "MediumVioletRed",      0xFFC71585 },
        { "MidnightBlue",         0xFF191970 }, { "MintCream",            0xFFF5FFFA },
        { "MistyRose",            0xFFFFE4E1 }, { "Moccasin",             0xFFFFE4B5 },
        { "NavajoWhite",          0xFFFFDEAD }, { "Navy",                 0xFF000080 },
        { "OldLace",              0xFFFDF5E6 }, { "Olive",                0xFF808000 },
        { "OliveDrab",            0xFF6B8E23 }, { "Orange",               0xFFFFA500 },
        { "OrangeRed",            0xFFFF4500 }, { "Orchid",               0xFFDA70D6 },
        { "PaleGoldenrod",        0xFFEEE8AA }, { "PaleGreen",            0xFF98FB98 },
        { "PaleTurquoise",        0xFFAFEEEE }, { "PaleVioletRed",        0xFFDB7093 },
        { "PapayaWhip",           0xFFFFEFD5 }, { "PeachPuff",            0xFFFFDAB9 },
        { "Peru",                 0xFFCD853F }, { "Pink",                 0xFFFFC0CB },
        { "Plum",                 0xFFDDA0DD }, { "PowderBlue",           0xFFB0E0E6 },
        { "Purple",               0xFF800080 }, { "Red",                  0xFFFF0000 },
        { "RosyBrown",            0xFFBC8F8F }, { "RoyalBlue",            0xFF4169E1 },
        { "SaddleBrown",          0xFF8B4513 }, { "Salmon",               0xFFFA8072 },
        { "SandyBrown",           0xFFF4A460 }, { "SeaGreen",             0xFF2E8B57 },
        { "SeaShell",             0xFFFFF5EE }, { "Sienna",               0xFFA0522D },
        { "Silver",               0xFFC0C0C0 }, { "SkyBlue",              0xFF87CEEB },
        { "SlateBlue",            0xFF6A5ACD }, { "SlateGray",            0xFF708090 },
        { "SlateGrey",            0xFF708090 }, { "Snow",                 0xFFFFFAFA },
        { "SpringGreen",          0xFF00FF7F }, { "SteelBlue",            0xFF4682B4 },
        { "Tan",                  0xFFD2B48C }, { "Teal",                 0xFF008080 },
        { "Thistle",              0xFFD8BFD8 }, { "Tomato",               0xFFFF6347 },
        { "Transparent",          0x00FFFFFF }, { "Turquoise",            0xFF40E0D0 },
        { "Violet",               0xFFEE82EE }, { "Wheat",                0xFFF5DEB3 },
        { "White",                0xFFFFFFFF }, { "WhiteSmoke",           0xFFF5F5F5 },
        { "Yellow",               0xFFFFFF00 }, { "YellowGreen",          0xFF9ACD32 },
        { NULL,                   NULL       }
    };
}


/// <summary>
/// Converts an ARGB formatted color to a D2D_COLOR_F format.
/// </summary>
D2D_COLOR_F Color::ARGBToD2D(ARGB argb) {
    D2D_COLOR_F ret;
    ret.a = (argb >> 24)/255.0f;
    ret.r = (argb >> 16 & 0xFF)/255.0f;
    ret.g = (argb >> 8 & 0xFF)/255.0f;
    ret.b = (argb & 0xFF)/255.0f;
    return ret;
}


/// <summary>
/// Converts a D2D_COLOR_F formatted color to the ARGB format.
/// </summary>
ARGB Color::D2DToARGB(D2D_COLOR_F d2d) {
    return ARGBToARGB(ARGB(d2d.a * 255), ARGB(d2d.r * 255), ARGB(d2d.g * 255), ARGB(d2d.b * 255));
}


/// <summary>
/// Returns the list of known colors.
/// </summary>
Color::KnownColor* Color::GetKnownColors() {
    return Color::knownColors;
}


// Converts red, green, blue values to ARGB.
ARGB Color::RGBToARGB(int red, int green, int blue) {
    return ARGBToARGB(0xFF, red, green, blue);
}


// Converts ARGB values to an ARGB.
ARGB Color::ARGBToARGB(int alpha, int red, int green, int blue) {
    return alpha << 24 | red << 16 | green << 8 | blue; 
}


// Converts ARGB values to an ARGB.
ARGB Color::ARGBfToARGB(int alpha, float red, float green, float blue) {
    return ARGBToARGB(alpha, (int)floor(red+0.5f), (int)floor(green+0.5f), (int)floor(blue+0.5f)); 
}


//
ARGB Color::HSLToARGB(int hue, float saturation, float lightness) {
    return AHSLToARGB(0xFF, hue, saturation, lightness);
}


ARGB Color::AHSLToARGB(AHSL color) {
    return AHSLToARGB(color.alpha, color.hue, color.saturation, color.lightness);
}


//
ARGB Color::AHSLToARGB(int alpha, int hue, float saturation, float lightness) {
    // Normalize the input
    float nSaturation = saturation/COLOR_MAX_SATURATION;
    float nLightness = lightness/COLOR_MAX_LIGHTNESS;

    float h = hue/60.0f;
    float chroma = nSaturation * (1 - fabs(2 * nLightness - 1));
    float m = nLightness - chroma / 2.0f;
    float x = chroma*(1.0f - fabs(fmodf(h, 2.0f) - 1.0f));

    switch ((int)h)
    {
    case 0:
        return Color::ARGBfToARGB(alpha, (chroma + m) * 255, (x + m) * 255, m * 255);
    case 1:
        return Color::ARGBfToARGB(alpha, (x + m) * 255, (chroma + m) * 255, m * 255);
    case 2:
        return Color::ARGBfToARGB(alpha, m * 255, (chroma + m) * 255, (x + m) * 255);
    case 3:
        return Color::ARGBfToARGB(alpha, m * 255, (x + m) * 255, (chroma + m) * 255);
    case 4:
        return Color::ARGBfToARGB(alpha, (x + m) * 255, m * 255, (chroma + m) * 255);
    case 5:
        return Color::ARGBfToARGB(alpha, (chroma + m) * 255, m * 255, (x + m) * 255);
    }

    return Color::ARGBToARGB(alpha, 0, 0, 0);
}


//
ARGB Color::HSVToARGB(int hue, int saturation, int value) {
    return AHSVToARGB(0xFF, hue, saturation, value);
}


// 
ARGB Color::AHSVToARGB(int alpha, int hue, int saturation, int value) {
    // Normalize the input
    float nSaturation = float(saturation)/COLOR_MAX_SATURATION;
    float nValue = float(value)/COLOR_MAX_VALUE;

    float h = hue/60.0f;
    float chroma = nValue * nSaturation;
    float m = nValue - chroma;
    float x = chroma*(1.0f - fabs(fmodf(h, 2.0f) - 1.0f));

    switch ((int)h)
    {
    case 0:
        return Color::ARGBToARGB(alpha, ARGB((chroma + m) * 255), ARGB((x + m) * 255), ARGB(m * 255));
    case 1:
        return Color::ARGBToARGB(alpha, ARGB((x + m) * 255), ARGB((chroma + m) * 255), ARGB(m * 255));
    case 2:
        return Color::ARGBToARGB(alpha, ARGB(m * 255), ARGB((chroma + m) * 255), ARGB((x + m) * 255));
    case 3:
        return Color::ARGBToARGB(alpha, ARGB(m * 255), ARGB((x + m) * 255), ARGB((chroma + m) * 255));
    case 4:
        return Color::ARGBToARGB(alpha, ARGB((x + m) * 255), ARGB(m * 255), ARGB((chroma + m) * 255));
    case 5:
        return Color::ARGBToARGB(alpha, ARGB((chroma + m) * 255), ARGB(m * 255), ARGB((x + m) * 255));
    }

    return Color::ARGBToARGB(alpha, 0, 0, 0);
}


//
AHSL Color::ARGBToAHSL(ARGB color) {
    AHSL ret;
    D2D_COLOR_F c = ARGBToD2D(color);

    ret.alpha = color >> 24;

    float M = max(c.r, max(c.g, c.b));
    float m = min(c.r, min(c.g, c.b));
    float C = M - m;

    if (C == 0) {
        ret.hue = 0;
    }
    else if (M == c.r) {
        ret.hue = int(60.0f*(fmodf((c.g - c.b)/C, 6.0f)));
    }
    else if (M == c.g) {
        ret.hue = int(60.0f*((c.b - c.r)/C + 2.0f));
    }
    else {
        ret.hue = int(60.0f*((c.r - c.g)/C + 4.0f));
    }
    
    if (ret.hue < 0) {
        ret.hue += 360;
    }

    float L = (M + m)/2.0f;

    ret.lightness = COLOR_MAX_LIGHTNESS*L;
    ret.saturation = COLOR_MAX_SATURATION*C/(1.0f-fabs(2.0f*L - 1.0f));

    return ret;
}


//
AHSV Color::ARGBToAHSV(ARGB color) {
    AHSV ret;
    D2D_COLOR_F c = ARGBToD2D(color);

    ret.alpha = color >> 24;

    float M = max(c.r, max(c.g, c.b));
    float m = min(c.r, min(c.g, c.b));
    float C = M - m;

    if (C == 0) {
        ret.hue = 0;
    }
    else if (M == c.r) {
        ret.hue = int(60.0f*(fmodf((c.g - c.b)/C, 6.0f)));
    }
    else if (M == c.g) {
        ret.hue = int(60.0f*((c.b - c.r)/C + 2.0f));
    }
    else {
        ret.hue = int(60.0f*((c.r - c.g)/C + 4.0f));
    }
    
    if (ret.hue < 0) {
        ret.hue += 360;
    }

    ret.value = UCHAR(COLOR_MAX_VALUE*M);
    ret.saturation = UCHAR(COLOR_MAX_SATURATION*C/M);

    return ret;
}
