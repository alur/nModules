#pragma once

#include "Popup.hpp"

class ContentPopup : public Popup {
public:
  ContentPopup(LPCWSTR title, LPCWSTR prefix);
  ~ContentPopup();
};
