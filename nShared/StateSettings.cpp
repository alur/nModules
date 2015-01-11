/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  StateSettings.cpp
 *  The nModules Project
 *
 *  Holds all RC settings used by the State class.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../nShared/LiteStep.h"
#include <strsafe.h>
#include "State.hpp"
#include "../Utilities/Unordered1To1Map.hpp"
#include "../Utilities/StringUtils.h"

template<typename SettingType>
using SettingMap = const Unordered1To1Map<
    SettingType,
    LPCTSTR,
    std::hash<SettingType>,
    CaseInsensitive::Hash,
    std::equal_to<SettingType>,
    CaseInsensitive::Equal
>;


static SettingMap<DWRITE_READING_DIRECTION> readingDirectionMap(
{
    { DWRITE_READING_DIRECTION_LEFT_TO_RIGHT, L"LeftToRight" },
    { DWRITE_READING_DIRECTION_RIGHT_TO_LEFT, L"RightToLeft" }
});

static SettingMap<DWRITE_WORD_WRAPPING> wordWrappingMap(
{
    { DWRITE_WORD_WRAPPING_NO_WRAP, L"NoWrap" },
    { DWRITE_WORD_WRAPPING_WRAP,    L"Wrap"   }
});

static SettingMap<DWRITE_FONT_STYLE> fontStyleMap(
{
    { DWRITE_FONT_STYLE_NORMAL,  L"Normal"     },
    { DWRITE_FONT_STYLE_OBLIQUE, L"Oblique"    },
    { DWRITE_FONT_STYLE_ITALIC,  L"Italic"     }
});

static SettingMap<DWRITE_TEXT_ALIGNMENT> textAlignmentMap(
{
    { DWRITE_TEXT_ALIGNMENT_LEADING,  L"Left"   },
    { DWRITE_TEXT_ALIGNMENT_CENTER,   L"Center" },
    { DWRITE_TEXT_ALIGNMENT_TRAILING, L"Right"  }
});

static SettingMap<DWRITE_PARAGRAPH_ALIGNMENT> paragraphAlignmentMap(
{
    { DWRITE_PARAGRAPH_ALIGNMENT_NEAR,     L"Top"     },
    { DWRITE_PARAGRAPH_ALIGNMENT_CENTER,   L"Middle"  },
    { DWRITE_PARAGRAPH_ALIGNMENT_FAR,      L"Bottom"  }
});

static SettingMap<DWRITE_TRIMMING_GRANULARITY> trimmingGranularityMap(
{
    { DWRITE_TRIMMING_GRANULARITY_CHARACTER,    L"Character" },
    { DWRITE_TRIMMING_GRANULARITY_WORD,         L"Word"      },
    { DWRITE_TRIMMING_GRANULARITY_NONE,         L"None"      }
});

static SettingMap<DWRITE_FONT_STRETCH> fontStretchMap(
{
    { DWRITE_FONT_STRETCH_NORMAL,           L"Normal"          },
    { DWRITE_FONT_STRETCH_ULTRA_CONDENSED,  L"Ultra Condensed" },
    { DWRITE_FONT_STRETCH_EXTRA_CONDENSED,  L"Extra Condensed" },
    { DWRITE_FONT_STRETCH_CONDENSED,        L"Condensed"       },
    { DWRITE_FONT_STRETCH_SEMI_CONDENSED,   L"Semi Condensed"  },
    { DWRITE_FONT_STRETCH_MEDIUM,           L"Medium"          },
    { DWRITE_FONT_STRETCH_SEMI_EXPANDED,    L"Semi Expanded"   },
    { DWRITE_FONT_STRETCH_EXPANDED,         L"Expanded"        },
    { DWRITE_FONT_STRETCH_EXTRA_EXPANDED,   L"Extra Expanded"  },
    { DWRITE_FONT_STRETCH_ULTRA_EXPANDED,   L"Ultra Expanded"  }
});

static SettingMap<DWRITE_FONT_WEIGHT> fontWeightMap(
{
    { DWRITE_FONT_WEIGHT_THIN,         L"Thin"         },
    { DWRITE_FONT_WEIGHT_EXTRA_LIGHT,  L"Extra Light"  },
    { DWRITE_FONT_WEIGHT_ULTRA_LIGHT,  L"Ultra Light"  },
    { DWRITE_FONT_WEIGHT_LIGHT,        L"Light"        },
    { DWRITE_FONT_WEIGHT_SEMI_LIGHT,   L"Semi Light"   },
    { DWRITE_FONT_WEIGHT_REGULAR,      L"Regular"      },
    { DWRITE_FONT_WEIGHT_MEDIUM,       L"Medium"       },
    { DWRITE_FONT_WEIGHT_SEMI_BOLD,    L"Semi Bold"    },
    { DWRITE_FONT_WEIGHT_BOLD,         L"Bold"         },
    { DWRITE_FONT_WEIGHT_EXTRA_BOLD,   L"Extra Bold"   },
    { DWRITE_FONT_WEIGHT_ULTRA_BOLD,   L"Ultra Bold"   },
    { DWRITE_FONT_WEIGHT_BLACK,        L"Black"        },
    { DWRITE_FONT_WEIGHT_HEAVY,        L"Heavy"        },
    { DWRITE_FONT_WEIGHT_EXTRA_BLACK,  L"Extra Black"  },
    { DWRITE_FONT_WEIGHT_ULTRA_BLACK,  L"Ultra Black"  }
});


/// <summary>
/// Initalizes the class to all default settings.
/// </summary>
State::Settings::Settings()
    : cornerRadiusX(0.0f)
    , cornerRadiusY(0.0f)
    , fontSize(12.0f)
    , fontStretch(DWRITE_FONT_STRETCH_NORMAL)
    , fontStrokeWidth(0.0f)
    , fontStyle(DWRITE_FONT_STYLE_NORMAL)
    , fontWeight(DWRITE_FONT_WEIGHT_NORMAL)
    , outlineWidth(0.0f)
    , readingDirection(DWRITE_READING_DIRECTION_LEFT_TO_RIGHT)
    , textAlign(DWRITE_TEXT_ALIGNMENT_LEADING)
    , textOffsetBottom(0.0f)
    , textOffsetLeft(0.0f)
    , textOffsetRight(0.0f)
    , textOffsetTop(0.0f)
    , textRotation(0.0f)
    , textTrimmingGranularity(DWRITE_TRIMMING_GRANULARITY_CHARACTER)
    , textVerticalAlign(DWRITE_PARAGRAPH_ALIGNMENT_NEAR)
    , wordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP)
{
    StringCchCopy(this->font, _countof(this->font), L"Arial");
    brushSettings[State::BrushType::Text].color = Color::Create(0xFFFFFFFF);
}


/// <summary>
/// Loads settings from an RC file using the specified defaults.
/// </summary>
void State::Settings::Load(const ::Settings * settings, const Settings * defaults)
{
    TCHAR buffer[MAX_PATH];
    if (!defaults)
    {
        defaults = this;
    }

    this->cornerRadiusX = settings->GetFloat(L"CornerRadiusX", defaults->cornerRadiusX);
    this->cornerRadiusY = settings->GetFloat(L"CornerRadiusY", defaults->cornerRadiusY);
    settings->GetString(L"Font", buffer, _countof(buffer), defaults->font);
    StringCchCopy(this->font, _countof(this->font), buffer);
    this->fontSize = settings->GetFloat(L"FontSize", defaults->fontSize);

    settings->GetString(L"FontStretch", buffer, _countof(buffer), fontStretchMap.GetByA(defaults->fontStretch, L"Normal"));
    this->fontStretch = ParseFontStretch(buffer);

    settings->GetString(L"FontStyle", buffer, _countof(buffer), fontStyleMap.GetByA(defaults->fontStyle, L"Normal"));
    this->fontStyle = ParseFontStyle(buffer);

    settings->GetString(L"FontWeight", buffer, _countof(buffer), fontWeightMap.GetByA(defaults->fontWeight, L"Regular"));
    this->fontWeight = ParseFontWeight(buffer);

    this->outlineWidth = settings->GetFloat(L"OutlineWidth", defaults->outlineWidth);

    settings->GetString(L"ReadingDirection", buffer, _countof(buffer), readingDirectionMap.GetByA(defaults->readingDirection, L"LeftToRight"));
    this->readingDirection = ParseReadingDirection(buffer);

    settings->GetString(L"TextAlign", buffer, _countof(buffer), textAlignmentMap.GetByA(defaults->textAlign, L"Left"));
    this->textAlign = ParseTextAlignment(buffer);

    this->textOffsetBottom = settings->GetFloat(L"TextOffsetBottom", defaults->textOffsetBottom);
    this->textOffsetLeft = settings->GetFloat(L"TextOffsetLeft", defaults->textOffsetLeft);
    this->textOffsetRight = settings->GetFloat(L"TextOffsetRight", defaults->textOffsetRight);
    this->textOffsetTop = settings->GetFloat(L"TextOffsetTop", defaults->textOffsetTop);
    this->textRotation = settings->GetFloat(L"TextRotation", defaults->textRotation);

    this->fontStrokeWidth = settings->GetFloat(L"FontStrokeWidth", defaults->fontStrokeWidth);
    
    settings->GetString(L"TextTrimmingGranularity", buffer, _countof(buffer), trimmingGranularityMap.GetByA(defaults->textTrimmingGranularity, L"Character"));
    this->textTrimmingGranularity = ParseTrimmingGranularity(buffer);
    
    settings->GetString(L"TextVerticalAlign", buffer, _countof(buffer), paragraphAlignmentMap.GetByA(defaults->textVerticalAlign, L"Top"));
    this->textVerticalAlign = ParseParagraphAlignment(buffer);

    settings->GetString(L"WordWrapping", buffer, _countof(buffer), wordWrappingMap.GetByA(defaults->wordWrapping, L"NoWrap"));
    this->wordWrapping = ParseWordWrapping(buffer);

    // Load brushes
    this->brushSettings[State::BrushType::Background].Load(settings, &defaults->brushSettings[State::BrushType::Background]);
    
    ::Settings* outlineSettings = settings->CreateChild(L"Outline");
    this->brushSettings[State::BrushType::Outline].Load(outlineSettings, &defaults->brushSettings[State::BrushType::Outline]);

    ::Settings* textSettings = settings->CreateChild(L"Font");
    this->brushSettings[State::BrushType::Text].Load(textSettings, &defaults->brushSettings[State::BrushType::Text]);

    ::Settings* textOutlineSettings = textSettings->CreateChild(L"Stroke");
    this->brushSettings[State::BrushType::TextStroke].Load(textOutlineSettings, &defaults->brushSettings[State::BrushType::TextStroke]);
    
    delete outlineSettings;
    delete textSettings;
    delete textOutlineSettings;
}


DWRITE_FONT_STRETCH State::Settings::ParseFontStretch(LPCTSTR fontStretch)
{
    return fontStretchMap.GetByB(fontStretch, DWRITE_FONT_STRETCH_NORMAL);
}


DWRITE_FONT_STYLE State::Settings::ParseFontStyle(LPCTSTR fontStyle)
{
    return fontStyleMap.GetByB(fontStyle, DWRITE_FONT_STYLE_NORMAL);
}


DWRITE_FONT_WEIGHT State::Settings::ParseFontWeight(LPCTSTR weight)
{
    return fontWeightMap.GetByB(weight, DWRITE_FONT_WEIGHT_NORMAL);
}


DWRITE_TEXT_ALIGNMENT State::Settings::ParseTextAlignment(LPCTSTR textAlignment)
{
    return textAlignmentMap.GetByB(textAlignment, DWRITE_TEXT_ALIGNMENT_LEADING);
}


DWRITE_PARAGRAPH_ALIGNMENT State::Settings::ParseParagraphAlignment(LPCTSTR paragraphAlignment)
{
    return paragraphAlignmentMap.GetByB(paragraphAlignment, DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
}


DWRITE_TRIMMING_GRANULARITY State::Settings::ParseTrimmingGranularity(LPCTSTR trimmingGranularity)
{
    return trimmingGranularityMap.GetByB(trimmingGranularity, DWRITE_TRIMMING_GRANULARITY_CHARACTER);
}


DWRITE_READING_DIRECTION State::Settings::ParseReadingDirection(LPCTSTR readingDirection)
{
    return readingDirectionMap.GetByB(readingDirection, DWRITE_READING_DIRECTION_LEFT_TO_RIGHT);
}


DWRITE_WORD_WRAPPING State::Settings::ParseWordWrapping(LPCTSTR wordWrapping)
{
    return wordWrappingMap.GetByB(wordWrapping, DWRITE_WORD_WRAPPING_NO_WRAP);
}
