/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *	TransitionEffect.hpp										  August, 2012
 *	The nModules Project
 *
 *	Function declarations for the TransitionEffect abstract class.
 *      
 *													             Erik Welander
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#ifndef TRANSITIONEFFECT_HPP
#define TRANSITIONEFFECT_HPP

#include <d2d1.h>

class TransitionEffect {
public:
	typedef struct TransitionSettings {
		// The number of milliseconds the transition should take
		int iTime;

		// How many milliseconds into the transition we are
		int iProgress;

		// How often to repaint during a transition
		int iFrameInterval;

		// The size of the squares used during square transitions
		int iSquareSize;

		// For animations that fade different parts at different times, how long the fading of an induvidual part should take.
		float fFadeTime;

		// The rect encompassing the entire screen
		D2D1_RECT_F WPRect;

	} TransitionSettings;

	virtual void Initialize(TransitionSettings* transitionSettings) = 0;
	virtual void Start(ID2D1BitmapBrush* oldBrush, ID2D1BitmapBrush* newBrush) = 0;
	virtual void End() = 0;
	virtual void Paint(ID2D1RenderTarget* renderTarget, float progress) = 0;
	virtual void Resize() = 0;

protected:
	//
	TransitionSettings* m_pTransitionSettings;

	//
	ID2D1BitmapBrush* m_pOldBrush;
	
	//
	ID2D1BitmapBrush* m_pNewBrush;
};

#endif /* TRANSITIONEFFECT_HPP */