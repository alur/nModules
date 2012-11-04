/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  DrawableWindowState.cpp
 *  The nModules Project
 *
 *  A state for a DrawableWindow.
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "LiteStep.h"
#include "Debugging.h"
#include "State.hpp"
#include "Factories.h"
#include "Macros.h"
#include <d2d1.h>
#include <Wincodec.h>
#include "Color.h"


using namespace D2D1;


State::State(Settings* settings, int defaultPriority, LPCWSTR* text) {
    this->active = false;
    this->backBrush = new Brush();
    this->drawingSettings = new StateSettings();
    this->outlineBrush = new Brush();
    this->priority = settings->GetInt("Priority", defaultPriority);
    this->settings = settings;
    this->text = text;
    this->textBrush = new Brush();
    this->textFormat = NULL;
}


State::~State() {
    DiscardDeviceResources();
    SAFEDELETE(this->settings);
    SAFEDELETE(this->drawingSettings);
    SAFERELEASE(this->textFormat);
    SAFEDELETE(this->backBrush);
    SAFEDELETE(this->outlineBrush);
    SAFEDELETE(this->textBrush);
}


void State::UpdatePosition(D2D1_RECT_F position) {
    this->drawingArea.rect = position;
    this->textRotationOrigin.x = position.left + (position.right - position.left)/2.0f;
    this->textRotationOrigin.y = position.top + (position.bottom - position.top)/2.0f;

    this->textArea = this->drawingArea.rect;
    this->textArea.bottom -= this->drawingSettings->textOffsetBottom;
    this->textArea.top += this->drawingSettings->textOffsetTop;
    this->textArea.left += this->drawingSettings->textOffsetLeft;
    this->textArea.right -= this->drawingSettings->textOffsetRight;

    this->backBrush->UpdatePosition(position);
    this->outlineBrush->UpdatePosition(position);
    this->textBrush->UpdatePosition(position);

    this->drawingArea.rect = this->backBrush->brushPosition;
}


void State::Load(StateSettings* defaultSettings) {
    this->drawingSettings->Load(this->settings, defaultSettings);

    this->drawingArea.radiusX = this->drawingSettings->cornerRadiusX;
    this->drawingArea.radiusY = this->drawingSettings->cornerRadiusY;

    this->backBrush->Load(&this->drawingSettings->backgroundBrush);
    this->outlineBrush->Load(&this->drawingSettings->outlineBrush);
    this->textBrush->Load(&this->drawingSettings->textBrush);

    CreateTextFormat();
}


void State::DiscardDeviceResources() {
    backBrush->Discard();
    outlineBrush->Discard();
    textBrush->Discard();
}


void State::Paint(ID2D1RenderTarget* renderTarget) {
    if (this->backBrush->brush) {
        renderTarget->FillRoundedRectangle(this->drawingArea, this->backBrush->brush);
    }
    if (this->outlineBrush->brush) {
        renderTarget->DrawRoundedRectangle(this->drawingArea, this->outlineBrush->brush, this->drawingSettings->outlineWidth);
    }
    if (this->textBrush->brush) {
        renderTarget->SetTransform(Matrix3x2F::Rotation(this->drawingSettings->textRotation, this->textRotationOrigin));
        renderTarget->DrawText(*this->text, lstrlenW(*this->text), this->textFormat, this->textArea, this->textBrush->brush, D2D1_DRAW_TEXT_OPTIONS_CLIP);
        renderTarget->SetTransform(Matrix3x2F::Identity());
    }
}


HRESULT State::ReCreateDeviceResources(ID2D1RenderTarget* renderTarget) {
    backBrush->ReCreate(renderTarget);
    outlineBrush->ReCreate(renderTarget);
    textBrush->ReCreate(renderTarget);

    this->drawingArea.rect = this->backBrush->brushPosition;
    return S_OK;
}


/// <summary>
/// Gets the "Desired" size of the window, given the specified constraints.
/// </summary>
/// <param name="maxWidth">Out. The maximum width to return.</param>
/// <param name="maxHeight">Out. The maximum height to return.</param>
/// <param name="size">Out. The desired size will be placed in this SIZE.</param>
void State::GetDesiredSize(int maxWidth, int maxHeight, LPSIZE size) {
    IDWriteFactory* factory = NULL;
    IDWriteTextLayout* textLayout = NULL;
    DWRITE_TEXT_METRICS metrics;
    maxWidth -= int(this->drawingSettings->textOffsetLeft + this->drawingSettings->textOffsetRight);
    maxHeight -= int(this->drawingSettings->textOffsetTop + this->drawingSettings->textOffsetBottom);

    Factories::GetDWriteFactory(reinterpret_cast<LPVOID*>(&factory));
    factory->CreateTextLayout(*this->text, lstrlenW(*this->text), this->textFormat, (float)maxWidth, (float)maxHeight, &textLayout);
    textLayout->GetMetrics(&metrics);
    SAFERELEASE(textLayout);

    size->cx = long(metrics.width + this->drawingSettings->textOffsetLeft + this->drawingSettings->textOffsetRight) + 1;
    size->cy = long(metrics.height + this->drawingSettings->textOffsetTop + this->drawingSettings->textOffsetBottom) + 1;
}


/// <summary>
/// Creates a textFormat based on the specified drawingSettings.
/// </summary>
/// <param name="drawingSettings">The settings to create the textformat with.</param>
/// <param name="textFormat">Out. The textformat.</param>
/// <returns>S_OK</returns>
HRESULT State::CreateTextFormat() {
    // Font weight
    DWRITE_FONT_WEIGHT fontWeight;
    if (_stricmp(drawingSettings->fontWeight, "Thin") == 0)
        fontWeight = DWRITE_FONT_WEIGHT_THIN;
    else if (_stricmp(drawingSettings->fontWeight, "Extra Light") == 0)
        fontWeight = DWRITE_FONT_WEIGHT_EXTRA_LIGHT;
    else if (_stricmp(drawingSettings->fontWeight, "Ultra Light") == 0)
        fontWeight = DWRITE_FONT_WEIGHT_ULTRA_LIGHT;
    else if (_stricmp(drawingSettings->fontWeight, "Light") == 0)
        fontWeight = DWRITE_FONT_WEIGHT_LIGHT;
    else if (_stricmp(drawingSettings->fontWeight, "Semi Light") == 0)
        fontWeight = DWRITE_FONT_WEIGHT_SEMI_LIGHT;
    else if (_stricmp(drawingSettings->fontWeight, "Regular") == 0)
        fontWeight = DWRITE_FONT_WEIGHT_REGULAR;
    else if (_stricmp(drawingSettings->fontWeight, "Medium") == 0)
        fontWeight = DWRITE_FONT_WEIGHT_MEDIUM;
    else if (_stricmp(drawingSettings->fontWeight, "Semi Bold") == 0)
        fontWeight = DWRITE_FONT_WEIGHT_SEMI_BOLD;
    else if (_stricmp(drawingSettings->fontWeight, "Bold") == 0)
        fontWeight = DWRITE_FONT_WEIGHT_BOLD;
    else if (_stricmp(drawingSettings->fontWeight, "Extra Bold") == 0)
        fontWeight = DWRITE_FONT_WEIGHT_EXTRA_BOLD;
    else if (_stricmp(drawingSettings->fontWeight, "Ultra Bold") == 0)
        fontWeight = DWRITE_FONT_WEIGHT_ULTRA_BOLD;
    else if (_stricmp(drawingSettings->fontWeight, "Black") == 0)
        fontWeight = DWRITE_FONT_WEIGHT_BLACK;
    else if (_stricmp(drawingSettings->fontWeight, "Heavy") == 0)
        fontWeight = DWRITE_FONT_WEIGHT_HEAVY;
    else if (_stricmp(drawingSettings->fontWeight, "Extra Black") == 0)
        fontWeight = DWRITE_FONT_WEIGHT_EXTRA_BLACK;
    else if (_stricmp(drawingSettings->fontWeight, "Ultra Black") == 0)
        fontWeight = DWRITE_FONT_WEIGHT_ULTRA_BLACK;
    else
        fontWeight = DWRITE_FONT_WEIGHT_NORMAL;

    // Font style
    DWRITE_FONT_STYLE fontStyle;
    if (_stricmp(drawingSettings->fontStyle, "Oblique") == 0)
        fontStyle = DWRITE_FONT_STYLE_OBLIQUE;
    else if (_stricmp(drawingSettings->fontStyle, "Italic") == 0)
        fontStyle = DWRITE_FONT_STYLE_ITALIC;
    else
        fontStyle = DWRITE_FONT_STYLE_NORMAL;

    // Font stretch
    DWRITE_FONT_STRETCH fontStretch;
    if (_stricmp(drawingSettings->fontStretch, "Ultra Condensed") == 0)
        fontStretch = DWRITE_FONT_STRETCH_ULTRA_CONDENSED;
    else if (_stricmp(drawingSettings->fontStretch, "Extra Condensed") == 0)
        fontStretch = DWRITE_FONT_STRETCH_EXTRA_CONDENSED;
    else if (_stricmp(drawingSettings->fontStretch, "Condensed") == 0)
        fontStretch = DWRITE_FONT_STRETCH_CONDENSED;
    else if (_stricmp(drawingSettings->fontStretch, "Semi Condensed") == 0)
        fontStretch = DWRITE_FONT_STRETCH_SEMI_CONDENSED;
    else if (_stricmp(drawingSettings->fontStretch, "Medium") == 0)
        fontStretch = DWRITE_FONT_STRETCH_MEDIUM;
    else if (_stricmp(drawingSettings->fontStretch, "Semi Expanded") == 0)
        fontStretch = DWRITE_FONT_STRETCH_SEMI_EXPANDED;
    else if (_stricmp(drawingSettings->fontStretch, "Expanded") == 0)
        fontStretch = DWRITE_FONT_STRETCH_EXPANDED;
    else if (_stricmp(drawingSettings->fontStretch, "Extra Expanded") == 0)
        fontStretch = DWRITE_FONT_STRETCH_EXTRA_EXPANDED;
    else if (_stricmp(drawingSettings->fontStretch, "Ultra Expanded") == 0)
        fontStretch = DWRITE_FONT_STRETCH_ULTRA_EXPANDED;
    else
        fontStretch = DWRITE_FONT_STRETCH_NORMAL;

    // Create the text format
    IDWriteFactory *pDWFactory = NULL;
    Factories::GetDWriteFactory(reinterpret_cast<LPVOID*>(&pDWFactory));
    pDWFactory->CreateTextFormat(
        drawingSettings->font,
        NULL,
        fontWeight,
        fontStyle,
        fontStretch,
        drawingSettings->fontSize,
        L"en-US",
        &this->textFormat);

    // Set the horizontal text alignment
    if (_stricmp(drawingSettings->textAlign, "Center") == 0)
        this->textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
    else if (_stricmp(drawingSettings->textAlign, "Right") == 0)
        this->textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
    else
        this->textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);

    // Set the vertical text alignment
    if (_stricmp(drawingSettings->textVerticalAlign, "Middle") == 0)
        this->textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    else if (_stricmp(drawingSettings->textVerticalAlign, "Bottom") == 0)
        this->textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_FAR);
    else
        this->textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);

    // Set the trimming method
    DWRITE_TRIMMING trimmingOptions;
    trimmingOptions.delimiter = 0;
    trimmingOptions.delimiterCount = 0;
    if (_stricmp(drawingSettings->textTrimmingGranularity, "None") == 0)
        trimmingOptions.granularity = DWRITE_TRIMMING_GRANULARITY_NONE;
    else if (_stricmp(drawingSettings->textVerticalAlign, "Word") == 0)
        trimmingOptions.granularity = DWRITE_TRIMMING_GRANULARITY_WORD;
    else
        trimmingOptions.granularity = DWRITE_TRIMMING_GRANULARITY_CHARACTER;

    this->textFormat->SetTrimming(&trimmingOptions, NULL);

    // Set word wrapping
    this->textFormat->SetWordWrapping(drawingSettings->wordWrap ? DWRITE_WORD_WRAPPING_WRAP : DWRITE_WORD_WRAPPING_NO_WRAP);

    // Set reading direction
    this->textFormat->SetReadingDirection(drawingSettings->rightToLeft ? DWRITE_READING_DIRECTION_RIGHT_TO_LEFT : DWRITE_READING_DIRECTION_LEFT_TO_RIGHT);

    return S_OK;
}


/// <summary>
/// Sets the text offsets for the specified state.
/// </summary>
/// <param name="left">The text offset from the left.</param>
/// <param name="top">The text offset from the top.</param>
/// <param name="right">The text offset from the right.</param>
/// <param name="bottom">The text offset from the bottom.</param>
/// <param name="state">The state to set the offsets for.</param>
void State::SetTextOffsets(float left, float top, float right, float bottom) {
    this->drawingSettings->textOffsetBottom = bottom;
    this->drawingSettings->textOffsetLeft = left;
    this->drawingSettings->textOffsetRight = right;
    this->drawingSettings->textOffsetTop = top;

    this->textArea = this->drawingArea.rect;
    this->textArea.bottom -= this->drawingSettings->textOffsetBottom;
    this->textArea.top += this->drawingSettings->textOffsetTop;
    this->textArea.left += this->drawingSettings->textOffsetLeft;
    this->textArea.right -= this->drawingSettings->textOffsetRight;
}
