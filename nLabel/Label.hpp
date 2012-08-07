/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *	Label.hpp														July, 2012
 *	The nModules Project
 *
 *	Declaration of the Label class.
 *      
 *													             Erik Welander
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#ifndef LABEL_HPP
#define LABEL_HPP

#include "../nShared/DrawableWindow.hpp"
#include <gdiplus.h>

class Label {
public:
	explicit Label(LPCSTR);
	virtual ~Label();

	void LoadSettings(bool = false);
	void CreateLabelWindow();
	LRESULT WINAPI HandleMessage(UINT, WPARAM, LPARAM);

private:
	// The name of this label
	LPCSTR m_pszName;

	// The label's window
	DrawableWindow* m_pWindow;

	// PaintSettings
	PaintSettings* m_pPaintSettings;

	// The parent window
	HWND m_hWndParent;

	// The label's position, relative to the parent.
	RECT m_recPosition;

	// The label's background color
	Gdiplus::ARGB m_backColor;

	// True if the label should have DWM blur applied to it
	bool m_bDWMBlur;
};

#endif /* LABEL_HPP */