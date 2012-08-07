/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *	WorkArea.h														July, 2012
 *	The nModules Project
 *
 *	Function declarations for WorkArea.cpp
 *      
 *													             Erik Welander
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#ifndef WORKAREA_H
#define WORKAREA_H

#include "../nShared/MonitorInfo.hpp"

namespace WorkArea {
	void LoadSettings(MonitorInfo *, bool = false);
	void ResetWorkAreas(MonitorInfo *);
	void ParseLine(MonitorInfo *, LPCSTR);
}

#endif /* WORKAREA_H */
