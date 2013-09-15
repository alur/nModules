/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  StateTextRender.hpp
 *  The nModules Project
 *
 *  Renders a state's text.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "../Utilities/Common.h"
#include <dwrite.h>
#include "State.hpp"

class StateTextRender : public IDWriteTextRenderer
{
public:
    StateTextRender(State *);

private:
    ~StateTextRender();

    // IDWriteTextRender
public:
    HRESULT DrawGlyphRun(
        LPVOID clientDrawingContext,
        FLOAT baselineOriginX,
        FLOAT baselineOriginY,
        DWRITE_MEASURING_MODE  measuringMode,
        const DWRITE_GLYPH_RUN * glyphRun,
        const DWRITE_GLYPH_RUN_DESCRIPTION * glyphRunDescription,
        LPUNKNOWN clientDrawingEffect
    ) override;

    HRESULT DrawInlineObject(
        LPVOID clientDrawingContext,
        FLOAT originX,
        FLOAT originY,
        IDWriteInlineObject * inlineObject,
        BOOL isSideways,
        BOOL isRightToLeft,
        LPUNKNOWN clientDrawingEffect
    ) override;

    HRESULT DrawStrikethrough(
        LPVOID clientDrawingContext,
        FLOAT baselineOriginX,
        FLOAT baselineOriginY,
        const DWRITE_STRIKETHROUGH * strikethrough,
        LPUNKNOWN clientDrawingEffect
    ) override;

    HRESULT DrawUnderline(
        LPVOID clientDrawingContext,
        FLOAT baselineOriginX,
        FLOAT baselineOriginY,
        const DWRITE_UNDERLINE * underline,
        LPUNKNOWN clientDrawingEffect
    ) override;

    // IDWritePixelSnapping
public:
    HRESULT GetCurrentTransform(
        LPVOID clientDrawingContext,
        DWRITE_MATRIX * transform
    ) override;

    HRESULT GetPixelsPerDip(
        LPVOID clientDrawingContext,
        PFLOAT pixelsPerDip
    ) override;

    HRESULT IsPixelSnappingDisabled(
        LPVOID clientDrawingContext,
        LPBOOL isDisabled
    ) override;

    // IUnknown
public:
    ULONG AddRef() override;
    ULONG Release() override;
    HRESULT QueryInterface(REFIID riid, void **ppvObject) override;

private:
    ULONG mRefCount;
    State *mState;
};
