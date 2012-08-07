/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *	FadeEffect.cpp												  August, 2012
 *	The nModules Project
 *
 *	Fades between two images.
 *      
 *													             Erik Welander
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "FadeEffect.hpp"

/// <summary>
///	Constructor
/// </summary>
FadeEffect::FadeEffect(FadeType fadeType) {
	m_fadeType = fadeType;
}

/// <summary>
///	Destructor
/// </summary>
FadeEffect::~FadeEffect() {
}

/// <summary>
///	Initializes the effect
/// </summary>
void FadeEffect::Initialize(TransitionSettings* transitionSettings) {
	m_pTransitionSettings = transitionSettings;
}

/// <summary>
///	Called just before a new painting session is about to begin
/// </summary>
void FadeEffect::Start(ID2D1BitmapBrush* oldBrush, ID2D1BitmapBrush* newBrush) {
	m_pOldBrush = oldBrush;
	m_pNewBrush = newBrush;
}

/// <summary>
///	Paints the state of the animation at progress to the specified rendertarget
/// </summary>
void FadeEffect::Paint(ID2D1RenderTarget* renderTarget, float fProgress) {
	switch (m_fadeType) {
	case FadeType::FADE_IN:
		m_pNewBrush->SetOpacity(fProgress);
		renderTarget->FillRectangle(m_pTransitionSettings->WPRect, m_pOldBrush);
		renderTarget->FillRectangle(m_pTransitionSettings->WPRect, m_pNewBrush);
		break;

	case FadeType::FADE_OUT:
		m_pOldBrush->SetOpacity(1.0f - fProgress);
		renderTarget->FillRectangle(m_pTransitionSettings->WPRect, m_pNewBrush);
		renderTarget->FillRectangle(m_pTransitionSettings->WPRect, m_pOldBrush);
		break;
	}
}

/// <summary>
///	End of the effect. Should cleanup.
/// </summary>
void FadeEffect::End() {
	m_pNewBrush->SetOpacity(1.0f);

	m_pOldBrush = NULL;
	m_pNewBrush = NULL;
}

/// <summary>
///	Called when the rendertarget has been resized
/// </summary>
void FadeEffect::Resize() {
}