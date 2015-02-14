#include "Pane.hpp"


void Pane::PaintChildren(ID2D1RenderTarget *renderTarget, const D2D1_RECT_F *area) const {
  for (const Pane* child : mChildren) {
    child->Paint(renderTarget, area);
  }
}
