//--------------------------------------------------------------------------------------
// BrushBangs.h
// The nModules Project
//
// Bangs for brushes
//
//--------------------------------------------------------------------------------------
#pragma once

#include "Window.hpp"
#include <functional>

namespace BrushBangs
{
    void Register(LPCTSTR prefix, std::function<Window* (LPCTSTR)> windowFinder);
    void UnRegister(LPCTSTR prefix);
}
