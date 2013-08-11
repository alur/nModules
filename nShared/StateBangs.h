//--------------------------------------------------------------------------------------
// StateBangs.h
// The nModules Project
//
// Bangs for states
//
//--------------------------------------------------------------------------------------
#pragma once

#include "Window.hpp"
#include <functional>

namespace StateBangs
{
    void Register(LPCTSTR prefix, std::function<Window* (LPCTSTR)> windowFinder);
    void UnRegister(LPCTSTR prefix);
}
