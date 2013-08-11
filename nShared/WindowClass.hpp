/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  WindowClass.hpp
 *  The nModules Project
 *
 *  
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

//#include "../Utilities/PointerIterator.hpp"
//#include "State.hpp"
//#include <list>
//
//class WindowClass
//{
//    // typedefs
//public:
//    typedef PointerIterator<std::list<State*>::iterator, State> STATE;
//
//    // Constructors & Destructors
//public:
//    explicit WindowClass();
//    virtual ~WindowClass();
//    
//public:
//
//    // Discards device-dependent stuff.
//    void DiscardDeviceResources();
//
//    // (Re)Creates D2D device-dependent stuff.
//    HRESULT ReCreateDeviceResources(ID2D1HwndRenderTarget*);
//    
//    // The render target to draw to.
//    ID2D1HwndRenderTarget* renderTarget;
//
//    //
//    State *GetState(LPCTSTR stateName);
//
//private:
//    // The base state -- the one to use when no others are active.
//    STATE mBaseState;
//
//    // Settings.
//    Settings* mSettings;
//
//    // All current states.
//    std::list<State*> mStates;
//};
