/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  ColorParser.cpp
 *  The nModules Project
 *
 *  Does the String -> ColorVal conversion.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "BinaryColorVal.hpp"
#include "DWMColorVal.hpp"
#include "IColorVal.hpp"
#include "LiteStep.h"
#include "LiteralColorVal.hpp"
#include "UnaryColorVal.hpp"

#include "../Utilities/Math.h"

#include <strsafe.h>


// Literal Color functions
static struct {
  LPCWSTR funcName;
  UCHAR numParams;
  USHORT paramLimits[4];
  ARGB(*func)(LPINT);
} gLiteralFunctions[] = {
  { L"RGB", 3, { 255, 255, 255 }, [] (LPINT params) -> ARGB {
    return Color::RGBToARGB(params[0], params[1], params[2]);
  } },
  { L"HSL", 3, { COLOR_MAX_HUE, COLOR_MAX_SATURATION, COLOR_MAX_LIGHTNESS }, [] (LPINT params) -> ARGB {
    return Color::HSLToARGB(params[0], float(params[1]), float(params[2]));
  } },
  { L"HSV", 3, { COLOR_MAX_HUE, COLOR_MAX_SATURATION, COLOR_MAX_VALUE }, [] (LPINT params) -> ARGB {
    return Color::HSVToARGB(params[0], params[1], params[2]);
  } },
  { L"RGBA", 4, { 255, 255, 255, 255 }, [] (LPINT params) -> ARGB {
    return Color::ARGBToARGB(params[3], params[0], params[1], params[2]);
  } },
  { L"HSLA", 4, { COLOR_MAX_HUE, COLOR_MAX_SATURATION, COLOR_MAX_LIGHTNESS, 255 }, [] (LPINT params) -> ARGB {
    return Color::AHSLToARGB(params[3], params[0], float(params[1]), float(params[2]));
  } },
  { L"HSVA", 4, { COLOR_MAX_HUE, COLOR_MAX_SATURATION, COLOR_MAX_VALUE, 255 }, [] (LPINT params) -> ARGB {
    return Color::AHSVToARGB(params[3], params[0], params[1], params[2]);
  } },

  // These are deprecated, for the sake of consistency, and should not be added to the
  // documentation. Still here for legacy support though.
  { L"ARGB", 4, { 255, 255, 255, 255 }, [] (LPINT params) -> ARGB {
    return Color::ARGBToARGB(params[0], params[1], params[2], params[3]);
  } },
  { L"AHSL", 4, { COLOR_MAX_HUE, COLOR_MAX_SATURATION, COLOR_MAX_LIGHTNESS, 255 }, [] (LPINT params) -> ARGB {
    return Color::AHSLToARGB(params[0], params[1], float(params[2]), float(params[3]));
  } },
  { L"AHSV", 4, { COLOR_MAX_HUE, COLOR_MAX_SATURATION, COLOR_MAX_VALUE, 255 }, [] (LPINT params) -> ARGB {
    return Color::AHSVToARGB(params[0], params[1], params[2], params[3]);
  } }
};


// Unary color functions
static struct {
  LPCTSTR funcName;
  ARGB(*func)(ARGB, long);
} gUnaryFunctions[] = {
  { L"Lighten", [] (ARGB color, long value) -> ARGB {
    AHSL hslColor = Color::ARGBToAHSL(color);
    hslColor.lightness = Clamp(hslColor.lightness + value, 0.0f, (float)COLOR_MAX_LIGHTNESS);
    return Color::AHSLToARGB(hslColor);
  } },
  { L"Darken", [] (ARGB color, long value) -> ARGB {
    AHSL hslColor = Color::ARGBToAHSL(color);
    hslColor.lightness = Clamp(hslColor.lightness - value, 0.0f, (float)COLOR_MAX_LIGHTNESS);
    return Color::AHSLToARGB(hslColor);
  } },
  { L"SetLightness", [] (ARGB color, long value) -> ARGB {
    AHSL hslColor = Color::ARGBToAHSL(color);
    hslColor.lightness = Clamp((float)value, 0.0f, (float)COLOR_MAX_LIGHTNESS);
    return Color::AHSLToARGB(hslColor);
  } },
  { L"Saturate", [] (ARGB color, long value) -> ARGB {
    AHSL hslColor = Color::ARGBToAHSL(color);
    hslColor.saturation = Clamp(hslColor.saturation + value, 0.0f, (float)COLOR_MAX_SATURATION);
    return Color::AHSLToARGB(hslColor);
  } },
  { L"Desaturate", [] (ARGB color, long value) -> ARGB {
    AHSL hslColor = Color::ARGBToAHSL(color);
    hslColor.saturation = Clamp(hslColor.saturation - value, 0.0f, (float)COLOR_MAX_SATURATION);
    return Color::AHSLToARGB(hslColor);
  } },
  { L"SetSaturation", [] (ARGB color, long value) -> ARGB {
    AHSL hslColor = Color::ARGBToAHSL(color);
    hslColor.saturation = Clamp((float)value, 0.0f, (float)COLOR_MAX_SATURATION);
    return Color::AHSLToARGB(hslColor);
  } },
  { L"Fadein", [] (ARGB color, long value) -> ARGB {
    // Could use bitops exclusively, but iffy when value is out of range.
    return Clamp((color >> 24) + value, 0, 0xFF) << 24 | color & 0xFFFFFF;
  } },
  { L"FadeOut", [] (ARGB color, long value) -> ARGB {
    return Clamp((color >> 24) - value, 0, 0xFF) << 24 | color & 0xFFFFFF;
  } },
  { L"SetAlpha", [] (ARGB color, long value) -> ARGB {
    return Clamp(value, 0, 0xFF) << 24 | color & 0xFFFFFF;
  } },
  { L"Spin", [] (ARGB color, long value) -> ARGB {
    AHSL hslColor = Color::ARGBToAHSL(color);
    hslColor.hue += value;
    hslColor.hue %= 360;
    if (hslColor.hue < 0) {
      hslColor.hue += 360;
    }
    return Color::AHSLToARGB(hslColor);
  } },
  { L"SetHue", [] (ARGB color, long value) -> ARGB {
    AHSL hslColor = Color::ARGBToAHSL(color);
    hslColor.hue = value;
    hslColor.hue %= 360;
    if (hslColor.hue < 0) {
      hslColor.hue += 360;
    }
    return Color::AHSLToARGB(hslColor);
  } }
};


// Binary color functions
static struct {
  LPCWSTR funcName;
  ARGB (*func)(ARGB, ARGB, float);
} gBinaryFunctions [] = {
  { L"Mix", Color::Mix }
};


/// <summary>
/// Determines if the specified string is a function with the specified name.
/// </summary>
static inline bool _IsFunctionOf(LPCWSTR source, LPCWSTR name) {
  return _wcsnicmp(name, source, wcslen(name)) == 0 && source[wcslen(name)] == L'(' &&
      source[wcslen(source) - 1] == L')';
}


/// <summary>
/// Grabs the first maxParams parameters from the function contained in the given string.
/// </summary>
/// <returns>The number of parameters actually retrived.</returns>
static int _GetParameters(LPCWSTR source, UCHAR maxParams, LPWSTR dests[], size_t cchDest) {
  int currentParam = 0;
  int parenDepth = 0;

  LPCWSTR pos = wcschr(source, L'(');
  LPCWSTR paramStart = pos + 1;

  bool done = false;
  while (!done && currentParam < maxParams) {
    switch (*++pos) {
    case L'\0':
      {
        done = true;
      }
      break;

    case L'(':
      {
        ++parenDepth;
      }
      break;

    case L',':
      {
        if (parenDepth == 0) {
          StringCchCopyN(dests[currentParam++], cchDest, paramStart, pos - paramStart);
          // Drop whitespace between parameters
          for (; *(pos + 1) == L' ' || *(pos + 1) == L'\t'; ++pos);
          paramStart = pos + 1;
        }
      }
      break;

    case L')':
      {
        if (parenDepth == 0) {
          StringCchCopyN(dests[currentParam++], cchDest, paramStart, pos - paramStart);
          done = true;
        } else {
          --parenDepth;
        }
      }
      break;
    }
  }

  return currentParam;
}


/// <summary>
/// Retrieves up to maxParams number of parameters from source, storing them into out.
/// </summary>
/// <returns>The number of parameters actually retrieved.</returns>
static int _GetParametersAsInts(LPCWSTR source, UCHAR maxParams, LPINT out) {
  WCHAR val1[8], val2[8], val3[8], val4[8];
  LPWSTR params[] = { val1, val2, val3, val4 };
  LPWSTR endPtr;

  int numParams = _GetParameters(source, maxParams, params, 8);

  for (int i = 0; i < numParams; ++i) {
    out[i] = wcstoul(params[i], &endPtr, 0);

    if (*endPtr != L'\0') {
      return i;
    }
  }

  return numParams;
}


/// <summary>
/// Extracts an IColorVal and an amount from source.
/// </summary>
static bool _GetColorAndAmount(LPCWSTR source, IColorVal **color, LPLONG amount) {
  bool ParseColor(LPCWSTR color, IColorVal **target);

  WCHAR val1[MAX_LINE_LENGTH], val2[MAX_LINE_LENGTH];
  LPWSTR params[] = { val1, val2 };
  LPWSTR endPtr;

  if (_GetParameters(source, 2, params, MAX_LINE_LENGTH) != 2) {
    return false;
  }

  *amount = wcstol(val2, &endPtr, 0);
  if (*val2 == L'\0' || *endPtr != L'\0') {
    return false;
  }

  return ParseColor(val1, color);
}


/// <summary>
/// Parses a string to to a colorval.
/// </summary>
/// <returns>True if the parsing succeeded.</returns>
bool ParseColor(LPCTSTR color, IColorVal **target) {
  // This happens a lot, might as well quit early.
  if (*color == L'\0') {
    return false;
  }

  // Try to parse the color as a hex number
  if (color[0] == L'#') {
    LPTSTR endPtr;
    ARGB hexValue = wcstoul(color + 1, &endPtr, 16);
    if (*endPtr != L'\0') {
      return false;
    }

    ARGB colorValue;
    size_t length = wcslen(color);
    switch (length) {
    case 4: // #RGB
    case 5: // #ARGB
      colorValue = length == 4 ? 0xFF000000 :
          ((0xF000 & hexValue) << 16 | (0xF000 & hexValue) << 12);
      colorValue |= (0xF00 & hexValue) << 12 | (0xF00 & hexValue) << 8;
      colorValue |= (0xF0 & hexValue) << 8 | (0xF0 & hexValue) << 4;
      colorValue |= (0xF & hexValue) << 4 | (0xF & hexValue);
      break;

    case 7: // #RRGGBB
      colorValue = 0xFF000000 | hexValue;
      break;

    case 9: // #AARRGGBB
      colorValue = hexValue;
      break;

    default:
      return false;
    }

    *target = new LiteralColorVal(colorValue);

    return true;
  }

  // Check if it is a literal color function.
  for (auto literal : gLiteralFunctions) {
    if (_IsFunctionOf(color, literal.funcName)) {
      int parameters[4];
      if (_GetParametersAsInts(color, literal.numParams, parameters) == literal.numParams) {
        *target = new LiteralColorVal(literal.func(parameters));
        return true;
      }
      return false;
    }
  }

  // Check if it is a unary color function.
  for (auto unary : gUnaryFunctions) {
    if (_IsFunctionOf(color, unary.funcName)) {
      IColorVal *colorVal;
      long amount;

      if (_GetColorAndAmount(color, &colorVal, &amount)) {
        if (colorVal->IsConstant()) {
          *target = new LiteralColorVal(unary.func(colorVal->Evaluate(), amount));
          delete colorVal;
        } else {
          *target = new UnaryColorVal(unary.func, colorVal, amount);
        }
        return true;
      }
      return false;
    }
  }

  // Check if it is a binary color function.
  for (auto binary : gBinaryFunctions) {
    if (_IsFunctionOf(color, binary.funcName)) {
      TCHAR val1[MAX_LINE_LENGTH], val2[MAX_LINE_LENGTH], val3[MAX_LINE_LENGTH];
      LPTSTR params[] = { val1, val2, val3 };
      LPTSTR endPtr;

      IColorVal *color1, *color2;

      if (_GetParameters(color, 3, params, MAX_LINE_LENGTH) != 3) {
        return false;
      }

      float value = wcstof(val3, &endPtr);

      if (*endPtr != '\0') {
        return false;
      }

      if (!ParseColor(val1, &color1)) {
        return false;
      }

      if (!ParseColor(val2, &color2)) {
        delete color1;
        return false;
      }

      if (color1->IsConstant() && color2->IsConstant()) {
        *target = new LiteralColorVal(binary.func(color1->Evaluate(), color2->Evaluate(), value));
      } else {
        *target = new BinaryColorVal(binary.func, color1, color2, value);
      }

      return true;
    }
  }

  // Check if it is the DWM color
  if (_wcsicmp(color, L"DWMColor") == 0) {
    *target = new DWMColorVal();
    return true;
  }

  // Check if it's a named color
  ARGB argb;
  if (Color::GetNamedColor(color, &argb)) {
    *target = new LiteralColorVal(argb);
    return true;
  }

  return false;
}
