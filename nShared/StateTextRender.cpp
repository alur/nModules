/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  StateTextRender.cpp
 *  The nModules Project
 *
 *  Renders a state's text.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "StateTextRender.hpp"
#include "Factories.h"


/// <summary>
/// Constructor
/// </summary>
StateTextRender::StateTextRender(State *state)
    : mRefCount(1)
    , mState(state)
{
}


/// <summary>
/// Destructor
/// </summary>
StateTextRender::~StateTextRender()
{
}


/// <summary>
/// IUnknown::AddRef
/// Increments the reference count for an interface on an object.
/// </summary>
ULONG StateTextRender::AddRef()
{
    return InterlockedIncrement(&mRefCount);
}


/// <summary>
/// IUnknown::Release
/// Decrements the reference count for an interface on an object.
/// </summary>
ULONG StateTextRender::Release()
{
    if (InterlockedDecrement(&mRefCount) == 0)
    {
        delete this;
        return 0;
    }

    return mRefCount;
}


/// <summary>
/// IUnknown::QueryInterface
/// Retrieves pointers to the supported interfaces on an object.
/// </summary>
HRESULT StateTextRender::QueryInterface(REFIID riid, void **ppvObject)
{
    if (ppvObject == nullptr)
    {
        return E_POINTER;
    }

    if (riid == __uuidof(IDWriteTextRenderer))
    {
        *ppvObject = this;
    }
    else if (riid == __uuidof(IDWritePixelSnapping))
    {
        *ppvObject = (IDWritePixelSnapping*)this;
    }
    else if (riid == IID_IUnknown)
    {
        *ppvObject = (IUnknown*)this;
    }
    else
    {
        *ppvObject = nullptr;
        return E_NOINTERFACE;
    }

    AddRef();
    return S_OK;
}


/// <summary>
/// IDWritePixelSnapping::GetCurrentTransform
/// Returns the current transform applied to the render target.
/// </summary>
HRESULT StateTextRender::GetCurrentTransform(
    LPVOID clientDrawingContext,
    DWRITE_MATRIX * transform)
{
    if (transform == nullptr)
    {
        return E_POINTER;
    }

    ((ID2D1RenderTarget*)clientDrawingContext)->GetTransform(reinterpret_cast<D2D1_MATRIX_3X2_F*>(transform));

    return S_OK;
}


/// <summary>
/// IDWritePixelSnapping::GetPixelsPerDip
/// This returns the number of pixels per DIP.
/// </summary>
HRESULT StateTextRender::GetPixelsPerDip(
    LPVOID clientDrawingContext,
    PFLOAT pixelsPerDip)
{
    if (pixelsPerDip == nullptr)
    {
        return E_POINTER;
    }

    FLOAT x, y;

    ((ID2D1RenderTarget*)clientDrawingContext)->GetDpi(&x, &y);

    *pixelsPerDip = x / 96.0f;

    return S_OK;
}


/// <summary>
/// IDWritePixelSnapping::IsPixelSnappingDisabled
/// Determines whether pixel snapping is disabled.
/// </summary>
HRESULT StateTextRender::IsPixelSnappingDisabled(
    LPVOID clientDrawingContext,
    LPBOOL isDisabled)
{
    UNREFERENCED_PARAMETER(clientDrawingContext);

    if (isDisabled == nullptr)
    {
        return E_POINTER;
    }

    *isDisabled = FALSE;
    return S_OK;
}


/// <summary>
/// IDWriteTextRenderer::DrawGlyphRun
/// Draws glyphs
/// </summary>
HRESULT StateTextRender::DrawGlyphRun(
        LPVOID clientDrawingContext,
        FLOAT baselineOriginX,
        FLOAT baselineOriginY,
        DWRITE_MEASURING_MODE measuringMode,
        const DWRITE_GLYPH_RUN * glyphRun,
        const DWRITE_GLYPH_RUN_DESCRIPTION * glyphRunDescription,
        LPUNKNOWN clientDrawingEffect)
{
    UNREFERENCED_PARAMETER(glyphRunDescription);
    UNREFERENCED_PARAMETER(clientDrawingEffect);

    HRESULT hr = S_OK;

    auto renderTarget = ((ID2D1RenderTarget*) clientDrawingContext);

    // Since the fill looks horrid anyways, only do this if we are
    // going to stroke, and fill using D2D's DrawGlyphRun
    if (mState->mStateSettings.fontStrokeWidth > 0.0f)
    {
        // Get our D2D factory
        ID2D1Factory *d2dFactory;
        renderTarget->GetFactory(&d2dFactory);

        // Create the path geometry.
        ID2D1PathGeometry *pPathGeometry = nullptr;
        hr = d2dFactory->CreatePathGeometry(&pPathGeometry);

        // Write to the path geometry using the geometry sink.
        ID2D1GeometrySink* sink = nullptr;
        if (SUCCEEDED(hr))
        {
            hr = pPathGeometry->Open(&sink);
        }

        // Get the glyph run outline geometries back from DirectWrite and place them within the geometry sink.
        if (SUCCEEDED(hr))
        {
            hr = glyphRun->fontFace->GetGlyphRunOutline(
                glyphRun->fontEmSize,
                glyphRun->glyphIndices,
                glyphRun->glyphAdvances,
                glyphRun->glyphOffsets,
                glyphRun->glyphCount,
                glyphRun->isSideways,
                glyphRun->bidiLevel % 2,
                sink
            );
        }

        // Close the geometry sink
        if (SUCCEEDED(hr))
        {
            hr = sink->Close();
        }

        // Initialize a matrix to translate the origin of the glyph run.
        D2D1::Matrix3x2F const matrix = D2D1::Matrix3x2F(
            1.0f, 0.0f,
            0.0f, 1.0f,
            baselineOriginX, baselineOriginY
            );

        // Create the transformed geometry
        ID2D1TransformedGeometry* pTransformedGeometry = nullptr;
        if (SUCCEEDED(hr))
        {
            hr = d2dFactory->CreateTransformedGeometry(
                pPathGeometry,
                &matrix,
                &pTransformedGeometry
                );
        }

        ID2D1StrokeStyle* strokeStyle = nullptr;
        if (SUCCEEDED(hr))
        {
            d2dFactory->CreateStrokeStyle(
                D2D1::StrokeStyleProperties(
                    D2D1_CAP_STYLE_FLAT,
                    D2D1_CAP_STYLE_FLAT,
                    D2D1_CAP_STYLE_SQUARE,
                    D2D1_LINE_JOIN_BEVEL,
                    5.0f,
                    D2D1_DASH_STYLE_SOLID,
                    0.0f
                ),
                nullptr,
                0,
                &strokeStyle);
        }

        if (SUCCEEDED(hr))
        {

            float strokeWidth = mState->mStateSettings.fontStrokeWidth;
            if (strokeWidth != 0.0f)
            {
                // Draw the outline of the glyph run
                renderTarget->DrawGeometry(
                    pTransformedGeometry,
                    mState->mBrushes[State::BrushType::TextStroke].brush,
                    strokeWidth,
                    strokeStyle
                    );
            }

            // Fill in the glyph run
            // This is so ugly...
            /*renderTarget->FillGeometry(
                pTransformedGeometry,
                mState->mBrushes[State::BrushType::Text].brush
                );*/
        }
    
        SAFERELEASE(strokeStyle);
        SAFERELEASE(pPathGeometry);
        SAFERELEASE(sink);
        SAFERELEASE(pTransformedGeometry);
        d2dFactory->Release();
    }

    // TODO::Figure out how to replicate the quality of D2Ds DrawGlyphRun!
    renderTarget->DrawGlyphRun(D2D1::Point2F(baselineOriginX, baselineOriginY), glyphRun,
        mState->mBrushes[State::BrushType::Text].brush, measuringMode);

    return hr;
}


/// <summary>
/// IDWriteTextRenderer::DrawInlineObject
/// Draws an inline object.
/// </summary>
HRESULT StateTextRender::DrawInlineObject(
        LPVOID clientDrawingContext,
        FLOAT originX,
        FLOAT originY,
        IDWriteInlineObject * inlineObject,
        BOOL isSideways,
        BOOL isRightToLeft,
        LPUNKNOWN clientDrawingEffect)
{
    if (inlineObject == nullptr)
    {
        return E_POINTER;
    }

    return inlineObject->Draw(clientDrawingContext, this, originX, originY, isSideways, isRightToLeft, clientDrawingEffect);
}


/// <summary>
/// IDWriteTextRenderer::DrawStrikethrough
/// Draws a strikethrough.
/// </summary>
HRESULT StateTextRender::DrawStrikethrough(
        LPVOID clientDrawingContext,
        FLOAT baselineOriginX,
        FLOAT baselineOriginY,
        const DWRITE_STRIKETHROUGH * strikethrough,
        LPUNKNOWN clientDrawingEffect)
{
    UNREFERENCED_PARAMETER(clientDrawingEffect);

    D2D1_RECT_F rect = D2D1::RectF(
        baselineOriginX,
        baselineOriginY + strikethrough->offset,
        baselineOriginX + strikethrough->width,
        baselineOriginY + strikethrough->offset + strikethrough->thickness
        );

    auto renderTarget = ((ID2D1RenderTarget*) clientDrawingContext);

    renderTarget->DrawRectangle(rect, mState->mBrushes[State::BrushType::TextStroke].brush, mState->mStateSettings.fontStrokeWidth);
    renderTarget->FillRectangle(rect, mState->mBrushes[State::BrushType::Text].brush);

    return S_OK;
}


/// <summary>
/// IDWriteTextRenderer::DrawUnderline
/// Draws an underline.
/// </summary>
HRESULT StateTextRender::DrawUnderline(
        LPVOID clientDrawingContext,
        FLOAT baselineOriginX,
        FLOAT baselineOriginY,
        const DWRITE_UNDERLINE * underline,
        LPUNKNOWN clientDrawingEffect)
{
    UNREFERENCED_PARAMETER(clientDrawingEffect);

    D2D1_RECT_F rect = D2D1::RectF(
        baselineOriginX,
        baselineOriginY + underline->offset,
        baselineOriginX + underline->width,
        baselineOriginY + underline->offset + underline->thickness
        );

    auto renderTarget = ((ID2D1RenderTarget*) clientDrawingContext);

    renderTarget->DrawRectangle(rect, mState->mBrushes[State::BrushType::TextStroke].brush, mState->mStateSettings.fontStrokeWidth);
    renderTarget->FillRectangle(rect, mState->mBrushes[State::BrushType::Text].brush);

    return S_OK;
}
