/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  TextFunctions.h
 *  The nModules Project
 *
 *  Provides Dynamic Text Functions.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

namespace TextFunctions
{
    void _Register();
    void _UnRegister();

    // Updates the text function data
    void _Update();

    // Notifies the core about the updates.
    void _UpdateNotify();
}
