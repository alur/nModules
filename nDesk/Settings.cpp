/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *	Settings.cpp												  August, 2012
 *	The nModules Project
 *
 *	
 *      
 *													             Erik Welander
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../headers/lsapi.h"
#include "../nCoreCom/Core.h"
#include "Settings.h"

extern DesktopPainter* g_pDesktopPainter;

void Settings::Load() {
	using namespace nCore::InputParsing;

	char buf[MAX_LINE_LENGTH];

	// Defaults to 625ms
	g_pDesktopPainter->SetTransitionTime(GetRCInt("nDeskTransitionDuration", 625));

	// Defaults to 16ms per frame -> 62.5fps
	g_pDesktopPainter->SetFrameInterval(GetRCInt("nDeskTransitionFrameInterval", 16));

	//
	g_pDesktopPainter->SetSquareSize(GetRCInt("nDeskTransitionSquareSize", 150));

	// 
	GetRCString("nDeskTransitionEffect",  buf, "FadeOut", sizeof(buf));
	g_pDesktopPainter->SetTransitionType(TransitionTypeFromString(buf));
}


DesktopPainter::EasingType Settings::EasingFromString(LPCSTR pszEasing) {
	return DesktopPainter::EasingType::LINEAR;
}


DesktopPainter::TransitionType Settings::TransitionTypeFromString(LPCSTR pszTransition) {
	if (_stricmp(pszTransition, "FadeIn") == 0) return DesktopPainter::TransitionType::FADE_IN;
	if (_stricmp(pszTransition, "FadeOut") == 0) return DesktopPainter::TransitionType::FADE_OUT;

	if (_stricmp(pszTransition, "SlideLeft") == 0) return DesktopPainter::TransitionType::SLIDE_BOTH_LEFT;
	if (_stricmp(pszTransition, "SlideRight") == 0) return DesktopPainter::TransitionType::SLIDE_BOTH_RIGHT;
	if (_stricmp(pszTransition, "SlideUp") == 0) return DesktopPainter::TransitionType::SLIDE_BOTH_UP;
	if (_stricmp(pszTransition, "SlideDown") == 0) return DesktopPainter::TransitionType::SLIDE_BOTH_DOWN;
	if (_stricmp(pszTransition, "SlideInLeft") == 0) return DesktopPainter::TransitionType::SLIDE_IN_LEFT;
	if (_stricmp(pszTransition, "SlideInRight") == 0) return DesktopPainter::TransitionType::SLIDE_IN_RIGHT;
	if (_stricmp(pszTransition, "SlideInUp") == 0) return DesktopPainter::TransitionType::SLIDE_IN_UP;
	if (_stricmp(pszTransition, "SlideInDown") == 0) return DesktopPainter::TransitionType::SLIDE_IN_DOWN;
	if (_stricmp(pszTransition, "SlideOutLeft") == 0) return DesktopPainter::TransitionType::SLIDE_OUT_LEFT;
	if (_stricmp(pszTransition, "SlideOutRight") == 0) return DesktopPainter::TransitionType::SLIDE_OUT_RIGHT;
	if (_stricmp(pszTransition, "SlideOutUp") == 0) return DesktopPainter::TransitionType::SLIDE_OUT_UP;
	if (_stricmp(pszTransition, "SlideOutDown") == 0) return DesktopPainter::TransitionType::SLIDE_OUT_DOWN;

	if (_stricmp(pszTransition, "ScanLeft") == 0) return DesktopPainter::TransitionType::SCAN_LEFT;
	if (_stricmp(pszTransition, "ScanRight") == 0) return DesktopPainter::TransitionType::SCAN_RIGHT;
	if (_stricmp(pszTransition, "ScanUp") == 0) return DesktopPainter::TransitionType::SCAN_UP;
	if (_stricmp(pszTransition, "ScanDown") == 0) return DesktopPainter::TransitionType::SCAN_DOWN;

	if (_stricmp(pszTransition, "SquaresRandomIn") == 0) return DesktopPainter::TransitionType::SQUARES_RANDOM_IN;
	if (_stricmp(pszTransition, "SquaresRandomOut") == 0) return DesktopPainter::TransitionType::SQUARES_RANDOM_OUT;
	if (_stricmp(pszTransition, "SquaresLinearVerticalIn") == 0) return DesktopPainter::TransitionType::SQUARES_LINEAR_VERTICAL_IN;
	if (_stricmp(pszTransition, "SquaresLinearVerticalOut") == 0) return DesktopPainter::TransitionType::SQUARES_LINEAR_VERTICAL_OUT;
	if (_stricmp(pszTransition, "SquaresLinearHorizontalIn") == 0) return DesktopPainter::TransitionType::SQUARES_LINEAR_HORIZONTAL_IN;
	if (_stricmp(pszTransition, "SquaresLinearHorizontalOut") == 0) return DesktopPainter::TransitionType::SQUARES_LINEAR_HORIZONTAL_OUT;

	return DesktopPainter::TransitionType::NONE;
}
