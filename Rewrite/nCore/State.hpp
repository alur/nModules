#pragma once

#include "../nUtilities/lsapi.h"

#include <dwrite.h>
#include <vector>

class State {
public:
  State();
  ~State();

private:
  struct Settings {
    wchar_t font[MAX_PATH]; // Default: Arial
    DWRITE_FONT_STRETCH fontStretch; // Default: Normal
    DWRITE_FONT_STYLE fontStyle; // Default Normal
    DWRITE_FONT_WEIGHT fontWeight; // Default: Normal
    DWRITE_READING_DIRECTION readingDirection; // Default: LeftToRight
    DWRITE_TEXT_ALIGNMENT textAlign; // Default: Left
    DWRITE_TRIMMING_GRANULARITY textTrimmingGranularity; // Default: Character
    DWRITE_PARAGRAPH_ALIGNMENT textVerticalAlign; // Default: Top
    DWRITE_WORD_WRAPPING wordWrapping; // Default: NoWrap
    float cornerRadiusX; // Default: 0
    float cornerRadiusY; // Default: 0
    float fontSize; // Default: 12
    float outlineWidth; // Default: 0
    float textOffsetBottom; // Default: 0
    float textOffsetLeft; // Default: 0
    float textOffsetRight; // Default: 0
    float textOffsetTop; // Default: 0
    float textRotation; // Default: 0
    float fontStrokeWidth; // Default: 0
  } mSettings;

  State *mBase;

  std::vector<State*> mDependents;
};
