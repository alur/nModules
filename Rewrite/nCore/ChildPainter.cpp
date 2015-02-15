#include "Api.h"
#include "ChildPainter.hpp"
#include "Pane.hpp"


EXPORT_CDECL(IPainter*) GetChildPainter() {
  static ChildPainter childPainter;
  return &childPainter;
}


LPVOID ChildPainter::AddPane(const IPane*) {
  return nullptr;
}


HRESULT ChildPainter::CreateDeviceResources(ID2D1RenderTarget*) {
  return S_OK;
}


void ChildPainter::DiscardDeviceResources() {}


bool ChildPainter::DynamicColorChanged(ID2D1RenderTarget*) {
  return false;
}


void ChildPainter::Paint(ID2D1RenderTarget *renderTarget, const D2D1_RECT_F *area,
    const IPane *pane, LPVOID, UINT) const {
  ((Pane*)pane)->PaintChildren(renderTarget, area);
}


void ChildPainter::PaintTransform(ID2D1RenderTarget *renderTarget, const D2D1_RECT_F *area,
    const class IPane *pane, LPVOID painterData, UINT, UINT newState, float) const {
  Paint(renderTarget, area, pane, painterData, newState);
}


void ChildPainter::PositionChanged(const IPane*, LPVOID, const D2D1_RECT_F&, bool, bool) {
}


void ChildPainter::RemovePane(const IPane*, LPVOID) {}


void ChildPainter::TextChanged(const IPane*, LPVOID, LPCWSTR) {}
