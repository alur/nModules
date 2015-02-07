#pragma once

#include <vector>

// Per-brush data for a pane.
struct PainterBrushData {

};

// Per-State data for a pane.
struct PainterStateData {
  PainterBrushData brushData[4];

  // The rect we draw text in. This is the rect painting rect of the pane, offset from the edges.
  D2D1_RECT_F textPosition;
  IDWriteTextLayout *textLayout;

  // The area we paint the background in.
  D2D1_ROUNDED_RECT backgroundArea;

  // The area we paint the outline in.
  D2D1_ROUNDED_RECT outlineArea;
};

struct PainterData {
  std::vector<bool> stateStatus;
  std::vector<PainterStateData> stateData;
  BYTE activeState;
};
