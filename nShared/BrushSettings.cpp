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
BrushSettings::BrushSettings()
{
    StringCchCopy(this->brushType, sizeof(this->brushType), _T("SolidColor"));
    this->color = std::unique_ptr<IColorVal>(new LiteralColorVal(0xFF000000));
    this->gradientColors = _tcsdup(_T("Black"));
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
    this->gradientStops = _tcsdup(_T("0.0"));
    this->image = _tcsdup(_T(""));
    this->imageEdges = D2D1::RectF();
    this->imageOpacity = 1.0f;
    this->imageRotation = 0.0f;
    StringCchCopy(this->imageScalingMode, sizeof(this->imageScalingMode), _T("Tile"));
    StringCchCopy(this->tilingModeX, sizeof(this->tilingModeX), _T("Tile"));
    StringCchCopy(this->tilingModeY, sizeof(this->tilingModeY), _T("Tile"));
}


BrushSettings::BrushSettings(const BrushSettings &source)
{
    StringCchCopy(this->brushType, sizeof(this->brushType), source.brushType);
    this->color = std::unique_ptr<IColorVal>(source.color->Copy());
    this->gradientColors = _tcsdup(source.gradientColors);
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
    this->gradientStops = _tcsdup(source.gradientStops);
    this->image = _tcsdup(source.image);
    this->imageEdges = source.imageEdges;
    this->imageOpacity = source.imageOpacity;
    this->imageRotation = source.imageRotation;
    StringCchCopy(this->imageScalingMode, sizeof(this->imageScalingMode), source.imageScalingMode);
    StringCchCopy(this->tilingModeX, sizeof(this->tilingModeX), source.tilingModeX);
    StringCchCopy(this->tilingModeY, sizeof(this->tilingModeY), source.tilingModeY);
}


/// <summary>
/// Destructor.
/// </summary>
BrushSettings::~BrushSettings()
{
    free(this->gradientColors);
    free(this->gradientStops);
    free(this->image);
}


/// <summary>
/// Loads settings from an RC file using the specified defaults.
/// </summary>
void BrushSettings::Load(const Settings * settings, const BrushSettings * defaults)
{
    TCHAR buf[MAX_LINE_LENGTH];

    settings->GetString(_T("BrushType"), this->brushType, sizeof(this->brushType), defaults->brushType);
    this->color = std::unique_ptr<IColorVal>(settings->GetColor(_T("Color"), defaults->color.get()));
    this->gradientEndX = settings->GetFloat(_T("GradientEndX"), defaults->gradientEndX);
    this->gradientEndY = settings->GetFloat(_T("GradientEndY"), defaults->gradientEndY);
    settings->GetString(_T("GradientColors"), buf, sizeof(buf), defaults->gradientColors);
    this->gradientColors = StringUtils::ReallocOverwrite(this->gradientColors, buf);
    this->gradientCenterX = settings->GetFloat(_T("GradientCenterX"), defaults->gradientCenterX);
    this->gradientCenterY = settings->GetFloat(_T("GradientCenterY"), defaults->gradientCenterY);
    this->gradientOriginOffsetX = settings->GetFloat(_T("GradientOriginOffsetX"), defaults->gradientOriginOffsetX);
    this->gradientOriginOffsetY = settings->GetFloat(_T("GradientOriginOffsetY"), defaults->gradientOriginOffsetY);
    this->gradientRadiusX = settings->GetFloat(_T("GradientRadiusX"), defaults->gradientRadiusX);
    this->gradientRadiusY = settings->GetFloat(_T("GradientRadiusY"), defaults->gradientRadiusY);
    this->gradientStartX = settings->GetFloat(_T("GradientStartX"), defaults->gradientStartX);
    this->gradientStartY = settings->GetFloat(_T("GradientStartY"), defaults->gradientStartY);
    settings->GetString(_T("GradientStops"), buf, sizeof(buf), defaults->gradientStops);
    this->gradientStops = StringUtils::ReallocOverwrite(this->gradientStops, buf);
    settings->GetString(_T("Image"), buf, sizeof(buf), defaults->image);
    this->image = StringUtils::ReallocOverwrite(this->image, buf);
    this->imageEdges = settings->GetOffsetRectF(_T("ImageEdges"), &defaults->imageEdges);
    this->imageOpacity = settings->GetInt(_T("Alpha"), int(defaults->imageOpacity*255))/255.0f;
    this->imageRotation = settings->GetFloat(_T("ImageRotation"), defaults->imageRotation);
    settings->GetString(_T("ImageScalingMode"), this->imageScalingMode, sizeof(this->imageScalingMode), defaults->imageScalingMode);
    settings->GetString(_T("TilingModeX"), this->tilingModeX, sizeof(this->tilingModeX), defaults->tilingModeX);
    settings->GetString(_T("TilingModeY"), this->tilingModeY, sizeof(this->tilingModeY), defaults->tilingModeY);
}
