/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  IIconEventHandler.hpp                                      September, 2012
 *  The nModules Project
 *
 *  Interface for a class capable of handling events from Icon.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#ifndef IICONEVENTHANDLER_HPP
#define IICONEVENTHANDLER_HPP

class IIconEventHandler {
public:
	void IconEvent_Moved(LPVOID self);
	void IconEvent_Rename(LPVOID self, LPCSTR newName);
	void IconEvent_Delete(LPVOID self);
};

#endif /* IICONEVENTHANDLER_HPP */
