/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *	Bangs.h															July, 2012
 *	The nModules Project
 *
 *	Function declarations for Bangs.cpp
 *      
 *													             Erik Welander
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#ifndef BANGS_H
#define BANGS_H

namespace Bangs {
	void _Register();
	void _Unregister();

	void SetWorkArea(HWND, LPCSTR);
	void On(HWND, LPCSTR);
	void Off(HWND, LPCSTR);
	void Test(HWND, LPCSTR);

	typedef struct BangItem {
		char szName[54];
		BANGCOMMANDPROC pCommand;
	} BangItem;
}

#endif /* BANGS_H */