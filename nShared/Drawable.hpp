#pragma once

#include "IDrawable.hpp"

class Drawable : public IDrawable
{
public:
    explicit Drawable(LPCTSTR prefix);
    explicit Drawable(IDrawable *parent, LPCTSTR prefix, bool independent = false);
    explicit Drawable(LPCTSTR prefix, LPSettings parentSettings);

protected:
    ~Drawable();
};
