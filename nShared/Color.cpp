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
#include "../Utilities/StringUtils.h"
#include <algorithm>
#include "LiteralColorVal.hpp"
#include <unordered_map>
#include <functional>

typedef StringKeyedMaps<LPCWSTR, ARGB>::UnorderedMap ColorMap;

// Predefined colors. These contain all the CSS3 named colors, and some extras.
static ColorMap namedColors = {
  { L"AliceBlue",              0xFFF0F8FF },
  { L"Almond",                 0xFFEFDECD },
  { L"AntiqueBrass",           0xFFCD9575 },
  { L"AntiqueWhite",           0xFFFAEBD7 },
  { L"Apricot",                0xFFFDD9B5 },
  { L"Aqua",                   0xFF00FFFF },
  { L"Aquamarine",             0xFF7FFFD4 },
  { L"Asparagus",              0xFF87A96B },
  { L"AtomicTangerine",        0xFFFFA474 },
  { L"Azure",                  0xFFF0FFFF },
  { L"BananaMania",            0xFFFAE7B5 },
  { L"Beaver",                 0xFF9F8170 },
  { L"Beige",                  0xFFF5F5DC },
  { L"Bisque",                 0xFFFFE4C4 },
  { L"Bittersweet",            0xFFFD7C6E },
  { L"Black",                  0xFF000000 },
  { L"BlanchedAlmond",         0xFFFFEBCD },
  { L"BlizzardBlue",           0xFFACE5EE },
  { L"Blue",                   0xFF0000FF },
  { L"BlueBell",               0xFFA2A2D0 },
  { L"BlueGray",               0xFF6699CC },
  { L"BlueGreen",              0xFF0D98BA },
  { L"BlueViolet",             0xFF8A2BE2 },
  { L"Blush",                  0xFFDE5D83 },
  { L"BrickRed",               0xFFCB4154 },
  { L"Brown",                  0xFFA52A2A },
  { L"BurlyWood",              0xFFDEB887 },
  { L"BurntOrange",            0xFFFF7F49 },
  { L"BurntSienna",            0xFFEA7E5D },
  { L"CadetBlue",              0xFF5F9EA0 },
  { L"Canary",                 0xFFFFFF99 },
  { L"CaribbeanGreen",         0xFF1CD3A2 },
  { L"CarnationPink",          0xFFFFAACC },
  { L"Cerise",                 0xFFDD4492 },
  { L"Cerulean",               0xFF1DACD6 },
  { L"Chartreuse",             0xFF7FFF00 },
  { L"Chestnut",               0xFFBC5D58 },
  { L"Chocolate",              0xFFD2691E },
  { L"Copper",                 0xFFDD9475 },
  { L"Coral",                  0xFFFF7F50 },
  { L"Cornflower",             0xFF9ACEEB },
  { L"CornflowerBlue",         0xFF6495ED },
  { L"Cornsilk",               0xFFFFF8DC },
  { L"CottonCandy",            0xFFFFBCD9 },
  { L"Cranberry",              0xFFDE5D83 },
  { L"Crimson",                0xFFDC143C },
  { L"Cyan",                   0xFF00FFFF },
  { L"Dandelion",              0xFFFDDB6D },
  { L"DarkBlue",               0xFF00008B },
  { L"DarkCyan",               0xFF008B8B },
  { L"DarkGoldenrod",          0xFFB8860B },
  { L"DarkGray",               0xFFA9A9A9 },
  { L"DarkGreen",              0xFF006400 },
  { L"DarkGrey",               0xFFA9A9A9 },
  { L"DarkKhaki",              0xFFBDB76B },
  { L"DarkMagenta",            0xFF8B008B },
  { L"DarkOliveGreen",         0xFF556B2F },
  { L"DarkOrange",             0xFFFF8C00 },
  { L"DarkOrchid",             0xFF9932CC },
  { L"DarkRed",                0xFF8B0000 },
  { L"DarkSalmon",             0xFFE9967A },
  { L"DarkSeaGreen",           0xFF8FBC8B },
  { L"DarkSlateBlue",          0xFF483D8B },
  { L"DarkSlateGray",          0xFF2F4F4F },
  { L"DarkSlateGrey",          0xFF2F4F4F },
  { L"DarkTurquoise",          0xFF00CED1 },
  { L"DarkViolet",             0xFF9400D3 },
  { L"DeepPink",               0xFFFF1493 },
  { L"DeepSkyBlue",            0xFF00BFFF },
  { L"Denim",                  0xFF2B6CC4 },
  { L"DesertSand",             0xFFEFCDB8 },
  { L"DimGray",                0xFF696969 },
  { L"DimGrey",                0xFF696969 },
  { L"DodgerBlue",             0xFF1E90FF },
  { L"Eggplant",               0xFF6E5160 },
  { L"ElectricLime",           0xFFCEFF1D },
  { L"Fern",                   0xFF71BC78 },
  { L"Firebrick",              0xFFB22222 },
  { L"FloralWhite",            0xFFFFFAF0 },
  { L"ForestGreen",            0xFF228B22 },
  { L"Fuchsia",                0xFFFF00FF },
  { L"FuzzyWuzzy",             0xFFCC6666 },
  { L"Gainsboro",              0xFFDCDCDC },
  { L"GhostWhite",             0xFFF8F8FF },
  { L"Gold",                   0xFFFFD700 },
  { L"Goldenrod",              0xFFDAA520 },
  { L"GrannySmithApple",       0xFFA8E4A0 },
  { L"Gray",                   0xFF808080 },
  { L"Green",                  0xFF008000 },
  { L"GreenBlue",              0xFF1164B4 },
  { L"GreenYellow",            0xFFADFF2F },
  { L"Grey",                   0xFF808080 },
  { L"Honeydew",               0xFFF0FFF0 },
  { L"HotMagenta",             0xFFFF1DCE },
  { L"HotPink",                0xFFFF69B4 },
  { L"Inchworm",               0xFFB2EC5D },
  { L"IndianRed",              0xFFCD5C5C },
  { L"Indigo",                 0xFF4B0082 },
  { L"Ivory",                  0xFFFFFFF0 },
  { L"JazzberryJam",           0xFFCA3767 },
  { L"JungleGreen",            0xFF3BB08F },
  { L"Khaki",                  0xFFF0E68C },
  { L"LaserLemon",             0xFFFEFE22 },
  { L"Lavender",               0xFFE6E6FA },
  { L"LavenderBlush",          0xFFFFF0F5 },
  { L"LawnGreen",              0xFF7CFC00 },
  { L"LemonChiffon",           0xFFFFFACD },
  { L"LemonYellow",            0xFFFFF44F },
  { L"LightBlue",              0xFFADD8E6 },
  { L"LightCoral",             0xFFF08080 },
  { L"LightCyan",              0xFFE0FFFF },
  { L"LightGoldenrodYellow",   0xFFFAFAD2 },
  { L"LightGray",              0xFFD3D3D3 },
  { L"LightGreen",             0xFF90EE90 },
  { L"LightGrey",              0xFFD3D3D3 },
  { L"LightPink",              0xFFFFB6C1 },
  { L"LightSalmon",            0xFFFFA07A },
  { L"LightSeaGreen",          0xFF20B2AA },
  { L"LightSkyBlue",           0xFF87CEFA },
  { L"LightSlateGray",         0xFF778899 },
  { L"LightSlateGrey",         0xFF778899 },
  { L"LightSteelBlue",         0xFFB0C4DE },
  { L"LightYellow",            0xFFFFFFE0 },
  { L"Lime",                   0xFF00FF00 },
  { L"LimeGreen",              0xFF32CD32 },
  { L"Linen",                  0xFFFAF0E6 },
  { L"MacaroniAndCheese",      0xFFFFBD88 },
  { L"Magenta",                0xFFFF00FF },
  { L"MagicMint",              0xFFAAF0D1 },
  { L"Mahogany",               0xFFCD4A4C },
  { L"Maize",                  0xFFEDD19C },
  { L"Manatee",                0xFF979AAA },
  { L"MangoTango",             0xFFFF8243 },
  { L"Maroon",                 0xFF800000 },
  { L"Mauvelous",              0xFFEF98AA },
  { L"MediumAquamarine",       0xFF66CDAA },
  { L"MediumBlue",             0xFF0000CD },
  { L"MediumOrchid",           0xFFBA55D3 },
  { L"MediumPurple",           0xFF9370DB },
  { L"MediumSeaGreen",         0xFF3CB371 },
  { L"MediumSlateBlue",        0xFF7B68EE },
  { L"MediumSpringGreen",      0xFF00FA9A },
  { L"MediumTurquoise",        0xFF48D1CC },
  { L"MediumVioletRed",        0xFFC71585 },
  { L"Melon",                  0xFFFDBCB4 },
  { L"MidnightBlue",           0xFF191970 },
  { L"MintCream",              0xFFF5FFFA },
  { L"MistyRose",              0xFFFFE4E1 },
  { L"Moccasin",               0xFFFFE4B5 },
  { L"MountainMeadow",         0xFF30BA8F },
  { L"Mulberry",               0xFFC54B8C },
  { L"NavajoWhite",            0xFFFFDEAD },
  { L"Navy",                   0xFF000080 },
  { L"NavyBlue",               0xFF1974D2 },
  { L"NeonCarrot",             0xFFFFA343 },
  { L"OldLace",                0xFFFDF5E6 },
  { L"Olive",                  0xFF808000 },
  { L"OliveDrab",              0xFF6B8E23 },
  { L"OliveGreen",             0xFFBAB86C },
  { L"Orange",                 0xFFFFA500 },
  { L"OrangeRed",              0xFFFF4500 },
  { L"OrangeYellow",           0xFFF8D568 },
  { L"Orchid",                 0xFFDA70D6 },
  { L"OuterSpace",             0xFF414A4C },
  { L"OutrageousOrange",       0xFFFF6E4A },
  { L"Pacific Blue",           0xFF1CA9C9 },
  { L"PaleGoldenrod",          0xFFEEE8AA },
  { L"PaleGreen",              0xFF98FB98 },
  { L"PaleTurquoise",          0xFFAFEEEE },
  { L"PaleVioletRed",          0xFFDB7093 },
  { L"PapayaWhip",             0xFFFFEFD5 },
  { L"Peach",                  0xFFFFCFAB },
  { L"PeachPuff",              0xFFFFDAB9 },
  { L"Periwinkle",             0xFFC5D0E6 },
  { L"Peru",                   0xFFCD853F },
  { L"PiggyPink",              0xFFFDDDE6 },
  { L"PineGreen",              0xFF158078 },
  { L"Pink",                   0xFFFFC0CB },
  { L"PinkFlamingo",           0xFFFC74FD },
  { L"PinkSherbert",           0xFFF78FA7 },
  { L"Plum",                   0xFFDDA0DD },
  { L"PowderBlue",             0xFFB0E0E6 },
  { L"Purple",                 0xFF800080 },
  { L"PurpleHeart",            0xFF7442C8 },
  { L"PurpleMountainsMajesty", 0xFF9D81BA },
  { L"PurplePizzazz",          0xFFFE4EDA },
  { L"RadicalRed",             0xFFFF496C },
  { L"RawSienna",              0xFFD68A59 },
  { L"RawUmber",               0xFF714B23 },
  { L"RazzleDazzleRose",       0xFFFF48D0 },
  { L"Razzmatazz",             0xFFE3256B },
  { L"Red",                    0xFFFF0000 },
  { L"RedOrange",              0xFFFF5349 },
  { L"RedViolet",              0xFFC0448F },
  { L"RobinsEggBlue",          0xFF1FCECB },
  { L"RosyBrown",              0xFFBC8F8F },
  { L"RoyalBlue",              0xFF4169E1 },
  { L"RoyalPurple",            0xFF7851A9 },
  { L"SaddleBrown",            0xFF8B4513 },
  { L"Salmon",                 0xFFFA8072 },
  { L"SandyBrown",             0xFFF4A460 },
  { L"Scarlet",                0xFFFC2847 },
  { L"ScreaminGreen",          0xFF76FF7A },
  { L"SeaGreen",               0xFF2E8B57 },
  { L"SeaShell",               0xFFFFF5EE },
  { L"Sepia",                  0xFFA5694F },
  { L"Shadow",                 0xFF8A795D },
  { L"Shamrock",               0xFF45CEA2 },
  { L"ShockingPink",           0xFFFB7EFD },
  { L"Sienna",                 0xFFA0522D },
  { L"Silver",                 0xFFC0C0C0 },
  { L"SkyBlue",                0xFF87CEEB },
  { L"SlateBlue",              0xFF6A5ACD },
  { L"SlateGray",              0xFF708090 },
  { L"SlateGrey",              0xFF708090 },
  { L"Snow",                   0xFFFFFAFA },
  { L"SpringGreen",            0xFF00FF7F },
  { L"SteelBlue",              0xFF4682B4 },
  { L"SunGlow",                0xFFFFCF48 },
  { L"SunsetOrange",           0xFFFD5E53 },
  { L"Tan",                    0xFFD2B48C },
  { L"Teal",                   0xFF008080 },
  { L"TealBlue",               0xFF18A7B5 },
  { L"Thistle",                0xFFD8BFD8 },
  { L"TickleMePink",           0xFFFC89AC },
  { L"Timberwolf",             0xFFDBD7D2 },
  { L"Tomato",                 0xFFFF6347 },
  { L"Transparent",            0x00FFFFFF },
  { L"TropicalRainForest",     0xFF17806D },
  { L"Tumbleweed",             0xFFDEAA88 },
  { L"Turquoise",              0xFF40E0D0 },
  { L"TurquoiseBlue",          0xFF77DDE7 },
  { L"UnmellowYellow",         0xFFFFFF66 },
  { L"Violet",                 0xFFEE82EE },
  { L"VioletPurple",           0xFF926EAE },
  { L"VioletBlue",             0xFF324AB2 },
  { L"VioletRed",              0xFFF75394 },
  { L"VividTangerine",         0xFFFFA089 },
  { L"VividViolet",            0xFF8F509D },
  { L"Wheat",                  0xFFF5DEB3 },
  { L"White",                  0xFFFFFFFF },
  { L"WhiteSmoke",             0xFFF5F5F5 },
  { L"WildBlueYonder",         0xFFA2ADD0 },
  { L"WildStrawberry",         0xFFFF43A4 },
  { L"WildWatermelon",         0xFFFC6C85 },
  { L"Wisteria",               0xFFCDA4DE },
  { L"Yellow",                 0xFFFFFF00 },
  { L"YellowGreen",            0xFF9ACD32 },
  { L"YellowOrange",           0xFFFFAE42 }
};


/// <summary>
/// Converts an ARGB formatted color to a D2D_COLOR_F format.
/// </summary>
/// <param name="argb">The color to convert.</param>
D2D_COLOR_F Color::ARGBToD2D(ARGB argb) {
  D2D_COLOR_F ret;
  ret.a = (argb >> 24) / 255.0f;
  ret.r = (argb >> 16 & 0xFF) / 255.0f;
  ret.g = (argb >> 8 & 0xFF) / 255.0f;
  ret.b = (argb & 0xFF) / 255.0f;
  return ret;
}


/// <summary>
/// Converts a D2D_COLOR_F formatted color to the ARGB format.
/// </summary>
/// <param name="d2d">The color to convert.</param>
ARGB Color::D2DToARGB(D2D_COLOR_F d2d) {
  return ARGBToARGB(ARGB(d2d.a * 255), ARGB(d2d.r * 255), ARGB(d2d.g * 255), ARGB(d2d.b * 255));
}


/// <summary>
/// Retrives the ARGB value of a named color.
/// </summary>
bool Color::GetNamedColor(LPCTSTR name, LPARGB color) {
  ColorMap::const_iterator iter = namedColors.find(name);
  if (iter != namedColors.end()) {
    *color = iter->second;
    return true;
  }
  return false;
}


/// <summary>
/// Converts red, green, blue values to ARGB.
/// </summary>
ARGB Color::RGBToARGB(int red, int green, int blue) {
  return ARGBToARGB(0xFF, red, green, blue);
}


/// <summary>
/// Converts ARGB values to an ARGB.
/// </summary>
ARGB Color::ARGBToARGB(int alpha, int red, int green, int blue) {
  return alpha << 24 | red << 16 | green << 8 | blue;
}


/// <summary>
/// Converts ARGB float values to an ARGB.
/// </summary>
ARGB Color::ARGBfToARGB(int alpha, float red, float green, float blue) {
  return ARGBToARGB(alpha, (int)floor(red + 0.5f), (int)floor(green + 0.5f), (int)floor(blue + 0.5f));
}


/// <summary>
/// Converts hue, saturation, and lightness to an ARGB.
/// </summary>
ARGB Color::HSLToARGB(int hue, float saturation, float lightness) {
  return AHSLToARGB(0xFF, hue, saturation, lightness);
}


/// <summary>
/// Converts an AHSL to an ARGB.
/// </summary>
ARGB Color::AHSLToARGB(AHSL color) {
  return AHSLToARGB(color.alpha, color.hue, color.saturation, color.lightness);
}


/// <summary>
/// Converts alpha, hue, saturation, and lightness to an ARGB.
/// </summary>
ARGB Color::AHSLToARGB(int alpha, int hue, float saturation, float lightness) {
  // Normalize the input
  float nSaturation = saturation / COLOR_MAX_SATURATION;
  float nLightness = lightness / COLOR_MAX_LIGHTNESS;

  float h = hue / 60.0f;
  float chroma = nSaturation * (1 - fabs(2 * nLightness - 1));
  float m = nLightness - chroma / 2.0f;
  float x = chroma*(1.0f - fabs(fmodf(h, 2.0f) - 1.0f));

  switch (int(h)) {
  case 0: return ARGBfToARGB(alpha, (chroma + m) * 255, (x + m) * 255, m * 255);
  case 1: return ARGBfToARGB(alpha, (x + m) * 255, (chroma + m) * 255, m * 255);
  case 2: return ARGBfToARGB(alpha, m * 255, (chroma + m) * 255, (x + m) * 255);
  case 3: return ARGBfToARGB(alpha, m * 255, (x + m) * 255, (chroma + m) * 255);
  case 4: return ARGBfToARGB(alpha, (x + m) * 255, m * 255, (chroma + m) * 255);
  case 5: return ARGBfToARGB(alpha, (chroma + m) * 255, m * 255, (x + m) * 255);
  }

  return ARGBToARGB(alpha, 0, 0, 0);
}


ARGB Color::HSVToARGB(int hue, int saturation, int value) {
  return AHSVToARGB(0xFF, hue, saturation, value);
}


ARGB Color::AHSVToARGB(int alpha, int hue, int saturation, int value) {
  // Normalize the input
  float nSaturation = float(saturation) / COLOR_MAX_SATURATION;
  float nValue = float(value) / COLOR_MAX_VALUE;

  float h = hue / 60.0f;
  float chroma = nValue * nSaturation;
  float m = nValue - chroma;
  float x = chroma*(1.0f - fabs(fmodf(h, 2.0f) - 1.0f));

  switch ((int)h) {
  case 0: return ARGBToARGB(alpha, ARGB((chroma + m) * 255), ARGB((x + m) * 255), ARGB(m * 255));
  case 1: return ARGBToARGB(alpha, ARGB((x + m) * 255), ARGB((chroma + m) * 255), ARGB(m * 255));
  case 2: return ARGBToARGB(alpha, ARGB(m * 255), ARGB((chroma + m) * 255), ARGB((x + m) * 255));
  case 3: return ARGBToARGB(alpha, ARGB(m * 255), ARGB((x + m) * 255), ARGB((chroma + m) * 255));
  case 4: return ARGBToARGB(alpha, ARGB((x + m) * 255), ARGB(m * 255), ARGB((chroma + m) * 255));
  case 5: return ARGBToARGB(alpha, ARGB((chroma + m) * 255), ARGB(m * 255), ARGB((x + m) * 255));
  }

  return ARGBToARGB(alpha, 0, 0, 0);
}


AHSL Color::ARGBToAHSL(ARGB color) {
  AHSL ret;
  D2D_COLOR_F c = ARGBToD2D(color);

  ret.alpha = color >> 24;

  float M = std::max(c.r, std::max(c.g, c.b));
  float m = std::min(c.r, std::min(c.g, c.b));
  float C = M - m;

  if (C == 0) {
    ret.hue = 0;
  } else if (M == c.r) {
    ret.hue = int(60.0f*(fmodf((c.g - c.b) / C, 6.0f)));
  } else if (M == c.g) {
    ret.hue = int(60.0f*((c.b - c.r) / C + 2.0f));
  } else {
    ret.hue = int(60.0f*((c.r - c.g) / C + 4.0f));
  }

  if (ret.hue < 0) {
    ret.hue += 360;
  }

  float L = (M + m) / 2.0f;

  ret.lightness = COLOR_MAX_LIGHTNESS*L;
  ret.saturation = COLOR_MAX_SATURATION*C / (1.0f - fabs(2.0f*L - 1.0f));

  return ret;
}


AHSV Color::ARGBToAHSV(ARGB color) {
  AHSV ret;
  D2D_COLOR_F c = ARGBToD2D(color);

  ret.alpha = color >> 24;

  float M = std::max(c.r, std::max(c.g, c.b));
  float m = std::min(c.r, std::min(c.g, c.b));
  float C = M - m;

  if (C == 0) {
    ret.hue = 0;
  } else if (M == c.r) {
    ret.hue = int(60.0f*(fmodf((c.g - c.b) / C, 6.0f)));
  } else if (M == c.g) {
    ret.hue = int(60.0f*((c.b - c.r) / C + 2.0f));
  } else {
    ret.hue = int(60.0f*((c.r - c.g) / C + 4.0f));
  }

  if (ret.hue < 0) {
    ret.hue += 360;
  }

  ret.value = UCHAR(COLOR_MAX_VALUE*M);
  ret.saturation = UCHAR(COLOR_MAX_SATURATION*C / M);

  return ret;
}


ARGB Color::Mix(ARGB color1, ARGB color2, float weight) {
  AHSL HSLcolor1 = ARGBToAHSL(color1);
  AHSL HSLcolor2 = ARGBToAHSL(color2);
  AHSL mix;
  mix.alpha = int(Lerp(float(HSLcolor1.alpha), float(HSLcolor2.alpha), weight) + 0.5f);
  mix.lightness = Lerp(HSLcolor1.lightness, HSLcolor2.lightness, weight);
  mix.saturation = Lerp(HSLcolor1.saturation, HSLcolor2.saturation, weight);
  mix.hue = int(WrappingLerp(float(HSLcolor1.hue), float(HSLcolor2.hue), weight, 0, COLOR_MAX_HUE));

  return AHSLToARGB(mix);
}


IColorVal *Color::Parse(LPCTSTR colorString, const IColorVal* defaultValue) {
  IColorVal *color;
  if (ParseColor(colorString, &color)) {
    return color;
  }
  return defaultValue->Copy();
}


std::unique_ptr<IColorVal> Color::Create(ARGB value) {
  return std::unique_ptr<IColorVal>(new LiteralColorVal(value));
}
