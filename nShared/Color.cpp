/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Color.cpp
 *  The nModules Project
 *
 *  Functions for dealing with colors.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "LiteStep.h"
#include "Color.h"
#include "IColorVal.hpp"
#include "../Utilities/Math.h"
#include <algorithm>
#include "LiteralColorVal.hpp"

using namespace Math;

namespace Color {
    // Predefined colors. These match the CSS3 named colors.
    KnownColor knownColors[] = {
        { _T("AliceBlue"),            0xFFF0F8FF }, { _T("AntiqueWhite"),         0xFFFAEBD7 },
        { _T("Aqua"),                 0xFF00FFFF }, { _T("Aquamarine"),           0xFF7FFFD4 },
        { _T("Azure"),                0xFFF0FFFF }, { _T("Beige"),                0xFFF5F5DC },
        { _T("Bisque"),               0xFFFFE4C4 }, { _T("Black"),                0xFF000000 },
        { _T("BlanchedAlmond"),       0xFFFFEBCD }, { _T("Blue"),                 0xFF0000FF },
        { _T("BlueViolet"),           0xFF8A2BE2 }, { _T("Brown"),                0xFFA52A2A },
        { _T("BurlyWood"),            0xFFDEB887 }, { _T("CadetBlue"),            0xFF5F9EA0 },
        { _T("Chartreuse"),           0xFF7FFF00 }, { _T("Chocolate"),            0xFFD2691E },
        { _T("Coral"),                0xFFFF7F50 }, { _T("CornflowerBlue"),       0xFF6495ED },
        { _T("Cornsilk"),             0xFFFFF8DC }, { _T("Crimson"),              0xFFDC143C },
        { _T("Cyan"),                 0xFF00FFFF }, { _T("DarkBlue"),             0xFF00008B },
        { _T("DarkCyan"),             0xFF008B8B }, { _T("DarkGoldenrod"),        0xFFB8860B },
        { _T("DarkGray"),             0xFFA9A9A9 }, { _T("DarkGreen"),            0xFF006400 },
        { _T("DarkGrey"),             0xFFA9A9A9 }, { _T("DarkKhaki"),            0xFFBDB76B },
        { _T("DarkMagenta"),          0xFF8B008B }, { _T("DarkOliveGreen"),       0xFF556B2F },
        { _T("DarkOrange"),           0xFFFF8C00 }, { _T("DarkOrchid"),           0xFF9932CC },
        { _T("DarkRed"),              0xFF8B0000 }, { _T("DarkSalmon"),           0xFFE9967A },
        { _T("DarkSeaGreen"),         0xFF8FBC8B }, { _T("DarkSlateBlue"),        0xFF483D8B },
        { _T("DarkSlateGray"),        0xFF2F4F4F }, { _T("DarkSlateGrey"),        0xFF2F4F4F },
        { _T("DarkTurquoise"),        0xFF00CED1 }, { _T("DarkViolet"),           0xFF9400D3 },
        { _T("DeepPink"),             0xFFFF1493 }, { _T("DeepSkyBlue"),          0xFF00BFFF },
        { _T("DimGray"),              0xFF696969 }, { _T("DimGrey"),              0xFF696969 },
        { _T("DodgerBlue"),           0xFF1E90FF }, { _T("Firebrick"),            0xFFB22222 },
        { _T("FloralWhite"),          0xFFFFFAF0 }, { _T("ForestGreen"),          0xFF228B22 },
        { _T("Fuchsia"),              0xFFFF00FF }, { _T("Gainsboro"),            0xFFDCDCDC },
        { _T("GhostWhite"),           0xFFF8F8FF }, { _T("Gold"),                 0xFFFFD700 },
        { _T("Goldenrod"),            0xFFDAA520 }, { _T("Gray"),                 0xFF808080 },
        { _T("Green"),                0xFF008000 }, { _T("GreenYellow"),          0xFFADFF2F },
        { _T("Grey"),                 0xFF808080 }, { _T("Honeydew"),             0xFFF0FFF0 },
        { _T("HotPink"),              0xFFFF69B4 }, { _T("IndianRed"),            0xFFCD5C5C },
        { _T("Indigo"),               0xFF4B0082 }, { _T("Ivory"),                0xFFFFFFF0 },
        { _T("Khaki"),                0xFFF0E68C }, { _T("Lavender"),             0xFFE6E6FA },
        { _T("LavenderBlush"),        0xFFFFF0F5 }, { _T("LawnGreen"),            0xFF7CFC00 },
        { _T("LemonChiffon"),         0xFFFFFACD }, { _T("LightBlue"),            0xFFADD8E6 },
        { _T("LightCoral"),           0xFFF08080 }, { _T("LightCyan"),            0xFFE0FFFF },
        { _T("LightGoldenrodYellow"), 0xFFFAFAD2 }, { _T("LightGray"),            0xFFD3D3D3 },
        { _T("LightGreen"),           0xFF90EE90 }, { _T("LightGrey"),            0xFFD3D3D3 },
        { _T("LightPink"),            0xFFFFB6C1 }, { _T("LightSalmon"),          0xFFFFA07A },
        { _T("LightSeaGreen"),        0xFF20B2AA }, { _T("LightSkyBlue"),         0xFF87CEFA },
        { _T("LightSlateGray"),       0xFF778899 }, { _T("LightSlateGrey"),       0xFF778899 },
        { _T("LightSteelBlue"),       0xFFB0C4DE }, { _T("LightYellow"),          0xFFFFFFE0 },
        { _T("Lime"),                 0xFF00FF00 }, { _T("LimeGreen"),            0xFF32CD32 },
        { _T("Linen"),                0xFFFAF0E6 }, { _T("Magenta"),              0xFFFF00FF },
        { _T("Maroon"),               0xFF800000 }, { _T("MediumAquamarine"),     0xFF66CDAA },
        { _T("MediumBlue"),           0xFF0000CD }, { _T("MediumOrchid"),         0xFFBA55D3 },
        { _T("MediumPurple"),         0xFF9370DB }, { _T("MediumSeaGreen"),       0xFF3CB371 },
        { _T("MediumSlateBlue"),      0xFF7B68EE }, { _T("MediumSpringGreen"),    0xFF00FA9A },
        { _T("MediumTurquoise"),      0xFF48D1CC }, { _T("MediumVioletRed"),      0xFFC71585 },
        { _T("MidnightBlue"),         0xFF191970 }, { _T("MintCream"),            0xFFF5FFFA },
        { _T("MistyRose"),            0xFFFFE4E1 }, { _T("Moccasin"),             0xFFFFE4B5 },
        { _T("NavajoWhite"),          0xFFFFDEAD }, { _T("Navy"),                 0xFF000080 },
        { _T("OldLace"),              0xFFFDF5E6 }, { _T("Olive"),                0xFF808000 },
        { _T("OliveDrab"),            0xFF6B8E23 }, { _T("Orange"),               0xFFFFA500 },
        { _T("OrangeRed"),            0xFFFF4500 }, { _T("Orchid"),               0xFFDA70D6 },
        { _T("PaleGoldenrod"),        0xFFEEE8AA }, { _T("PaleGreen"),            0xFF98FB98 },
        { _T("PaleTurquoise"),        0xFFAFEEEE }, { _T("PaleVioletRed"),        0xFFDB7093 },
        { _T("PapayaWhip"),           0xFFFFEFD5 }, { _T("PeachPuff"),            0xFFFFDAB9 },
        { _T("Peru"),                 0xFFCD853F }, { _T("Pink"),                 0xFFFFC0CB },
        { _T("Plum"),                 0xFFDDA0DD }, { _T("PowderBlue"),           0xFFB0E0E6 },
        { _T("Purple"),               0xFF800080 }, { _T("Red"),                  0xFFFF0000 },
        { _T("RosyBrown"),            0xFFBC8F8F }, { _T("RoyalBlue"),            0xFF4169E1 },
        { _T("SaddleBrown"),          0xFF8B4513 }, { _T("Salmon"),               0xFFFA8072 },
        { _T("SandyBrown"),           0xFFF4A460 }, { _T("SeaGreen"),             0xFF2E8B57 },
        { _T("SeaShell"),             0xFFFFF5EE }, { _T("Sienna"),               0xFFA0522D },
        { _T("Silver"),               0xFFC0C0C0 }, { _T("SkyBlue"),              0xFF87CEEB },
        { _T("SlateBlue"),            0xFF6A5ACD }, { _T("SlateGray"),            0xFF708090 },
        { _T("SlateGrey"),            0xFF708090 }, { _T("Snow"),                 0xFFFFFAFA },
        { _T("SpringGreen"),          0xFF00FF7F }, { _T("SteelBlue"),            0xFF4682B4 },
        { _T("Tan"),                  0xFFD2B48C }, { _T("Teal"),                 0xFF008080 },
        { _T("Thistle"),              0xFFD8BFD8 }, { _T("Tomato"),               0xFFFF6347 },
        { _T("Transparent"),          0x00FFFFFF }, { _T("Turquoise"),            0xFF40E0D0 },
        { _T("Violet"),               0xFFEE82EE }, { _T("Wheat"),                0xFFF5DEB3 },
        { _T("White"),                0xFFFFFFFF }, { _T("WhiteSmoke"),           0xFFF5F5F5 },
        { _T("Yellow"),               0xFFFFFF00 }, { _T("YellowGreen"),          0xFF9ACD32 },
        { nullptr,                    NULL       }
    };
}


/// <summary>
/// Converts an ARGB formatted color to a D2D_COLOR_F format.
/// </summary>
/// <param name="argb">The color to convert.</param>
D2D_COLOR_F Color::ARGBToD2D(ARGB argb)
{
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
/// <param name="d2d">The color to convert.</param>
ARGB Color::D2DToARGB(D2D_COLOR_F d2d)
{
    return ARGBToARGB(ARGB(d2d.a * 255), ARGB(d2d.r * 255), ARGB(d2d.g * 255), ARGB(d2d.b * 255));
}


/// <summary>
/// Returns a null terminated list of known colors.
/// </summary>
Color::LPKNOWNCOLOR Color::GetKnownColors()
{
    return Color::knownColors;
}


/// <summary>
/// Converts red, green, blue values to ARGB.
/// </summary>
ARGB Color::RGBToARGB(int red, int green, int blue)
{
    return ARGBToARGB(0xFF, red, green, blue);
}


/// <summary>
/// Converts ARGB values to an ARGB.
/// </summary>
ARGB Color::ARGBToARGB(int alpha, int red, int green, int blue)
{
    return alpha << 24 | red << 16 | green << 8 | blue; 
}


/// <summary>
/// Converts ARGB float values to an ARGB.
/// </summary>
ARGB Color::ARGBfToARGB(int alpha, float red, float green, float blue)
{
    return ARGBToARGB(alpha, (int)floor(red+0.5f), (int)floor(green+0.5f), (int)floor(blue+0.5f)); 
}


/// <summary>
/// Converts hue, saturation, and lightness to an ARGB.
/// </summary>
ARGB Color::HSLToARGB(int hue, float saturation, float lightness)
{
    return AHSLToARGB(0xFF, hue, saturation, lightness);
}


/// <summary>
/// Converts an AHSL to an ARGB.
/// </summary>
ARGB Color::AHSLToARGB(AHSL color)
{
    return AHSLToARGB(color.alpha, color.hue, color.saturation, color.lightness);
}


/// <summary>
/// Converts alpha, hue, saturation, and lightness to an ARGB.
/// </summary>
ARGB Color::AHSLToARGB(int alpha, int hue, float saturation, float lightness)
{
    // Normalize the input
    float nSaturation = saturation/COLOR_MAX_SATURATION;
    float nLightness = lightness/COLOR_MAX_LIGHTNESS;

    float h = hue/60.0f;
    float chroma = nSaturation * (1 - fabs(2 * nLightness - 1));
    float m = nLightness - chroma / 2.0f;
    float x = chroma*(1.0f - fabs(fmodf(h, 2.0f) - 1.0f));

    switch (int(h))
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
ARGB Color::HSVToARGB(int hue, int saturation, int value)
{
    return AHSVToARGB(0xFF, hue, saturation, value);
}


// 
ARGB Color::AHSVToARGB(int alpha, int hue, int saturation, int value)
{
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
AHSL Color::ARGBToAHSL(ARGB color)
{
    AHSL ret;
    D2D_COLOR_F c = ARGBToD2D(color);

    ret.alpha = color >> 24;

    float M = max(c.r, max(c.g, c.b));
    float m = min(c.r, min(c.g, c.b));
    float C = M - m;

    if (C == 0)
    {
        ret.hue = 0;
    }
    else if (M == c.r)
    {
        ret.hue = int(60.0f*(fmodf((c.g - c.b)/C, 6.0f)));
    }
    else if (M == c.g)
    {
        ret.hue = int(60.0f*((c.b - c.r)/C + 2.0f));
    }
    else
    {
        ret.hue = int(60.0f*((c.r - c.g)/C + 4.0f));
    }
    
    if (ret.hue < 0)
    {
        ret.hue += 360;
    }

    float L = (M + m)/2.0f;

    ret.lightness = COLOR_MAX_LIGHTNESS*L;
    ret.saturation = COLOR_MAX_SATURATION*C/(1.0f-fabs(2.0f*L - 1.0f));

    return ret;
}


//
AHSV Color::ARGBToAHSV(ARGB color)
{
    AHSV ret;
    D2D_COLOR_F c = ARGBToD2D(color);

    ret.alpha = color >> 24;

    float M = max(c.r, max(c.g, c.b));
    float m = min(c.r, min(c.g, c.b));
    float C = M - m;

    if (C == 0)
    {
        ret.hue = 0;
    }
    else if (M == c.r)
    {
        ret.hue = int(60.0f*(fmodf((c.g - c.b)/C, 6.0f)));
    }
    else if (M == c.g)
    {
        ret.hue = int(60.0f*((c.b - c.r)/C + 2.0f));
    }
    else {
        ret.hue = int(60.0f*((c.r - c.g)/C + 4.0f));
    }
    
    if (ret.hue < 0)
    {
        ret.hue += 360;
    }

    ret.value = UCHAR(COLOR_MAX_VALUE*M);
    ret.saturation = UCHAR(COLOR_MAX_SATURATION*C/M);

    return ret;
}


ARGB Color::Mix(ARGB color1, ARGB color2, float weight)
{
    AHSL HSLcolor1 = Color::ARGBToAHSL(color1);
    AHSL HSLcolor2 = Color::ARGBToAHSL(color2);
    AHSL mix;
    mix.alpha = int(Math::Lerp(float(HSLcolor1.alpha), float(HSLcolor2.alpha), weight) + 0.5f);
    mix.lightness = Math::Lerp(HSLcolor1.lightness, HSLcolor2.lightness, weight);
    mix.saturation = Math::Lerp(HSLcolor1.saturation, HSLcolor2.saturation, weight);
    mix.hue = int(Math::WrappingLerp(float(HSLcolor1.hue), float(HSLcolor2.hue), weight, 0, COLOR_MAX_HUE));

    return Color::AHSLToARGB(mix);
}


// Parsing
IColorVal* Color::Parse(LPCTSTR colorString, const IColorVal* defaultValue)
{
    IColorVal *color;
    if (ParseColor(colorString, &color))
    {
        return color;
    }
    return defaultValue->Copy();
}


std::unique_ptr<IColorVal> Color::Create(ARGB value)
{
    return std::unique_ptr<IColorVal>(new LiteralColorVal(value));
}
