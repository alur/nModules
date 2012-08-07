/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *	PaintSettings.hpp												July, 2012
 *	The nModules Project
 *
 *	Function declarations for the PaintSettings class.
 *      
 *													             Erik Welander
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#ifndef PAINTSETTINGS_HPP
#define PAINTSETTINGS_HPP

#include <d2d1.h>
#include <dwrite.h>

class PaintSettings {
public:
	explicit PaintSettings(LPCSTR pszPrefix);
	virtual ~PaintSettings();

	void Load();
	void OverLoad(LPCSTR pszPrefix);

	RECT position;
	bool DWMBlur;
	D2D_COLOR_F backColor;

	LPCWSTR text;
	LPCWSTR font;
	D2D_COLOR_F fontColor;
	float fontSize;

	D2D_RECT_F textOffset;

	DWRITE_TEXT_ALIGNMENT textAlignment;
	DWRITE_PARAGRAPH_ALIGNMENT textVerticalAlignment;

	void setText(LPCWSTR);

private:
	LPCSTR m_pszPrefix;

	D2D_COLOR_F ARGBToD2DColor(DWORD argb);
};

#endif /* PAINTSETTINGS_HPP */