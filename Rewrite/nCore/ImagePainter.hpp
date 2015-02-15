#pragma once

#include "../nCoreApi/IImagePainter.hpp"

class ImagePainter : public IImagePainter {
public:
  ImagePainter();
  ~ImagePainter();

  // IDiscardable
public:
  void APICALL Discard() override;

  // IPainter
public:
  LPVOID APICALL AddPane(const IPane *pane) override;
  HRESULT APICALL CreateDeviceResources(ID2D1RenderTarget *renderTarget) override;
  void APICALL DiscardDeviceResources() override;
  bool APICALL DynamicColorChanged(ID2D1RenderTarget *renderTarget) override;
  void APICALL Paint(ID2D1RenderTarget *renderTarget, const D2D1_RECT_F *area, const IPane *pane,
    LPVOID painterData, UINT state) const override;
  void APICALL PaintTransform(ID2D1RenderTarget *renderTarget, const D2D1_RECT_F *area,
    const class IPane *pane, LPVOID painterData, UINT oldState, UINT newState, float time) const
    override;
  void APICALL PositionChanged(const IPane *pane, LPVOID painterData,
    const D2D1_RECT_F &position, bool isMove, bool isSize) override;
  void APICALL RemovePane(const IPane *pane, LPVOID painterData) override;
  void APICALL TextChanged(const IPane *pane, LPVOID painterData, LPCWSTR text) override;

  // IImagePainter
public:
  void APICALL SetPosition(NRECT position, const IPane *pane) override;
  void APICALL SetImage(HICON icon) override;
  void APICALL SetImage(HBITMAP bitmap) override;

private:
  void SetImage(IWICBitmapSource *image);
  D2D1_MATRIX_3X2_F GetTransform() const;

private:
  IWICBitmapSource *mImage;
  ID2D1BitmapBrush *mBrush;
  ID2D1RenderTarget *mRenderTarget;

  D2D1_RECT_F mPaintingPosition;
  D2D1_SIZE_U mImageSize;
  NRECT mPosition;
};
