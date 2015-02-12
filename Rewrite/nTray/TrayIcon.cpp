#include "TrayIcon.hpp"


TrayIcon::TrayIcon(IPane *parent) {
}


bool TrayIcon::GetScreenRect(LPRECT rect) {
  D2D1_RECT_F r;
  if (mPane->GetScreenPosition(&r)) {
    rect->left = (LONG)r.left;
    rect->top = (LONG)r.top;
    rect->right = (LONG)r.right;
    rect->bottom = (LONG)r.bottom;
    return true;
  }
  return false;
}
