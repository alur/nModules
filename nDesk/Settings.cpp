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
    LiteStep::GetRCString(L"nDeskOnResolutionChange",  onResolutionChange, L"", _countof(onResolutionChange));

    // Defaults to 625ms
    g_pDesktopPainter->SetTransitionTime(LiteStep::GetRCInt(L"nDeskTransitionDuration", 625));

    // 
    g_pDesktopPainter->SetSquareSize(LiteStep::GetRCInt(L"nDeskTransitionSquareSize", 150));

    // 
    LiteStep::GetRCString(L"nDeskTransitionEffect",  buf, L"FadeOut", _countof(buf));
    g_pDesktopPainter->SetTransitionType(TransitionTypeFromString(buf));

    // 
    g_pDesktopPainter->SetInvalidateAllOnUpdate(LiteStep::GetRCBoolDef(L"nDeskInvalidateAllOnUpdate", FALSE) != FALSE);
}


/// <summary>
/// String -> TransitionType
/// </summary>
DesktopPainter::TransitionType nDesk::Settings::TransitionTypeFromString(LPCTSTR pszTransition)
{
    if (_wcsicmp(pszTransition, L"FadeIn") == 0) return DesktopPainter::TransitionType::FADE_IN;
    if (_wcsicmp(pszTransition, L"FadeOut") == 0) return DesktopPainter::TransitionType::FADE_OUT;

    if (_wcsicmp(pszTransition, L"SlideLeft") == 0) return DesktopPainter::TransitionType::SLIDE_BOTH_LEFT;
    if (_wcsicmp(pszTransition, L"SlideRight") == 0) return DesktopPainter::TransitionType::SLIDE_BOTH_RIGHT;
    if (_wcsicmp(pszTransition, L"SlideUp") == 0) return DesktopPainter::TransitionType::SLIDE_BOTH_UP;
    if (_wcsicmp(pszTransition, L"SlideDown") == 0) return DesktopPainter::TransitionType::SLIDE_BOTH_DOWN;
    if (_wcsicmp(pszTransition, L"SlideInLeft") == 0) return DesktopPainter::TransitionType::SLIDE_IN_LEFT;
    if (_wcsicmp(pszTransition, L"SlideInRight") == 0) return DesktopPainter::TransitionType::SLIDE_IN_RIGHT;
    if (_wcsicmp(pszTransition, L"SlideInUp") == 0) return DesktopPainter::TransitionType::SLIDE_IN_UP;
    if (_wcsicmp(pszTransition, L"SlideInDown") == 0) return DesktopPainter::TransitionType::SLIDE_IN_DOWN;
    if (_wcsicmp(pszTransition, L"SlideOutLeft") == 0) return DesktopPainter::TransitionType::SLIDE_OUT_LEFT;
    if (_wcsicmp(pszTransition, L"SlideOutRight") == 0) return DesktopPainter::TransitionType::SLIDE_OUT_RIGHT;
    if (_wcsicmp(pszTransition, L"SlideOutUp") == 0) return DesktopPainter::TransitionType::SLIDE_OUT_UP;
    if (_wcsicmp(pszTransition, L"SlideOutDown") == 0) return DesktopPainter::TransitionType::SLIDE_OUT_DOWN;

    if (_wcsicmp(pszTransition, L"ScanLeft") == 0) return DesktopPainter::TransitionType::SCAN_LEFT;
    if (_wcsicmp(pszTransition, L"ScanRight") == 0) return DesktopPainter::TransitionType::SCAN_RIGHT;
    if (_wcsicmp(pszTransition, L"ScanUp") == 0) return DesktopPainter::TransitionType::SCAN_UP;
    if (_wcsicmp(pszTransition, L"ScanDown") == 0) return DesktopPainter::TransitionType::SCAN_DOWN;

    if (_wcsicmp(pszTransition, L"SquaresRandomIn") == 0) return DesktopPainter::TransitionType::SQUARES_RANDOM_IN;
    if (_wcsicmp(pszTransition, L"SquaresRandomOut") == 0) return DesktopPainter::TransitionType::SQUARES_RANDOM_OUT;
    if (_wcsicmp(pszTransition, L"SquaresLinearVerticalIn") == 0) return DesktopPainter::TransitionType::SQUARES_LINEAR_VERTICAL_IN;
    if (_wcsicmp(pszTransition, L"SquaresLinearVerticalOut") == 0) return DesktopPainter::TransitionType::SQUARES_LINEAR_VERTICAL_OUT;
    if (_wcsicmp(pszTransition, L"SquaresLinearHorizontalIn") == 0) return DesktopPainter::TransitionType::SQUARES_LINEAR_HORIZONTAL_IN;
    if (_wcsicmp(pszTransition, L"SquaresLinearHorizontalOut") == 0) return DesktopPainter::TransitionType::SQUARES_LINEAR_HORIZONTAL_OUT;
    if (_wcsicmp(pszTransition, L"SquaresTriangularBottomRightIn") == 0) return DesktopPainter::TransitionType::SQUARES_TRIANGULAR_BOTTOM_RIGHT_IN;
    if (_wcsicmp(pszTransition, L"SquaresTriangularBottomRightOut") == 0) return DesktopPainter::TransitionType::SQUARES_TRIANGULAR_BOTTOM_RIGHT_OUT;
    if (_wcsicmp(pszTransition, L"SquaresClockwiseIn") == 0) return DesktopPainter::TransitionType::SQUARES_CLOCKWISE_IN;
    if (_wcsicmp(pszTransition, L"SquaresClockwiseOut") == 0) return DesktopPainter::TransitionType::SQUARES_CLOCKWISE_OUT;
    if (_wcsicmp(pszTransition, L"SquaresCounterClockwiseIn") == 0) return DesktopPainter::TransitionType::SQUARES_COUNTERCLOCKWISE_IN;
    if (_wcsicmp(pszTransition, L"SquaresCounterClockwiseOut") == 0) return DesktopPainter::TransitionType::SQUARES_COUNTERCLOCKWISE_OUT;
    
    return DesktopPainter::TransitionType::NONE;
}


/// <summary>
/// 
/// </summary>
void nDesk::Settings::OnResolutionChange()
{
    if (onResolutionChange[0] != L'\0')
    {
        LiteStep::LSExecute(nullptr, onResolutionChange, SW_SHOW);
    }
}
