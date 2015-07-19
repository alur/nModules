#pragma once

#include "../nCoreApi/IPane.hpp"
#include "../nCoreApi/ISettingsReader.hpp"

#include "../nShared/FallbackOptional.hpp"

#include "../Headers/d2d1.h"
#include "../Headers/lsapi.h"

#include <vector>

class BackgroundPainterState {
public:
  struct PerPaneData {
    float cornerRadiusX;
    float cornerRadiusY;
    float outlineWidth;
  };

public:
  BackgroundPainterState(const ISettingsReader *settingsReader, BackgroundPainterState *base);
  ~BackgroundPainterState();

  HRESULT CreateDeviceResources(ID2D1RenderTarget*);
  void DiscardDeviceResources();
  void Paint(ID2D1RenderTarget*, const D2D1_RECT_F *area, PerPaneData&, const IPane*) const;
  void PositionChanged(const IPane*, PerPaneData&, const D2D1_RECT_F&, bool isResize, bool isMove);

public:
  BackgroundPainterState(const BackgroundPainterState&) = delete;
  BackgroundPainterState& operator=(BackgroundPainterState&) = delete;

private:
  FallbackOptional<NLENGTH> mCornerRadiusX;
  FallbackOptional<NLENGTH> mCornerRadiusY;
  FallbackOptional<NLENGTH> mOutlineWidth;
  FallbackOptional<D2D1_COLOR_F> mColor;

private:
  BackgroundPainterState *const mBase;

  // The states which have this as their base state.
  std::vector<BackgroundPainterState*> mDependents;

  // TODO(Erik): Move this
private:
  ID2D1Brush *mBrush;
};
