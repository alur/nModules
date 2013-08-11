
#include "Drawable.hpp"

/// <summary>
/// Creates a new top-level drawable.
/// </summary>
Drawable::Drawable(LPCTSTR prefix)
    : IDrawable(prefix)
{
}


/// <summary>
/// Creates a new top-level drawable, derived from the specified settings.
/// E.g. tooltips, balloons, ... These drawables can not have parents.
/// </summary>
Drawable::Drawable(LPCTSTR prefix, Settings* parentSettings)
    : IDrawable(prefix, parentSettings)
{
}


/// <summary>
/// Creates a new child drawable.
/// </summary>
/// <param name="parent">The Drawable which this Drawable will be a child of.</param>
/// <param name="windowClass">The window class of this drawable.</param>
/// <param name="prefix">The prefix to append to the parents group.</param>
/// <param name="independent">If true, this Drawable's settings will not be a child of its parent's settings.</param>
Drawable::Drawable(IDrawable* parent, LPCTSTR prefix, bool independent)
    : IDrawable(parent, prefix, independent)
{
}


/// <summary>
/// Lets go of all drawable member variables.
/// </summary>
Drawable::~Drawable()
{
}