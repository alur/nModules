//--------------------------------------------------------------------------------------
// StateBangs.h
// The nModules Project
//
// Bangs for states
//
//--------------------------------------------------------------------------------------
#pragma once

#include "DrawableWindow.hpp"
#include <functional>

namespace StateBangs
{
    void Register(LPCTSTR prefix, std::function<DrawableWindow* (LPCTSTR)> windowFinder);
    void UnRegister(LPCTSTR prefix);
}
