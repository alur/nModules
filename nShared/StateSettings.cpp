/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  DrawableSettings.cpp
 *  The nModules Project
 *
 *  Holds all RC settings used by the DrawableWindow class.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../nShared/LiteStep.h"
#include <strsafe.h>
#include "StateSettings.hpp"
#include <map>


/// <summary>
/// Initalizes the class to all default settings.
/// </summary>
StateSettings::StateSettings() {
    this->cornerRadiusX = 0.0f;
    this->cornerRadiusY = 0.0f;
    StringCchCopyW(this->font, sizeof(this->font)/sizeof(WCHAR), L"Arial");
    this->fontSize = 12.0f;
    this->fontStretch = DWRITE_FONT_STRETCH_NORMAL;
    this->fontStyle = DWRITE_FONT_STYLE_NORMAL;
    this->fontWeight = DWRITE_FONT_WEIGHT_NORMAL;
    this->outlineWidth = 0.0f;
    this->readingDirection = DWRITE_READING_DIRECTION_LEFT_TO_RIGHT;
    this->textAlign = DWRITE_TEXT_ALIGNMENT_LEADING;
    this->textOffsetBottom = 0.0f;
    this->textOffsetLeft = 0.0f;
    this->textOffsetRight = 0.0f;
    this->textOffsetTop = 0.0f;
    this->textRotation = 0.0f;
    this->textTrimmingGranularity = DWRITE_TRIMMING_GRANULARITY_CHARACTER;
    this->textVerticalAlign = DWRITE_PARAGRAPH_ALIGNMENT_NEAR;
    this->wordWrapping = DWRITE_WORD_WRAPPING_NO_WRAP;

    this->backgroundBrush.color = 0xFF000000;
    this->textBrush.color = 0xFFFFFFFF;
    this->textDropShadowBrush.color = 0x00000000;
}


template <typename T>
static T GetValue(LPCSTR string, std::map<T, LPCSTR> map, T defValue) {
    for (auto &x : map) {
        if (_stricmp(string, x.second) == 0) {
            return x.first;
        }
    }
    return defValue;
}


template <typename T>
static LPCSTR GetName(T value, std::map<T, LPCSTR> map) {
    for (auto &x : map) {
        if (x.first == value) {
            return x.second;
        }
    }
    return "";
}


static std::map<DWRITE_READING_DIRECTION, LPCSTR> readingDirectionMap = {
    { DWRITE_READING_DIRECTION_LEFT_TO_RIGHT, "LeftToRight" },
    { DWRITE_READING_DIRECTION_RIGHT_TO_LEFT, "RightToLeft" }
};

static std::map<DWRITE_WORD_WRAPPING, LPCSTR> wordWrappingMap = {
    { DWRITE_WORD_WRAPPING_NO_WRAP, "NoWrap" },
    { DWRITE_WORD_WRAPPING_WRAP,    "Wrap"   }
};

static std::map<DWRITE_FONT_STYLE, LPCSTR> fontStyleMap = {
    { DWRITE_FONT_STYLE_NORMAL,  "Normal"     },
    { DWRITE_FONT_STYLE_OBLIQUE, "Oblique"    },
    { DWRITE_FONT_STYLE_ITALIC,  "Italic"     }
};

static std::map<DWRITE_TEXT_ALIGNMENT, LPCSTR> textAlignmentMap = {
    { DWRITE_TEXT_ALIGNMENT_LEADING,  "Left"   },
    { DWRITE_TEXT_ALIGNMENT_CENTER,   "Center" },
    { DWRITE_TEXT_ALIGNMENT_TRAILING, "Right"  }
};

static std::map<DWRITE_PARAGRAPH_ALIGNMENT, LPCSTR> paragraphAlignmentMap = {
    { DWRITE_PARAGRAPH_ALIGNMENT_NEAR,     "Top"     },
    { DWRITE_PARAGRAPH_ALIGNMENT_CENTER,   "Middle"  },
    { DWRITE_PARAGRAPH_ALIGNMENT_FAR,      "Bottom"  }
};

static std::map<DWRITE_TRIMMING_GRANULARITY, LPCSTR> trimmingGranularityMap = {
    { DWRITE_TRIMMING_GRANULARITY_CHARACTER,    "Character" },
    { DWRITE_TRIMMING_GRANULARITY_WORD,         "Word"      },
    { DWRITE_TRIMMING_GRANULARITY_NONE,         "None"      }
};

static std::map<DWRITE_FONT_STRETCH, LPCSTR> fontStretchMap = {
    { DWRITE_FONT_STRETCH_NORMAL,           "Normal"           },
    { DWRITE_FONT_STRETCH_ULTRA_CONDENSED,  "Ultra Condensed"  },
    { DWRITE_FONT_STRETCH_EXTRA_CONDENSED,  "Extra Condensed"  },
    { DWRITE_FONT_STRETCH_CONDENSED,        "Condensed"        },
    { DWRITE_FONT_STRETCH_SEMI_CONDENSED,   "Semi Condensed"   },
    { DWRITE_FONT_STRETCH_MEDIUM,           "Medium"           },
    { DWRITE_FONT_STRETCH_SEMI_EXPANDED,    "Semi Expanded"    },
    { DWRITE_FONT_STRETCH_EXPANDED,         "Expanded"         },
    { DWRITE_FONT_STRETCH_EXTRA_EXPANDED,   "Extra Expanded"   },
    { DWRITE_FONT_STRETCH_ULTRA_EXPANDED,   "Ultra Expanded"   }
};

static std::map<DWRITE_FONT_WEIGHT, LPCSTR> fontWeightMap = {
    { DWRITE_FONT_WEIGHT_THIN,         "Thin"         },
    { DWRITE_FONT_WEIGHT_EXTRA_LIGHT,  "Extra Light"  },
    { DWRITE_FONT_WEIGHT_ULTRA_LIGHT,  "Ultra Light"  },
    { DWRITE_FONT_WEIGHT_LIGHT,        "Light"        },
    { DWRITE_FONT_WEIGHT_SEMI_LIGHT,   "Semi Light"   },
    { DWRITE_FONT_WEIGHT_REGULAR,      "Regular"      },
    { DWRITE_FONT_WEIGHT_MEDIUM,       "Medium"       },
    { DWRITE_FONT_WEIGHT_SEMI_BOLD,    "Semi Bold"    },
    { DWRITE_FONT_WEIGHT_BOLD,         "Bold"         },
    { DWRITE_FONT_WEIGHT_EXTRA_BOLD,   "Extra Bold"   },
    { DWRITE_FONT_WEIGHT_ULTRA_BOLD,   "Ultra Bold"   },
    { DWRITE_FONT_WEIGHT_BLACK,        "Black"        },
    { DWRITE_FONT_WEIGHT_HEAVY,        "Heavy"        },
    { DWRITE_FONT_WEIGHT_EXTRA_BLACK,  "Extra Black"  },
    { DWRITE_FONT_WEIGHT_ULTRA_BLACK,  "Ultra Black"  }
};



/// <summary>
/// Loads settings from an RC file using the specified defaults.
/// </summary>
void StateSettings::Load(Settings* settings, StateSettings* defaults) {
    if (!defaults) {
        defaults = this;
    }
    char buffer[128];

    this->cornerRadiusX = settings->GetFloat("CornerRadiusX", defaults->cornerRadiusX);
    this->cornerRadiusY = settings->GetFloat("CornerRadiusY", defaults->cornerRadiusY);
    settings->GetString("Font", this->font, _countof(this->font), defaults->font);
    this->fontSize = settings->GetFloat("FontSize", defaults->fontSize);

    settings->GetString("FontStretch", buffer, _countof(buffer), GetName(defaults->fontStretch, fontStretchMap));
    this->fontStretch = ParseFontStretch(buffer);

    settings->GetString("FontStyle", buffer, _countof(buffer),  GetName(defaults->fontStyle, fontStyleMap));
    this->fontStyle = ParseFontStyle(buffer);

    settings->GetString("FontWeight",buffer, _countof(buffer), GetName(defaults->fontWeight, fontWeightMap));
    this->fontWeight = ParseFontWeight(buffer);

    this->outlineWidth = settings->GetFloat("OutlineWidth", defaults->outlineWidth);

    settings->GetString("ReadingDirection", buffer, _countof(buffer), GetName(defaults->readingDirection, readingDirectionMap));
    this->readingDirection = ParseReadingDirection(buffer);

    settings->GetString("TextAlign", buffer, _countof(buffer), GetName(defaults->textAlign, textAlignmentMap));
    this->textAlign = ParseTextAlignment(buffer);

    this->textOffsetBottom = settings->GetFloat("TextOffsetBottom", defaults->textOffsetBottom);
    this->textOffsetLeft = settings->GetFloat("TextOffsetLeft", defaults->textOffsetLeft);
    this->textOffsetRight = settings->GetFloat("TextOffsetRight", defaults->textOffsetRight);
    this->textOffsetTop = settings->GetFloat("TextOffsetTop", defaults->textOffsetTop);
    this->textRotation = settings->GetFloat("TextRotation", defaults->textRotation);
    
    settings->GetString("TextTrimmingGranularity", buffer, _countof(buffer), GetName(defaults->textTrimmingGranularity, trimmingGranularityMap));
    this->textTrimmingGranularity = ParseTrimmingGranularity(buffer);
    
    settings->GetString("TextVerticalAlign", buffer, _countof(buffer), GetName(defaults->textVerticalAlign, paragraphAlignmentMap));
    this->textVerticalAlign = ParseParagraphAlignment(buffer);

    settings->GetString("WordWrapping", buffer, _countof(buffer), GetName(defaults->wordWrapping, wordWrappingMap));
    this->wordWrapping = ParseWordWrapping(buffer);

    this->backgroundBrush.Load(settings, &defaults->backgroundBrush);
    
    Settings* outlineSettings = settings->CreateChild("Outline");
    this->outlineBrush.Load(outlineSettings, &defaults->outlineBrush);
    delete outlineSettings;

    Settings* textSettings = settings->CreateChild("Font");
    this->textBrush.Load(textSettings, &defaults->textBrush);
    delete textSettings;

    Settings* dropSettings = settings->CreateChild("TextDropShadow");
    this->textDropShadowBrush.Load(dropSettings, &defaults->textDropShadowBrush);
    delete dropSettings;
}


DWRITE_FONT_STRETCH StateSettings::ParseFontStretch(LPCSTR fontStretch) {
    return GetValue(fontStretch, fontStretchMap, DWRITE_FONT_STRETCH_NORMAL);
}


DWRITE_FONT_STYLE StateSettings::ParseFontStyle(LPCSTR fontStyle) {
    return GetValue(fontStyle, fontStyleMap, DWRITE_FONT_STYLE_NORMAL);
}


DWRITE_FONT_WEIGHT StateSettings::ParseFontWeight(LPCSTR weight) {
    return GetValue(weight, fontWeightMap, DWRITE_FONT_WEIGHT_NORMAL);
}


DWRITE_TEXT_ALIGNMENT StateSettings::ParseTextAlignment(LPCSTR textAlignment) {
    return GetValue(textAlignment, textAlignmentMap, DWRITE_TEXT_ALIGNMENT_LEADING);
}


DWRITE_PARAGRAPH_ALIGNMENT StateSettings::ParseParagraphAlignment(LPCSTR paragraphAlignment) {
    return GetValue(paragraphAlignment, paragraphAlignmentMap, DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
}


DWRITE_TRIMMING_GRANULARITY StateSettings::ParseTrimmingGranularity(LPCSTR trimmingGranularity) {
    return GetValue(trimmingGranularity, trimmingGranularityMap, DWRITE_TRIMMING_GRANULARITY_CHARACTER);
}


DWRITE_READING_DIRECTION StateSettings::ParseReadingDirection(LPCSTR readingDirection) {
    return GetValue(readingDirection, readingDirectionMap, DWRITE_READING_DIRECTION_LEFT_TO_RIGHT);
}


DWRITE_WORD_WRAPPING StateSettings::ParseWordWrapping(LPCSTR wordWrapping) {
    return GetValue(wordWrapping, wordWrappingMap, DWRITE_WORD_WRAPPING_NO_WRAP);
}
