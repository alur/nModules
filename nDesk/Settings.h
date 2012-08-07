/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *	Settings.h													  August, 2012
 *	The nModules Project
 *
 *	
 *      
 *													             Erik Welander
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "DesktopPainter.hpp"

namespace Settings {
	void Load();
	DesktopPainter::EasingType EasingFromString(LPCSTR pszTransition);
	DesktopPainter::TransitionType TransitionTypeFromString(LPCSTR pszTransition);
}