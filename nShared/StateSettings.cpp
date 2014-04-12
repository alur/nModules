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
    { DWRITE_READING_DIRECTION_LEFT_TO_RIGHT, _T("LeftToRight") },
    { DWRITE_READING_DIRECTION_RIGHT_TO_LEFT, _T("RightToLeft") }
});

static SettingMap<DWRITE_WORD_WRAPPING> wordWrappingMap(
{
    { DWRITE_WORD_WRAPPING_NO_WRAP, _T("NoWrap") },
    { DWRITE_WORD_WRAPPING_WRAP,    _T("Wrap")   }
});

static SettingMap<DWRITE_FONT_STYLE> fontStyleMap(
{
    { DWRITE_FONT_STYLE_NORMAL,  _T("Normal")     },
    { DWRITE_FONT_STYLE_OBLIQUE, _T("Oblique")    },
    { DWRITE_FONT_STYLE_ITALIC,  _T("Italic")     }
});

static SettingMap<DWRITE_TEXT_ALIGNMENT> textAlignmentMap(
{
    { DWRITE_TEXT_ALIGNMENT_LEADING,  _T("Left")   },
    { DWRITE_TEXT_ALIGNMENT_CENTER,   _T("Center") },
    { DWRITE_TEXT_ALIGNMENT_TRAILING, _T("Right")  }
});

static SettingMap<DWRITE_PARAGRAPH_ALIGNMENT> paragraphAlignmentMap(
{
    { DWRITE_PARAGRAPH_ALIGNMENT_NEAR,     _T("Top")     },
    { DWRITE_PARAGRAPH_ALIGNMENT_CENTER,   _T("Middle")  },
    { DWRITE_PARAGRAPH_ALIGNMENT_FAR,      _T("Bottom")  }
});

static SettingMap<DWRITE_TRIMMING_GRANULARITY> trimmingGranularityMap(
{
    { DWRITE_TRIMMING_GRANULARITY_CHARACTER,    _T("Character") },
    { DWRITE_TRIMMING_GRANULARITY_WORD,         _T("Word")      },
    { DWRITE_TRIMMING_GRANULARITY_NONE,         _T("None")      }
});

static SettingMap<DWRITE_FONT_STRETCH> fontStretchMap(
{
    { DWRITE_FONT_STRETCH_NORMAL,           _T("Normal")          },
    { DWRITE_FONT_STRETCH_ULTRA_CONDENSED,  _T("Ultra Condensed") },
    { DWRITE_FONT_STRETCH_EXTRA_CONDENSED,  _T("Extra Condensed") },
    { DWRITE_FONT_STRETCH_CONDENSED,        _T("Condensed")       },
    { DWRITE_FONT_STRETCH_SEMI_CONDENSED,   _T("Semi Condensed")  },
    { DWRITE_FONT_STRETCH_MEDIUM,           _T("Medium")          },
    { DWRITE_FONT_STRETCH_SEMI_EXPANDED,    _T("Semi Expanded")   },
    { DWRITE_FONT_STRETCH_EXPANDED,         _T("Expanded")        },
    { DWRITE_FONT_STRETCH_EXTRA_EXPANDED,   _T("Extra Expanded")  },
    { DWRITE_FONT_STRETCH_ULTRA_EXPANDED,   _T("Ultra Expanded")  }
});

static SettingMap<DWRITE_FONT_WEIGHT> fontWeightMap(
{
    { DWRITE_FONT_WEIGHT_THIN,         _T("Thin")         },
    { DWRITE_FONT_WEIGHT_EXTRA_LIGHT,  _T("Extra Light")  },
    { DWRITE_FONT_WEIGHT_ULTRA_LIGHT,  _T("Ultra Light")  },
    { DWRITE_FONT_WEIGHT_LIGHT,        _T("Light")        },
    { DWRITE_FONT_WEIGHT_SEMI_LIGHT,   _T("Semi Light")   },
    { DWRITE_FONT_WEIGHT_REGULAR,      _T("Regular")      },
    { DWRITE_FONT_WEIGHT_MEDIUM,       _T("Medium")       },
    { DWRITE_FONT_WEIGHT_SEMI_BOLD,    _T("Semi Bold")    },
    { DWRITE_FONT_WEIGHT_BOLD,         _T("Bold")         },
    { DWRITE_FONT_WEIGHT_EXTRA_BOLD,   _T("Extra Bold")   },
    { DWRITE_FONT_WEIGHT_ULTRA_BOLD,   _T("Ultra Bold")   },
    { DWRITE_FONT_WEIGHT_BLACK,        _T("Black")        },
    { DWRITE_FONT_WEIGHT_HEAVY,        _T("Heavy")        },
    { DWRITE_FONT_WEIGHT_EXTRA_BLACK,  _T("Extra Black")  },
    { DWRITE_FONT_WEIGHT_ULTRA_BLACK,  _T("Ultra Black")  }
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
    StringCchCopy(this->font, _countof(this->font), _T("Arial"));
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

    this->cornerRadiusX = settings->GetFloat(_T("CornerRadiusX"), defaults->cornerRadiusX);
    this->cornerRadiusY = settings->GetFloat(_T("CornerRadiusY"), defaults->cornerRadiusY);
    settings->GetString(_T("Font"), buffer, _countof(buffer), defaults->font);
    StringCchCopy(this->font, _countof(this->font), buffer);
    this->fontSize = settings->GetFloat(_T("FontSize"), defaults->fontSize);

    settings->GetString(_T("FontStretch"), buffer, _countof(buffer), fontStretchMap.GetByA(defaults->fontStretch, _T("Normal")));
    this->fontStretch = ParseFontStretch(buffer);

    settings->GetString(_T("FontStyle"), buffer, _countof(buffer), fontStyleMap.GetByA(defaults->fontStyle, _T("Normal")));
    this->fontStyle = ParseFontStyle(buffer);

    settings->GetString(_T("FontWeight"), buffer, _countof(buffer), fontWeightMap.GetByA(defaults->fontWeight, _T("Regular")));
    this->fontWeight = ParseFontWeight(buffer);

    this->outlineWidth = settings->GetFloat(_T("OutlineWidth"), defaults->outlineWidth);

    settings->GetString(_T("ReadingDirection"), buffer, _countof(buffer), readingDirectionMap.GetByA(defaults->readingDirection, _T("LeftToRight")));
    this->readingDirection = ParseReadingDirection(buffer);

    settings->GetString(_T("TextAlign"), buffer, _countof(buffer), textAlignmentMap.GetByA(defaults->textAlign, _T("Left")));
    this->textAlign = ParseTextAlignment(buffer);

    this->textOffsetBottom = settings->GetFloat(_T("TextOffsetBottom"), defaults->textOffsetBottom);
    this->textOffsetLeft = settings->GetFloat(_T("TextOffsetLeft"), defaults->textOffsetLeft);
    this->textOffsetRight = settings->GetFloat(_T("TextOffsetRight"), defaults->textOffsetRight);
    this->textOffsetTop = settings->GetFloat(_T("TextOffsetTop"), defaults->textOffsetTop);
    this->textRotation = settings->GetFloat(_T("TextRotation"), defaults->textRotation);

    this->fontStrokeWidth = settings->GetFloat(_T("FontStrokeWidth"), defaults->fontStrokeWidth);
    
    settings->GetString(_T("TextTrimmingGranularity"), buffer, _countof(buffer), trimmingGranularityMap.GetByA(defaults->textTrimmingGranularity, _T("Character")));
    this->textTrimmingGranularity = ParseTrimmingGranularity(buffer);
    
    settings->GetString(_T("TextVerticalAlign"), buffer, _countof(buffer), paragraphAlignmentMap.GetByA(defaults->textVerticalAlign, _T("Top")));
    this->textVerticalAlign = ParseParagraphAlignment(buffer);

    settings->GetString(_T("WordWrapping"), buffer, _countof(buffer), wordWrappingMap.GetByA(defaults->wordWrapping, _T("NoWrap")));
    this->wordWrapping = ParseWordWrapping(buffer);

    // Load brushes
    this->brushSettings[State::BrushType::Background].Load(settings, &defaults->brushSettings[State::BrushType::Background]);
    
    ::Settings* outlineSettings = settings->CreateChild(_T("Outline"));
    this->brushSettings[State::BrushType::Outline].Load(outlineSettings, &defaults->brushSettings[State::BrushType::Outline]);

    ::Settings* textSettings = settings->CreateChild(_T("Font"));
    this->brushSettings[State::BrushType::Text].Load(textSettings, &defaults->brushSettings[State::BrushType::Text]);

    ::Settings* textOutlineSettings = textSettings->CreateChild(_T("Stroke"));
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
