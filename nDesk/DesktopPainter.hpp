/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *	DesktopPainter.hpp												July, 2012
 *	The nModules Project
 *
 *	Function declarations for the DesktopPainter class.
 *      
 *													             Erik Welander
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#ifndef DESKTOPPAINTER_HPP
#define DESKTOPPAINTER_HPP

#include <d2d1.h>
#include "TransitionEffects.h"

class DesktopPainter {
public:
	// Available transition types
	enum TransitionType {
		NONE,

		FADE_IN,
		FADE_OUT,

		SLIDE_BOTH_LEFT,
		SLIDE_BOTH_RIGHT,
		SLIDE_BOTH_UP,
		SLIDE_BOTH_DOWN,
		SLIDE_IN_LEFT,
		SLIDE_IN_RIGHT,
		SLIDE_IN_UP,
		SLIDE_IN_DOWN,
		SLIDE_OUT_LEFT,
		SLIDE_OUT_RIGHT,
		SLIDE_OUT_UP,
		SLIDE_OUT_DOWN,

		SCAN_LEFT,
		SCAN_RIGHT,
		SCAN_UP,
		SCAN_DOWN,

		SQUARES_RANDOM_IN,
		SQUARES_RANDOM_OUT,
		SQUARES_LINEAR_VERTICAL_IN,
		SQUARES_LINEAR_VERTICAL_OUT,
		SQUARES_LINEAR_HORIZONTAL_IN,
		SQUARES_LINEAR_HORIZONTAL_OUT,

		CLOCKWISE_SQUARES,
		COUNTERCLOCKWISE_SQUARE,
		LINEAR_SQUARES_TOP_LEFT,
		LINEAR_SQUARES_TOP_RIGHT,
		LINEAR_SQUARES_BOTTOM_LEFT,
		LINEAR_SQUARES_BOTTOM_RIGHT,
		TRIANGLE_SQUARES_TOP_LEFT,
		TRIANGLE_SQUARES_TOP_RIGHT
	};

	// Available easing types
	enum EasingType {
		LINEAR,
		INQUAD,
		OUTQUAD,
		INOUTQUAD,
		INCUBIC,
		OUTQUBIC,
		INOUTCUBIC
	};

	explicit DesktopPainter(HWND);
	virtual ~DesktopPainter();

	void SetTransitionType(TransitionType);
	void SetTransitionTime(int);
	void SetFrameInterval(int);
	void SetSquareSize(int);

	void TransitionStep();
	
	void UpdateWallpaper(bool bNoTransition = false);
	void Resize();
	LRESULT HandleMessage(HWND, UINT, WPARAM, LPARAM);

private:
	void CalculateSizeDepdenentStuff();
	HRESULT ReCreateDeviceResources();
	void DiscardDeviceResources();

	void Paint();
	void PaintComposite();
	void Redraw();

	void TransitionStart();
	void TransitionEnd();
	TransitionEffect* TransitionEffectFromType(TransitionType transitionType);

	HRESULT CreateWallpaperBrush(ID2D1BitmapBrush** ppBitmapBrush);
	
	//
	HWND m_hWnd;

	// Direct2D targets
	ID2D1HwndRenderTarget* m_pRenderTarget;
	ID2D1BitmapBrush* m_pWallpaperBrush;
	ID2D1BitmapBrush* m_pOldWallpaperBrush;

	// The type of transition we should use
	TransitionType m_TransitionType;

	// The transition effect currently in use
	TransitionEffect* m_TransitionEffect;

	// Holds all settings for transitions. Passed in to the transitions on init.
	TransitionEffect::TransitionSettings m_TransitionSettings;
};

#endif /* DESKTOPPAINTER_HPP */