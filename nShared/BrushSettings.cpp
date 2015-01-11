/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  BrushSettings.cpp
 *  The nModules Project
 *
 *  Holds all RC settings used for a brush.
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../nShared/LiteStep.h"
#include <strsafe.h>
#include "BrushSettings.hpp"
#include "../Utilities/StringUtils.h"
#include "LiteralColorVal.hpp"


/// <summary>
/// Initalizes the class to all default settings.
/// </summary>
BrushSettings::BrushSettings() {
  StringCchCopy(this->brushType, sizeof(this->brushType), L"SolidColor");
  this->color = std::unique_ptr<IColorVal>(new LiteralColorVal(0xFF000000));
  this->gradientColors = _wcsdup(L"Black");
  this->gradientCenterX = 0.0f;
  this->gradientCenterY = 0.0f;
  this->gradientEndX = 0.0f;
  this->gradientEndY = 0.0f;
  this->gradientOriginOffsetX = 0.0f;
  this->gradientOriginOffsetY = 0.0f;
  this->gradientRadiusX = 0.0f;
  this->gradientRadiusY = 0.0f;
  this->gradientStartX = 0.0f;
  this->gradientStartY = 0.0f;
  this->gradientStops = _wcsdup(L"0.0");
  this->image = _wcsdup(L"");
  this->imageEdges = D2D1::RectF();
  this->imageOpacity = 1.0f;
  this->imageRotation = 0.0f;
  StringCchCopy(this->imageScalingMode, sizeof(this->imageScalingMode), L"Tile");
  StringCchCopy(this->tilingModeX, sizeof(this->tilingModeX), L"Tile");
  StringCchCopy(this->tilingModeY, sizeof(this->tilingModeY), L"Tile");
}


BrushSettings::BrushSettings(const BrushSettings &source) {
  StringCchCopy(this->brushType, sizeof(this->brushType), source.brushType);
  this->color = std::unique_ptr<IColorVal>(source.color->Copy());
  this->gradientColors = _wcsdup(source.gradientColors);
  this->gradientCenterX = source.gradientCenterX;
  this->gradientCenterY = source.gradientCenterY;
  this->gradientEndX = source.gradientEndX;
  this->gradientEndY = source.gradientEndY;
  this->gradientOriginOffsetX = source.gradientOriginOffsetX;
  this->gradientOriginOffsetY = source.gradientOriginOffsetY;
  this->gradientRadiusX = source.gradientRadiusX;
  this->gradientRadiusY = source.gradientRadiusY;
  this->gradientStartX = source.gradientStartX;
  this->gradientStartY = source.gradientStartY;
  this->gradientStops = _wcsdup(source.gradientStops);
  this->image = _wcsdup(source.image);
  this->imageEdges = source.imageEdges;
  this->imageOpacity = source.imageOpacity;
  this->imageRotation = source.imageRotation;
  StringCchCopy(this->imageScalingMode, sizeof(this->imageScalingMode), source.imageScalingMode);
  StringCchCopy(this->tilingModeX, sizeof(this->tilingModeX), source.tilingModeX);
  StringCchCopy(this->tilingModeY, sizeof(this->tilingModeY), source.tilingModeY);
}


BrushSettings::BrushSettings(void init(BrushSettings &)) : BrushSettings() {
  init(*this);
}


/// <summary>
/// Destructor.
/// </summary>
BrushSettings::~BrushSettings() {
  free(this->gradientColors);
  free(this->gradientStops);
  free(this->image);
}


/// <summary>
/// Loads settings from an RC file using the specified defaults.
/// </summary>
void BrushSettings::Load(const Settings * settings, const BrushSettings * defaults) {
  TCHAR buf[MAX_LINE_LENGTH];

  settings->GetString(L"BrushType", this->brushType, sizeof(this->brushType), defaults->brushType);
  this->color = std::unique_ptr<IColorVal>(settings->GetColor(L"Color", defaults->color.get()));
  this->gradientEndX = settings->GetFloat(L"GradientEndX", defaults->gradientEndX);
  this->gradientEndY = settings->GetFloat(L"GradientEndY", defaults->gradientEndY);
  settings->GetString(L"GradientColors", buf, sizeof(buf), defaults->gradientColors);
  this->gradientColors = StringUtils::ReallocOverwrite(this->gradientColors, buf);
  this->gradientCenterX = settings->GetFloat(L"GradientCenterX", defaults->gradientCenterX);
  this->gradientCenterY = settings->GetFloat(L"GradientCenterY", defaults->gradientCenterY);
  this->gradientOriginOffsetX = settings->GetFloat(L"GradientOriginOffsetX", defaults->gradientOriginOffsetX);
  this->gradientOriginOffsetY = settings->GetFloat(L"GradientOriginOffsetY", defaults->gradientOriginOffsetY);
  this->gradientRadiusX = settings->GetFloat(L"GradientRadiusX", defaults->gradientRadiusX);
  this->gradientRadiusY = settings->GetFloat(L"GradientRadiusY", defaults->gradientRadiusY);
  this->gradientStartX = settings->GetFloat(L"GradientStartX", defaults->gradientStartX);
  this->gradientStartY = settings->GetFloat(L"GradientStartY", defaults->gradientStartY);
  settings->GetString(L"GradientStops", buf, sizeof(buf), defaults->gradientStops);
  this->gradientStops = StringUtils::ReallocOverwrite(this->gradientStops, buf);
  settings->GetString(L"Image", buf, sizeof(buf), defaults->image);
  this->image = StringUtils::ReallocOverwrite(this->image, buf);
  this->imageEdges = settings->GetOffsetRectF(L"ImageEdges", &defaults->imageEdges);
  this->imageOpacity = settings->GetInt(L"Alpha", int(defaults->imageOpacity * 255)) / 255.0f;
  this->imageRotation = settings->GetFloat(L"ImageRotation", defaults->imageRotation);
  settings->GetString(L"ImageScalingMode", this->imageScalingMode, sizeof(this->imageScalingMode), defaults->imageScalingMode);
  settings->GetString(L"TilingModeX", this->tilingModeX, sizeof(this->tilingModeX), defaults->tilingModeX);
  settings->GetString(L"TilingModeY", this->tilingModeY, sizeof(this->tilingModeY), defaults->tilingModeY);
}
