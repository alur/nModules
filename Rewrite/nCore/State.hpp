#pragma once

#include "../nCoreApi/ISettingsReader.hpp"

#include "../Headers/d2d1.h"
#include "../Headers/lsapi.h"

#include <dwrite.h>
#include <vector>

class State {
public:
  State(ISettingsReader *settingsReader, State *base);
  ~State();

public:
  State(const State&) = delete;
  State& operator=(State&) = delete;

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
    NLENGTH cornerRadiusX; // Default: 0
    NLENGTH cornerRadiusY; // Default: 0
    NLENGTH fontSize; // Default: 12
    NLENGTH outlineWidth; // Default: 0
    NRECT textPadding; // Default: 0 0 0 0
    float textRotation; // Default: 0
    NLENGTH fontStrokeWidth; // Default: 0
  } mSettings;

private:
  State *const mBase;

  // The states which have this as their base state.
  std::vector<State*> mDependents;

  // TODO(Erik): Move this
private:
  D2D1_COLOR_F mColor;
};
