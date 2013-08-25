/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  WindowState.cpp
 *  The nModules Project
 *
 *  A state for a Window.
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "LiteStep.h"
#include "State.hpp"
#include "Factories.h"
#include <d2d1.h>
#include <Wincodec.h>
#include "Color.h"


using namespace D2D1;


State::State()
{
    this->settings = nullptr;
    this->textFormat = nullptr;
}


State::~State()
{
    DiscardDeviceResources();
    SAFEDELETE(this->settings);
    SAFERELEASE(this->textFormat);
}


void State::UpdatePosition(D2D1_RECT_F position, WindowData *windowData)
{
    windowData->drawingArea.rect = position;

    windowData->drawingArea.radiusX = windowData->outlineArea.radiusX = mStateSettings.cornerRadiusX;
    windowData->drawingArea.radiusY = windowData->outlineArea.radiusY = mStateSettings.cornerRadiusY;

    windowData->textRotationOrigin.x = position.left + (position.right - position.left)/2.0f;
    windowData->textRotationOrigin.y = position.top + (position.bottom - position.top)/2.0f;

    windowData->textArea = windowData->drawingArea.rect;
    windowData->textArea.bottom -= mStateSettings.textOffsetBottom;
    windowData->textArea.top += mStateSettings.textOffsetTop;
    windowData->textArea.left += mStateSettings.textOffsetLeft;
    windowData->textArea.right -= mStateSettings.textOffsetRight;

    // Adjust the drawing area to account for the outline.
    windowData->drawingArea.rect.left += mStateSettings.outlineWidth / 2.0f;
    windowData->drawingArea.rect.right -= mStateSettings.outlineWidth / 2.0f;
    windowData->drawingArea.rect.top += mStateSettings.outlineWidth / 2.0f;
    windowData->drawingArea.rect.bottom -= mStateSettings.outlineWidth / 2.0f;

    mBackBrush.UpdatePosition(windowData->drawingArea.rect);
    mOutlineBrush.UpdatePosition(windowData->drawingArea.rect);
    mTextBrush.UpdatePosition(windowData->drawingArea.rect);
    mTextShadowBrush.UpdatePosition(windowData->drawingArea.rect);

    windowData->drawingArea.rect = mBackBrush.brushPosition;
}


void State::Load(StateSettings* defaultSettings, LPCTSTR prefix, Settings *settings)
{
    assert(!this->settings);
    this->settings = settings;

    mStateSettings.Load(this->settings, defaultSettings);

    mBackBrush.Load(&mStateSettings.backgroundBrush);
    mOutlineBrush.Load(&mStateSettings.outlineBrush);
    mTextBrush.Load(&mStateSettings.textBrush);
    mTextShadowBrush.Load(&mStateSettings.textDropShadowBrush);

    CreateTextFormat(this->textFormat);
}


void State::DiscardDeviceResources()
{
    mBackBrush.Discard();
    mOutlineBrush.Discard();
    mTextBrush.Discard();
    mTextShadowBrush.Discard();
}


void State::Paint(ID2D1RenderTarget* renderTarget, WindowData *windowData, Window *window)
{
    if (mBackBrush.brush)
    {
        if (mBackBrush.IsImageEdgeBrush())
        {
            for (Brush::EdgeType type = Brush::EdgeType(0); type != Brush::EdgeType::Count;
                type = Brush::EdgeType(std::underlying_type<Brush::EdgeType>::type(type) + 1))
            {
                renderTarget->FillRectangle(mBackBrush.GetImageEdgeRectAndScaleBrush(type), mBackBrush.brush);
            }
        }
        else
        {
            renderTarget->FillRoundedRectangle(windowData->drawingArea, mBackBrush.brush);
        }
    }
    if (mOutlineBrush.brush && mStateSettings.outlineWidth != 0)
    {
        renderTarget->DrawRoundedRectangle(windowData->drawingArea, mOutlineBrush.brush, mStateSettings.outlineWidth);
    }
    /*if (this->textShadowBrush->brush)
    {
        renderTarget->SetTransform(Matrix3x2F::Rotation(mStateSettings.textRotation, this->textRotationOrigin));
        renderTarget->DrawText(*this->text, lstrlenW(*this->text), this->textDropFormat, this->textArea, this->textShadowBrush->brush, D2D1_DRAW_TEXT_OPTIONS_CLIP);
        renderTarget->SetTransform(Matrix3x2F::Identity());
    }*/
    if (mTextBrush.brush && *window->GetText() != L'\0')
    {
        renderTarget->SetTransform(Matrix3x2F::Rotation(mStateSettings.textRotation, windowData->textRotationOrigin));
        renderTarget->DrawText(window->GetText(), lstrlenW(window->GetText()), this->textFormat, windowData->textArea, mTextBrush.brush, D2D1_DRAW_TEXT_OPTIONS_CLIP);
        renderTarget->SetTransform(Matrix3x2F::Identity());
    }
}


HRESULT State::ReCreateDeviceResources(ID2D1RenderTarget* renderTarget)
{
    mBackBrush.ReCreate(renderTarget);
    mOutlineBrush.ReCreate(renderTarget);
    mTextBrush.ReCreate(renderTarget);
    mTextShadowBrush.ReCreate(renderTarget);

    //this->drawingArea.rect = mBackBrush.brushPosition;
    return S_OK;
}


bool State::UpdateDWMColor(ARGB newColor, ID2D1RenderTarget *renderTarget)
{
    bool ret = false;
    ret = mBackBrush.UpdateDWMColor(newColor, renderTarget) || ret;
    ret = mOutlineBrush.UpdateDWMColor(newColor, renderTarget) || ret;
    ret = mTextBrush.UpdateDWMColor(newColor, renderTarget) || ret;

    return ret;
}


/// <summary>
/// Gets the "Desired" size of the window, given the specified constraints.
/// </summary>
/// <param name="maxWidth">Out. The maximum width to return.</param>
/// <param name="maxHeight">Out. The maximum height to return.</param>
/// <param name="size">Out. The desired size will be placed in this SIZE.</param>
void State::GetDesiredSize(int maxWidth, int maxHeight, LPSIZE size, Window *window)
{
    IDWriteFactory* factory = NULL;
    IDWriteTextLayout* textLayout = NULL;
    DWRITE_TEXT_METRICS metrics;
    maxWidth -= int(mStateSettings.textOffsetLeft + mStateSettings.textOffsetRight);
    maxHeight -= int(mStateSettings.textOffsetTop + mStateSettings.textOffsetBottom);

    Factories::GetDWriteFactory(reinterpret_cast<LPVOID*>(&factory));
    factory->CreateTextLayout(window->GetText(), lstrlenW(window->GetText()), this->textFormat, (float)maxWidth, (float)maxHeight, &textLayout);
    textLayout->GetMetrics(&metrics);
    SAFERELEASE(textLayout);

    size->cx = long(metrics.width + mStateSettings.textOffsetLeft + mStateSettings.textOffsetRight) + 1;
    size->cy = long(metrics.height + mStateSettings.textOffsetTop + mStateSettings.textOffsetBottom) + 1;
}


/// <summary>
/// Creates a textFormat based on the specified drawingSettings.
/// </summary>
/// <param name="drawingSettings">The settings to create the textformat with.</param>
/// <param name="textFormat">Out. The textformat.</param>
/// <returns>S_OK</returns>
HRESULT State::CreateTextFormat(IDWriteTextFormat *&textFormat)
{
    // Create the text format
    IDWriteFactory *pDWFactory = nullptr;
    Factories::GetDWriteFactory(reinterpret_cast<LPVOID*>(&pDWFactory));
    pDWFactory->CreateTextFormat(
        mStateSettings.font,
        nullptr,
        mStateSettings.fontWeight,
        mStateSettings.fontStyle,
        mStateSettings.fontStretch,
        mStateSettings.fontSize,
        L"en-US",
        &textFormat);

    textFormat->SetTextAlignment(mStateSettings.textAlign);
    textFormat->SetParagraphAlignment(mStateSettings.textVerticalAlign);
    textFormat->SetWordWrapping(mStateSettings.wordWrapping);
    textFormat->SetReadingDirection(mStateSettings.readingDirection);

    // Set the trimming method
    DWRITE_TRIMMING trimmingOptions;
    trimmingOptions.delimiter = 0;
    trimmingOptions.delimiterCount = 0;
    trimmingOptions.granularity = mStateSettings.textTrimmingGranularity;
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
void State::SetTextOffsets(float left, float top, float right, float bottom)
{
    mStateSettings.textOffsetBottom = bottom;
    mStateSettings.textOffsetLeft = left;
    mStateSettings.textOffsetRight = right;
    mStateSettings.textOffsetTop = top;

    //this->textArea = this->drawingArea.rect;
    //this->textArea.bottom -= mStateSettings.textOffsetBottom;
    //this->textArea.top += mStateSettings.textOffsetTop;
    //this->textArea.left += mStateSettings.textOffsetLeft;
    //this->textArea.right -= mStateSettings.textOffsetRight;
}


void State::SetCornerRadiusX(float radius)
{
    //this->drawingArea.radiusX = this->outlineArea.radiusX = radius;
}


void State::SetCornerRadiusY(float radius)
{
    //this->drawingArea.radiusY = this->outlineArea.radiusY = radius;
}


void State::SetOutlineWidth(float width)
{
    mStateSettings.outlineWidth = width;
}


void State::SetReadingDirection(DWRITE_READING_DIRECTION direction)
{
    this->textFormat->SetReadingDirection(direction);
}


void State::SetTextAlignment(DWRITE_TEXT_ALIGNMENT alignment)
{
    this->textFormat->SetTextAlignment(alignment);
}


void State::SetTextRotation(float rotation)
{
    mStateSettings.textRotation = rotation;
}


void State::SetTextTrimmingGranuality(DWRITE_TRIMMING_GRANULARITY granularity)
{
    DWRITE_TRIMMING options;
    IDWriteInlineObject *trimmingSign;
    this->textFormat->GetTrimming(&options, &trimmingSign);
    options.granularity = granularity;
    this->textFormat->SetTrimming(&options, trimmingSign);
}


void State::SetTextVerticalAlign(DWRITE_PARAGRAPH_ALIGNMENT alignment)
{
    this->textFormat->SetParagraphAlignment(alignment);
}


void State::SetWordWrapping(DWRITE_WORD_WRAPPING wrapping)
{
    this->textFormat->SetWordWrapping(wrapping);
}


Brush* State::GetBrush(LPCTSTR brushName)
{
    if (*brushName == _T('\0'))
    {
        return &mBackBrush;
    }
    else if (_tcsicmp(brushName, _T("Text")) == 0)
    {
        return &mTextBrush;
    }
    else if (_tcsicmp(brushName, _T("Outline")) == 0)
    {
        return &mOutlineBrush;
    }

    return nullptr;
}