/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Settings.cpp
 *  The nModules Project
 *
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../nShared/LiteStep.h"
#include "../nCoreCom/Core.h"
#include "Settings.h"

extern DesktopPainter* g_pDesktopPainter;

//
TCHAR onResolutionChange[MAX_LINE_LENGTH];


/// <summary>
/// Loads all settings from .RC files and applies them.
/// </summary>
void nDesk::Settings::Load() {
    TCHAR buf[MAX_LINE_LENGTH];

    //
    LiteStep::GetRCString(_T("nDeskOnResolutionChange"),  onResolutionChange, _T(""), _countof(onResolutionChange));

    // Defaults to 625ms
    g_pDesktopPainter->SetTransitionTime(LiteStep::GetRCInt(_T("nDeskTransitionDuration"), 625));

    // 
    g_pDesktopPainter->SetSquareSize(LiteStep::GetRCInt(_T("nDeskTransitionSquareSize"), 150));

    // 
    LiteStep::GetRCString(_T("nDeskTransitionEffect"),  buf, _T("FadeOut"), _countof(buf));
    g_pDesktopPainter->SetTransitionType(TransitionTypeFromString(buf));

    // 
    g_pDesktopPainter->SetInvalidateAllOnUpdate(LiteStep::GetRCBoolDef(_T("nDeskInvalidateAllOnUpdate"), FALSE) != FALSE);
}


/// <summary>
/// String -> TransitionType
/// </summary>
DesktopPainter::TransitionType nDesk::Settings::TransitionTypeFromString(LPCTSTR pszTransition)
{
    if (_tcsicmp(pszTransition, _T("FadeIn")) == 0) return DesktopPainter::TransitionType::FADE_IN;
    if (_tcsicmp(pszTransition, _T("FadeOut")) == 0) return DesktopPainter::TransitionType::FADE_OUT;

    if (_tcsicmp(pszTransition, _T("SlideLeft")) == 0) return DesktopPainter::TransitionType::SLIDE_BOTH_LEFT;
    if (_tcsicmp(pszTransition, _T("SlideRight")) == 0) return DesktopPainter::TransitionType::SLIDE_BOTH_RIGHT;
    if (_tcsicmp(pszTransition, _T("SlideUp")) == 0) return DesktopPainter::TransitionType::SLIDE_BOTH_UP;
    if (_tcsicmp(pszTransition, _T("SlideDown")) == 0) return DesktopPainter::TransitionType::SLIDE_BOTH_DOWN;
    if (_tcsicmp(pszTransition, _T("SlideInLeft")) == 0) return DesktopPainter::TransitionType::SLIDE_IN_LEFT;
    if (_tcsicmp(pszTransition, _T("SlideInRight")) == 0) return DesktopPainter::TransitionType::SLIDE_IN_RIGHT;
    if (_tcsicmp(pszTransition, _T("SlideInUp")) == 0) return DesktopPainter::TransitionType::SLIDE_IN_UP;
    if (_tcsicmp(pszTransition, _T("SlideInDown")) == 0) return DesktopPainter::TransitionType::SLIDE_IN_DOWN;
    if (_tcsicmp(pszTransition, _T("SlideOutLeft")) == 0) return DesktopPainter::TransitionType::SLIDE_OUT_LEFT;
    if (_tcsicmp(pszTransition, _T("SlideOutRight")) == 0) return DesktopPainter::TransitionType::SLIDE_OUT_RIGHT;
    if (_tcsicmp(pszTransition, _T("SlideOutUp")) == 0) return DesktopPainter::TransitionType::SLIDE_OUT_UP;
    if (_tcsicmp(pszTransition, _T("SlideOutDown")) == 0) return DesktopPainter::TransitionType::SLIDE_OUT_DOWN;

    if (_tcsicmp(pszTransition, _T("ScanLeft")) == 0) return DesktopPainter::TransitionType::SCAN_LEFT;
    if (_tcsicmp(pszTransition, _T("ScanRight")) == 0) return DesktopPainter::TransitionType::SCAN_RIGHT;
    if (_tcsicmp(pszTransition, _T("ScanUp")) == 0) return DesktopPainter::TransitionType::SCAN_UP;
    if (_tcsicmp(pszTransition, _T("ScanDown")) == 0) return DesktopPainter::TransitionType::SCAN_DOWN;

    if (_tcsicmp(pszTransition, _T("SquaresRandomIn")) == 0) return DesktopPainter::TransitionType::SQUARES_RANDOM_IN;
    if (_tcsicmp(pszTransition, _T("SquaresRandomOut")) == 0) return DesktopPainter::TransitionType::SQUARES_RANDOM_OUT;
    if (_tcsicmp(pszTransition, _T("SquaresLinearVerticalIn")) == 0) return DesktopPainter::TransitionType::SQUARES_LINEAR_VERTICAL_IN;
    if (_tcsicmp(pszTransition, _T("SquaresLinearVerticalOut")) == 0) return DesktopPainter::TransitionType::SQUARES_LINEAR_VERTICAL_OUT;
    if (_tcsicmp(pszTransition, _T("SquaresLinearHorizontalIn")) == 0) return DesktopPainter::TransitionType::SQUARES_LINEAR_HORIZONTAL_IN;
    if (_tcsicmp(pszTransition, _T("SquaresLinearHorizontalOut")) == 0) return DesktopPainter::TransitionType::SQUARES_LINEAR_HORIZONTAL_OUT;
    if (_tcsicmp(pszTransition, _T("SquaresTriangularBottomRightIn")) == 0) return DesktopPainter::TransitionType::SQUARES_TRIANGULAR_BOTTOM_RIGHT_IN;
    if (_tcsicmp(pszTransition, _T("SquaresTriangularBottomRightOut")) == 0) return DesktopPainter::TransitionType::SQUARES_TRIANGULAR_BOTTOM_RIGHT_OUT;
    if (_tcsicmp(pszTransition, _T("SquaresClockwiseIn")) == 0) return DesktopPainter::TransitionType::SQUARES_CLOCKWISE_IN;
    if (_tcsicmp(pszTransition, _T("SquaresClockwiseOut")) == 0) return DesktopPainter::TransitionType::SQUARES_CLOCKWISE_OUT;
    if (_tcsicmp(pszTransition, _T("SquaresCounterClockwiseIn")) == 0) return DesktopPainter::TransitionType::SQUARES_COUNTERCLOCKWISE_IN;
    if (_tcsicmp(pszTransition, _T("SquaresCounterClockwiseOut")) == 0) return DesktopPainter::TransitionType::SQUARES_COUNTERCLOCKWISE_OUT;
    
    return DesktopPainter::TransitionType::NONE;
}


/// <summary>
/// 
/// </summary>
void nDesk::Settings::OnResolutionChange()
{
    if (onResolutionChange[0] != _T('\0'))
    {
        LiteStep::LSExecute(nullptr, onResolutionChange, SW_SHOW);
    }
}
