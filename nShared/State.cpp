/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  WindowState.cpp
 *  The nModules Project
 *
 *  A state for a Window.
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "Color.h"
#include "Factories.h"
#include "LiteStep.h"
#include "State.hpp"

#include "../Utilities/CommonD2D.h"

#include <Wincodec.h>


using namespace D2D1;


State::State()
: settings(nullptr)
, textFormat(nullptr)
, mTextRender(new StateTextRender(this)) {
  this->settings = nullptr;
  this->textFormat = nullptr;
}


State::~State() {
  DiscardDeviceResources();
  SAFEDELETE(this->settings);
  SAFERELEASE(this->textFormat);
  SAFERELEASE(mTextRender);
}


void State::UpdatePosition(D2D1_RECT_F position, WindowData *windowData) {
  windowData->drawingArea.rect = position;

  windowData->drawingArea.radiusX = windowData->outlineArea.radiusX = mStateSettings.cornerRadiusX;
  windowData->drawingArea.radiusY = windowData->outlineArea.radiusY = mStateSettings.cornerRadiusY;

  windowData->textRotationOrigin.x = position.left + (position.right - position.left) / 2.0f;
  windowData->textRotationOrigin.y = position.top + (position.bottom - position.top) / 2.0f;

  windowData->textArea = windowData->drawingArea.rect;
  windowData->textArea.bottom -= mStateSettings.textOffsetBottom;
  windowData->textArea.top += mStateSettings.textOffsetTop;
  windowData->textArea.left += mStateSettings.textOffsetLeft;
  windowData->textArea.right -= mStateSettings.textOffsetRight;

  if (windowData->textLayout) {
    windowData->textLayout->SetMaxHeight(windowData->textArea.bottom - windowData->textArea.top);
    windowData->textLayout->SetMaxWidth(windowData->textArea.right - windowData->textArea.left);
  }

  // Adjust the drawing area to account for the outline.
  windowData->drawingArea.rect.left += mStateSettings.outlineWidth / 2.0f;
  windowData->drawingArea.rect.right -= mStateSettings.outlineWidth / 2.0f;
  windowData->drawingArea.rect.top += mStateSettings.outlineWidth / 2.0f;
  windowData->drawingArea.rect.bottom -= mStateSettings.outlineWidth / 2.0f;

  for (BrushType type = BrushType(0); type != BrushType::Count; EnumIncrement(type)) {
    mBrushes[type].UpdatePosition(windowData->drawingArea.rect, &windowData->brushData[type]);
  }
}


void State::Load(const Settings * defaultSettings, const ::Settings * settings, LPCTSTR name) {
  ASSERT(!this->settings);
  this->settings = settings;
  mName = name;

  mStateSettings.Load(this->settings, defaultSettings);

  for (BrushType type = BrushType(0); type != BrushType::Count; EnumIncrement(type)) {
    mBrushes[type].Load(&mStateSettings.brushSettings[type]);
  }

  CreateTextFormat(this->textFormat);
}


void State::DiscardDeviceResources() {
  for (Brush & brush : mBrushes) {
    brush.Discard();
  }
}


void State::Paint(ID2D1RenderTarget* renderTarget, WindowData *windowData) {
  if (mBrushes[BrushType::Background].brush) {
    if (mBrushes[BrushType::Background].IsImageEdgeBrush()) {
      for (Brush::EdgeType type = Brush::EdgeType(0); type != Brush::EdgeType::Count;
        type = Brush::EdgeType(std::underlying_type<Brush::EdgeType>::type(type) + 1)) {
        renderTarget->FillRectangle(mBrushes[BrushType::Background].GetImageEdgeRectAndScaleBrush(type,
          &windowData->brushData[BrushType::Background]), mBrushes[BrushType::Background].brush);
      }
    } else {
      mBrushes[BrushType::Background].brush->SetTransform(windowData->brushData[BrushType::Background].brushTransform);
      renderTarget->FillRoundedRectangle(windowData->drawingArea, mBrushes[BrushType::Background].brush);
    }
  }
  if (mBrushes[BrushType::Outline].brush && mStateSettings.outlineWidth != 0) {
    mBrushes[BrushType::Outline].brush->SetTransform(windowData->brushData[BrushType::Outline].brushTransform);
    renderTarget->DrawRoundedRectangle(windowData->drawingArea, mBrushes[BrushType::Outline].brush, mStateSettings.outlineWidth);
  }
}


void State::PaintText(ID2D1RenderTarget* renderTarget, WindowData *windowData, Window *window) {
  if (mBrushes[BrushType::Text].brush && *window->GetText() != L'\0') {
    renderTarget->SetTransform(Matrix3x2F::Rotation(mStateSettings.textRotation, windowData->textRotationOrigin));
    mBrushes[BrushType::Text].brush->SetTransform(windowData->brushData[BrushType::Text].brushTransform);

    // TODO::Avoid re-creation of the layout here.
    if (!windowData->textLayout) {
      IDWriteFactory *dwFactory;
      if (SUCCEEDED(Factories::GetDWriteFactory((LPVOID*)&dwFactory))) {
        dwFactory->CreateTextLayout(window->GetText(), lstrlenW(window->GetText()), textFormat,
          windowData->textArea.right - windowData->textArea.left,
          windowData->textArea.bottom - windowData->textArea.top, &windowData->textLayout);
      }
    }

    if (windowData->textLayout) {
      windowData->textLayout->Draw(renderTarget, mTextRender, windowData->textArea.left, windowData->textArea.top);
    }

    renderTarget->SetTransform(Matrix3x2F::Identity());
  }
}


HRESULT State::ReCreateDeviceResources(ID2D1RenderTarget* renderTarget) {
  HRESULT hr = S_OK;

  for (Brush & brush : mBrushes) {
    RETURNONFAIL(hr, brush.ReCreate(renderTarget));
  }

  //this->drawingArea.rect = mBackBrush.brushPosition;
  return hr;
}


bool State::UpdateDWMColor(ARGB newColor, ID2D1RenderTarget *renderTarget) {
  bool ret = false;
  ret = mBrushes[BrushType::Background].UpdateDWMColor(newColor, renderTarget) || ret;
  ret = mBrushes[BrushType::Outline].UpdateDWMColor(newColor, renderTarget) || ret;
  ret = mBrushes[BrushType::Text].UpdateDWMColor(newColor, renderTarget) || ret;
  ret = mBrushes[BrushType::TextStroke].UpdateDWMColor(newColor, renderTarget) || ret;

  return ret;
}


/// <summary>
/// Gets the "Desired" size of the window, given the specified constraints.
/// </summary>
/// <param name="maxWidth">Out. The maximum width to return.</param>
/// <param name="maxHeight">Out. The maximum height to return.</param>
/// <param name="size">Out. The desired size will be placed in this SIZE.</param>
void State::GetDesiredSize(int maxWidth, int maxHeight, LPSIZE size, Window *window) {
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
HRESULT State::CreateTextFormat(IDWriteTextFormat *&textFormat) {
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
void State::SetTextOffsets(float left, float top, float right, float bottom) {
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


void State::SetCornerRadiusX(float radius) {
  //this->drawingArea.radiusX = this->outlineArea.radiusX = radius;
}


void State::SetCornerRadiusY(float radius) {
  //this->drawingArea.radiusY = this->outlineArea.radiusY = radius;
}


void State::SetOutlineWidth(float width) {
  mStateSettings.outlineWidth = width;
}


void State::SetReadingDirection(DWRITE_READING_DIRECTION direction) {
  this->textFormat->SetReadingDirection(direction);
}


void State::SetTextAlignment(DWRITE_TEXT_ALIGNMENT alignment) {
  this->textFormat->SetTextAlignment(alignment);
}


void State::SetTextRotation(float rotation) {
  mStateSettings.textRotation = rotation;
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


Brush* State::GetBrush(LPCTSTR brushName) {
  if (*brushName == L'\0') {
    return &mBrushes[BrushType::Background];
  } else if (_wcsicmp(brushName, L"Text") == 0) {
    return &mBrushes[BrushType::Text];
  } else if (_wcsicmp(brushName, L"Outline") == 0) {
    return &mBrushes[BrushType::Outline];
  } else if (_wcsicmp(brushName, L"TextStroke") == 0) {
    return &mBrushes[BrushType::TextStroke];
  }

  return nullptr;
}
