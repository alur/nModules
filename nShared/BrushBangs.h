//--------------------------------------------------------------------------------------
// BrushBangs.h
// The nModules Project
//
// Bangs for brushes
//
//--------------------------------------------------------------------------------------
#pragma once

#include "DrawableWindow.hpp"
#include <functional>

namespace BrushBangs
{
    void Register(LPCTSTR prefix, std::function<DrawableWindow* (LPCTSTR)> windowFinder);
    void UnRegister(LPCTSTR prefix);
}
