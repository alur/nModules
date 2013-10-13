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
    HRESULT STDMETHODCALLTYPE DrawGlyphRun(
        LPVOID clientDrawingContext,
        FLOAT baselineOriginX,
        FLOAT baselineOriginY,
        DWRITE_MEASURING_MODE  measuringMode,
        const DWRITE_GLYPH_RUN * glyphRun,
        const DWRITE_GLYPH_RUN_DESCRIPTION * glyphRunDescription,
        LPUNKNOWN clientDrawingEffect
    ) override;

    HRESULT STDMETHODCALLTYPE DrawInlineObject(
        LPVOID clientDrawingContext,
        FLOAT originX,
        FLOAT originY,
        IDWriteInlineObject * inlineObject,
        BOOL isSideways,
        BOOL isRightToLeft,
        LPUNKNOWN clientDrawingEffect
    ) override;

    HRESULT STDMETHODCALLTYPE DrawStrikethrough(
        LPVOID clientDrawingContext,
        FLOAT baselineOriginX,
        FLOAT baselineOriginY,
        const DWRITE_STRIKETHROUGH * strikethrough,
        LPUNKNOWN clientDrawingEffect
    ) override;

    HRESULT STDMETHODCALLTYPE DrawUnderline(
        LPVOID clientDrawingContext,
        FLOAT baselineOriginX,
        FLOAT baselineOriginY,
        const DWRITE_UNDERLINE * underline,
        LPUNKNOWN clientDrawingEffect
    ) override;

    // IDWritePixelSnapping
public:
    HRESULT STDMETHODCALLTYPE GetCurrentTransform(
        LPVOID clientDrawingContext,
        DWRITE_MATRIX * transform
    ) override;

    HRESULT STDMETHODCALLTYPE GetPixelsPerDip(
        LPVOID clientDrawingContext,
        PFLOAT pixelsPerDip
    ) override;

    HRESULT STDMETHODCALLTYPE IsPixelSnappingDisabled(
        LPVOID clientDrawingContext,
        LPBOOL isDisabled
    ) override;

    // IUnknown
public:
    ULONG STDMETHODCALLTYPE AddRef() override;
    ULONG STDMETHODCALLTYPE Release() override;
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject) override;

private:
    ULONG mRefCount;
    State *mState;
};
