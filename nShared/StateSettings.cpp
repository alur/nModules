/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  WindowSettings.cpp
 *  The nModules Project
 *
 *  Holds all RC settings used by the Window class.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../nShared/LiteStep.h"
#include <strsafe.h>
#include "State.hpp"
#include <map>

using std::map;


/// <summary>
/// Initalizes the class to all default settings.
/// </summary>
State::Settings::Settings()
{
    this->cornerRadiusX = 0.0f;
    this->cornerRadiusY = 0.0f;
    StringCchCopy(this->font, _countof(this->font), _T("Arial"));
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

    brushSettings[State::BrushType::Text].color = Color::Create(0xFFFFFFFF);
}


template <typename T>
static T GetValue(LPCTSTR string, map<T, LPCTSTR> map, T defValue)
{
    for (auto &x : map)
    {
        if (_tcsicmp(string, x.second) == 0)
        {
            return x.first;
        }
    }
    return defValue;
}


template <typename T>
static LPCTSTR GetName(T value, map<T, LPCTSTR> map)
{
    for (auto &x : map)
    {
        if (x.first == value)
        {
            return x.second;
        }
    }
    return _T("");
}


static map<DWRITE_READING_DIRECTION, LPCTSTR> readingDirectionMap =
{
    { DWRITE_READING_DIRECTION_LEFT_TO_RIGHT, _T("LeftToRight") },
    { DWRITE_READING_DIRECTION_RIGHT_TO_LEFT, _T("RightToLeft") }
};

static map<DWRITE_WORD_WRAPPING, LPCTSTR> wordWrappingMap =
{
    { DWRITE_WORD_WRAPPING_NO_WRAP, _T("NoWrap") },
    { DWRITE_WORD_WRAPPING_WRAP,    _T("Wrap")   }
};

static map<DWRITE_FONT_STYLE, LPCTSTR> fontStyleMap =
{
    { DWRITE_FONT_STYLE_NORMAL,  _T("Normal")     },
    { DWRITE_FONT_STYLE_OBLIQUE, _T("Oblique")    },
    { DWRITE_FONT_STYLE_ITALIC,  _T("Italic")     }
};

static map<DWRITE_TEXT_ALIGNMENT, LPCTSTR> textAlignmentMap =
{
    { DWRITE_TEXT_ALIGNMENT_LEADING,  _T("Left")   },
    { DWRITE_TEXT_ALIGNMENT_CENTER,   _T("Center") },
    { DWRITE_TEXT_ALIGNMENT_TRAILING, _T("Right")  }
};

static map<DWRITE_PARAGRAPH_ALIGNMENT, LPCTSTR> paragraphAlignmentMap =
{
    { DWRITE_PARAGRAPH_ALIGNMENT_NEAR,     _T("Top")     },
    { DWRITE_PARAGRAPH_ALIGNMENT_CENTER,   _T("Middle")  },
    { DWRITE_PARAGRAPH_ALIGNMENT_FAR,      _T("Bottom")  }
};

static map<DWRITE_TRIMMING_GRANULARITY, LPCTSTR> trimmingGranularityMap = {
    { DWRITE_TRIMMING_GRANULARITY_CHARACTER,    _T("Character") },
    { DWRITE_TRIMMING_GRANULARITY_WORD,         _T("Word")      },
    { DWRITE_TRIMMING_GRANULARITY_NONE,         _T("None")      }
};

static map<DWRITE_FONT_STRETCH, LPCTSTR> fontStretchMap =
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
};

static map<DWRITE_FONT_WEIGHT, LPCTSTR> fontWeightMap =
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
};



/// <summary>
/// Loads settings from an RC file using the specified defaults.
/// </summary>
void State::Settings::Load(::Settings* settings, Settings* defaults)
{
    std::unique_ptr<Settings> newDefs;
    if (!defaults)
    {
        newDefs = std::unique_ptr<Settings>(new Settings());
        defaults = newDefs.get();
    }
    TCHAR buffer[128];

    this->cornerRadiusX = settings->GetFloat(_T("CornerRadiusX"), defaults->cornerRadiusX);
    this->cornerRadiusY = settings->GetFloat(_T("CornerRadiusY"), defaults->cornerRadiusY);
    settings->GetString(_T("Font"), this->font, _countof(this->font), defaults->font);
    this->fontSize = settings->GetFloat(_T("FontSize"), defaults->fontSize);

    settings->GetString(_T("FontStretch"), buffer, _countof(buffer), GetName(defaults->fontStretch, fontStretchMap));
    this->fontStretch = ParseFontStretch(buffer);

    settings->GetString(_T("FontStyle"), buffer, _countof(buffer),  GetName(defaults->fontStyle, fontStyleMap));
    this->fontStyle = ParseFontStyle(buffer);

    settings->GetString(_T("FontWeight"),buffer, _countof(buffer), GetName(defaults->fontWeight, fontWeightMap));
    this->fontWeight = ParseFontWeight(buffer);

    this->outlineWidth = settings->GetFloat(_T("OutlineWidth"), defaults->outlineWidth);

    settings->GetString(_T("ReadingDirection"), buffer, _countof(buffer), GetName(defaults->readingDirection, readingDirectionMap));
    this->readingDirection = ParseReadingDirection(buffer);

    settings->GetString(_T("TextAlign"), buffer, _countof(buffer), GetName(defaults->textAlign, textAlignmentMap));
    this->textAlign = ParseTextAlignment(buffer);

    this->textOffsetBottom = settings->GetFloat(_T("TextOffsetBottom"), defaults->textOffsetBottom);
    this->textOffsetLeft = settings->GetFloat(_T("TextOffsetLeft"), defaults->textOffsetLeft);
    this->textOffsetRight = settings->GetFloat(_T("TextOffsetRight"), defaults->textOffsetRight);
    this->textOffsetTop = settings->GetFloat(_T("TextOffsetTop"), defaults->textOffsetTop);
    this->textRotation = settings->GetFloat(_T("TextRotation"), defaults->textRotation);
    
    settings->GetString(_T("TextTrimmingGranularity"), buffer, _countof(buffer), GetName(defaults->textTrimmingGranularity, trimmingGranularityMap));
    this->textTrimmingGranularity = ParseTrimmingGranularity(buffer);
    
    settings->GetString(_T("TextVerticalAlign"), buffer, _countof(buffer), GetName(defaults->textVerticalAlign, paragraphAlignmentMap));
    this->textVerticalAlign = ParseParagraphAlignment(buffer);

    settings->GetString(_T("WordWrapping"), buffer, _countof(buffer), GetName(defaults->wordWrapping, wordWrappingMap));
    this->wordWrapping = ParseWordWrapping(buffer);

    // Load brushes
    this->brushSettings[State::BrushType::Background].Load(settings, &defaults->brushSettings[State::BrushType::Background]);
    
    ::Settings* outlineSettings = settings->CreateChild(_T("Outline"));
    this->brushSettings[State::BrushType::Outline].Load(outlineSettings, &defaults->brushSettings[State::BrushType::Outline]);

    ::Settings* textSettings = settings->CreateChild(_T("Font"));
    this->brushSettings[State::BrushType::Text].Load(textSettings, &defaults->brushSettings[State::BrushType::Text]);

    ::Settings* textOutlineSettings = textSettings->CreateChild(_T("Outline"));
    this->brushSettings[State::BrushType::TextOutline].Load(textOutlineSettings, &defaults->brushSettings[State::BrushType::TextOutline]);
    
    delete outlineSettings;
    delete textSettings;
    delete textOutlineSettings;
}


DWRITE_FONT_STRETCH State::Settings::ParseFontStretch(LPCTSTR fontStretch) {
    return GetValue(fontStretch, fontStretchMap, DWRITE_FONT_STRETCH_NORMAL);
}


DWRITE_FONT_STYLE State::Settings::ParseFontStyle(LPCTSTR fontStyle) {
    return GetValue(fontStyle, fontStyleMap, DWRITE_FONT_STYLE_NORMAL);
}


DWRITE_FONT_WEIGHT State::Settings::ParseFontWeight(LPCTSTR weight) {
    return GetValue(weight, fontWeightMap, DWRITE_FONT_WEIGHT_NORMAL);
}


DWRITE_TEXT_ALIGNMENT State::Settings::ParseTextAlignment(LPCTSTR textAlignment) {
    return GetValue(textAlignment, textAlignmentMap, DWRITE_TEXT_ALIGNMENT_LEADING);
}


DWRITE_PARAGRAPH_ALIGNMENT State::Settings::ParseParagraphAlignment(LPCTSTR paragraphAlignment) {
    return GetValue(paragraphAlignment, paragraphAlignmentMap, DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
}


DWRITE_TRIMMING_GRANULARITY State::Settings::ParseTrimmingGranularity(LPCTSTR trimmingGranularity) {
    return GetValue(trimmingGranularity, trimmingGranularityMap, DWRITE_TRIMMING_GRANULARITY_CHARACTER);
}


DWRITE_READING_DIRECTION State::Settings::ParseReadingDirection(LPCTSTR readingDirection) {
    return GetValue(readingDirection, readingDirectionMap, DWRITE_READING_DIRECTION_LEFT_TO_RIGHT);
}


DWRITE_WORD_WRAPPING State::Settings::ParseWordWrapping(LPCTSTR wordWrapping) {
    return GetValue(wordWrapping, wordWrappingMap, DWRITE_WORD_WRAPPING_NO_WRAP);
}
