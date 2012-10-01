/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  IIconEventHandler.hpp
 *  The nModules Project
 *
 *  Interface for a class capable of handling events from Icon.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

class IIconEventHandler {
public:
	void IconEvent_Moved(LPVOID self);
	void IconEvent_Rename(LPVOID self, LPCSTR newName);
	void IconEvent_Delete(LPVOID self);
};
