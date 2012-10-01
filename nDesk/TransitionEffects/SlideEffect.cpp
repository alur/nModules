/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  SlideEffect.cpp
 *  The nModules Project
 *
 *  Slides in/out
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "SlideEffect.hpp"


/// <summary>
/// Constructor
/// </summary>
SlideEffect::SlideEffect(SlideType slideType, SlideDirection slideDirection) {
    m_slideType = slideType;
    m_SlideDirection = slideDirection;
    m_bSlideOld = (slideType == BOTH || slideType == OLD);
    m_bSlideNew = (slideType == BOTH || slideType == NEW);
}


/// <summary>
/// Destructor
/// </summary>
SlideEffect::~SlideEffect() {
}


/// <summary>
/// Initializes the effect
/// </summary>
void SlideEffect::Initialize(TransitionSettings* transitionSettings) {
    m_pTransitionSettings = transitionSettings;
    Resize();
}


/// <summary>
/// Called just before a new painting session is about to begin
/// </summary>
void SlideEffect::Start(ID2D1BitmapBrush* oldBrush, ID2D1BitmapBrush* newBrush) {
    m_pNewBrush = newBrush;
    m_pOldBrush = oldBrush;
}


/// <summary>
/// Paints the state of the animation at progress to the specified rendertarget
/// </summary>
void SlideEffect::Paint(ID2D1RenderTarget* renderTarget, float fProgress) {
    // Work out the rectangles for this frame and translate the brushes
    switch (m_SlideDirection) {
    case LEFT:
        m_newRect.left = m_oldRect.right = m_pTransitionSettings->WPRect.right * (1.0f - fProgress);
        if (m_bSlideNew)
            m_pNewBrush->SetTransform(D2D1::Matrix3x2F::Translation(m_newRect.left, 0));
        if (m_slideType != NEW)
            m_pOldBrush->SetTransform(D2D1::Matrix3x2F::Translation(m_oldRect.right - m_pTransitionSettings->WPRect.right, 0));
        break;

    case RIGHT:
        m_newRect.right = m_oldRect.left = m_pTransitionSettings->WPRect.right * fProgress;
        if (m_bSlideNew)
            m_pNewBrush->SetTransform(D2D1::Matrix3x2F::Translation(m_newRect.right - m_pTransitionSettings->WPRect.right, 0));
        if (m_bSlideOld)
            m_pOldBrush->SetTransform(D2D1::Matrix3x2F::Translation(m_oldRect.left, 0));
        break;

    case UP:
        m_newRect.top = m_oldRect.bottom = m_pTransitionSettings->WPRect.bottom * (1.0f - fProgress);
        if (m_bSlideNew)
            m_pNewBrush->SetTransform(D2D1::Matrix3x2F::Translation(0, m_newRect.top));
        if (m_bSlideOld)
            m_pOldBrush->SetTransform(D2D1::Matrix3x2F::Translation(0, m_oldRect.bottom - m_pTransitionSettings->WPRect.bottom));
        break;

    case DOWN:
        m_newRect.bottom = m_oldRect.top = m_pTransitionSettings->WPRect.bottom * fProgress;
        if (m_bSlideNew)
            m_pNewBrush->SetTransform(D2D1::Matrix3x2F::Translation(0, m_newRect.bottom - m_pTransitionSettings->WPRect.bottom));
        if (m_bSlideOld)
            m_pOldBrush->SetTransform(D2D1::Matrix3x2F::Translation(0, m_oldRect.top));
        break;
    }

    // Paint the frame
    renderTarget->FillRectangle(m_oldRect, m_pOldBrush);
    renderTarget->FillRectangle(m_newRect, m_pNewBrush);
}


/// <summary>
/// End of the effect. Should cleanup.
/// </summary>
void SlideEffect::End() {
    m_pNewBrush->SetTransform(D2D1::Matrix3x2F::Translation(0.0f, 0.0f));

    m_pNewBrush = NULL;
    m_pOldBrush = NULL;
}


/// <summary>
/// Called when the rendertarget has been resized
/// </summary>
void SlideEffect::Resize() {
    m_newRect = m_pTransitionSettings->WPRect;
    m_oldRect = m_pTransitionSettings->WPRect;
}
