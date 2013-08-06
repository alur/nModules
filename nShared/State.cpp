/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  DrawableWindowState.cpp
 *  The nModules Project
 *
 *  A state for a DrawableWindow.
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "LiteStep.h"
#include "State.hpp"
#include "Factories.h"
#include <d2d1.h>
#include <Wincodec.h>
#include "Color.h"


using namespace D2D1;


State::State(LPCTSTR stateName, Settings* settings, int defaultPriority, LPCWSTR* text) {
    this->active = false;
    this->backBrush = new Brush();
    this->drawingSettings = new StateSettings();
    this->outlineBrush = new Brush();
    this->priority = settings->GetInt(_T("Priority"), defaultPriority);
    this->settings = settings;
    this->text = text;
    this->textBrush = new Brush();
    this->textFormat = nullptr;
    this->textShadowBrush = new Brush();
    mName = _tcsdup(stateName);
}


State::~State() {
    DiscardDeviceResources();
    free(const_cast<LPTSTR>(mName));
    SAFEDELETE(this->settings);
    SAFEDELETE(this->drawingSettings);
    SAFERELEASE(this->textFormat);
    SAFEDELETE(this->backBrush);
    SAFEDELETE(this->outlineBrush);
    SAFEDELETE(this->textBrush);
    SAFEDELETE(this->textShadowBrush);
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
    this->textShadowBrush->UpdatePosition(position);

    this->drawingArea.rect = this->backBrush->brushPosition;

    this->outlineArea = this->drawingArea;
    this->outlineArea.rect.left += this->drawingSettings->outlineWidth / 2;
    this->outlineArea.rect.right -= this->drawingSettings->outlineWidth / 2;
    this->outlineArea.rect.top += this->drawingSettings->outlineWidth / 2;
    this->outlineArea.rect.bottom -= this->drawingSettings->outlineWidth / 2;
}


void State::Load(StateSettings* defaultSettings) {
    this->drawingSettings->Load(this->settings, defaultSettings);

    this->drawingArea.radiusX = this->outlineArea.radiusX = this->drawingSettings->cornerRadiusX;
    this->drawingArea.radiusY = this->outlineArea.radiusY = this->drawingSettings->cornerRadiusY;

    this->backBrush->Load(&this->drawingSettings->backgroundBrush);
    this->outlineBrush->Load(&this->drawingSettings->outlineBrush);
    this->textBrush->Load(&this->drawingSettings->textBrush);
    this->textShadowBrush->Load(&this->drawingSettings->textDropShadowBrush);

    CreateTextFormat(this->textFormat);
}


void State::DiscardDeviceResources() {
    backBrush->Discard();
    outlineBrush->Discard();
    textBrush->Discard();
    textShadowBrush->Discard();
}


void State::Paint(ID2D1RenderTarget* renderTarget) {
    if (this->backBrush->brush) {
        renderTarget->FillRoundedRectangle(this->drawingArea, this->backBrush->brush);
    }
    if (this->outlineBrush->brush && this->drawingSettings->outlineWidth != 0) {
        renderTarget->DrawRoundedRectangle(this->outlineArea, this->outlineBrush->brush, this->drawingSettings->outlineWidth);
    }
    /*if (this->textShadowBrush->brush) {
        renderTarget->SetTransform(Matrix3x2F::Rotation(this->drawingSettings->textRotation, this->textRotationOrigin));
        renderTarget->DrawText(*this->text, lstrlenW(*this->text), this->textDropFormat, this->textArea, this->textShadowBrush->brush, D2D1_DRAW_TEXT_OPTIONS_CLIP);
        renderTarget->SetTransform(Matrix3x2F::Identity());
    }*/
    if (this->textBrush->brush && **this->text != L'\0') {
        renderTarget->SetTransform(Matrix3x2F::Rotation(this->drawingSettings->textRotation, this->textRotationOrigin));
        renderTarget->DrawText(*this->text, lstrlenW(*this->text), this->textFormat, this->textArea, this->textBrush->brush, D2D1_DRAW_TEXT_OPTIONS_CLIP);
        renderTarget->SetTransform(Matrix3x2F::Identity());
    }
}


HRESULT State::ReCreateDeviceResources(ID2D1RenderTarget* renderTarget) {
    backBrush->ReCreate(renderTarget);
    outlineBrush->ReCreate(renderTarget);
    textBrush->ReCreate(renderTarget);
    textShadowBrush->ReCreate(renderTarget);

    this->drawingArea.rect = this->backBrush->brushPosition;
    this->outlineArea = this->drawingArea;
    this->outlineArea.rect.left += this->drawingSettings->outlineWidth / 2;
    this->outlineArea.rect.right -= this->drawingSettings->outlineWidth / 2;
    this->outlineArea.rect.top += this->drawingSettings->outlineWidth / 2;
    this->outlineArea.rect.bottom -= this->drawingSettings->outlineWidth / 2;
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
HRESULT State::CreateTextFormat(IDWriteTextFormat *&textFormat) {
    // Create the text format
    IDWriteFactory *pDWFactory = nullptr;
    Factories::GetDWriteFactory(reinterpret_cast<LPVOID*>(&pDWFactory));
    pDWFactory->CreateTextFormat(
        drawingSettings->font,
        nullptr,
        drawingSettings->fontWeight,
        drawingSettings->fontStyle,
        drawingSettings->fontStretch,
        drawingSettings->fontSize,
        L"en-US",
        &textFormat);

    textFormat->SetTextAlignment(drawingSettings->textAlign);
    textFormat->SetParagraphAlignment(drawingSettings->textVerticalAlign);
    textFormat->SetWordWrapping(drawingSettings->wordWrapping);
    textFormat->SetReadingDirection(drawingSettings->readingDirection);

    // Set the trimming method
    DWRITE_TRIMMING trimmingOptions;
    trimmingOptions.delimiter = 0;
    trimmingOptions.delimiterCount = 0;
    trimmingOptions.granularity = drawingSettings->textTrimmingGranularity;
    textFormat->SetTrimming(&trimmingOptions, nullptr);

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


void State::SetCornerRadiusX(float radius) {
    this->drawingArea.radiusX = this->outlineArea.radiusX = radius;
}


void State::SetCornerRadiusY(float radius) {
    this->drawingArea.radiusY = this->outlineArea.radiusY = radius;
}


void State::SetOutlineWidth(float width) {
    this->drawingSettings->outlineWidth = width;
}


void State::SetReadingDirection(DWRITE_READING_DIRECTION direction) {
    this->textFormat->SetReadingDirection(direction);
}


void State::SetTextAlignment(DWRITE_TEXT_ALIGNMENT alignment) {
    this->textFormat->SetTextAlignment(alignment);
}


void State::SetTextRotation(float rotation) {
    this->drawingSettings->textRotation = rotation;
}


void State::SetTextTrimmingGranuality(DWRITE_TRIMMING_GRANULARITY granularity) {
    DWRITE_TRIMMING options;
    IDWriteInlineObject *trimmingSign;
    this->textFormat->GetTrimming(&options, &trimmingSign);
    options.granularity = granularity;
    this->textFormat->SetTrimming(&options, trimmingSign);
}


void State::SetTextVerticalAlign(DWRITE_PARAGRAPH_ALIGNMENT alignment) {
    this->textFormat->SetParagraphAlignment(alignment);
}


void State::SetWordWrapping(DWRITE_WORD_WRAPPING wrapping) {
    this->textFormat->SetWordWrapping(wrapping);
}


Brush* State::GetBrush(LPCTSTR brushName)
{
    if (*brushName == _T('\0')) {
        return this->backBrush;
    }
    else if (_tcsicmp(brushName, _T("Text")) == 0) {
        return this->textBrush;
    }
    else if (_tcsicmp(brushName, _T("Outline")) == 0) {
        return this->outlineBrush;
    }

    return nullptr;
}