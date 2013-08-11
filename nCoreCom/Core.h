/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Core.h
 *  The nModules Project
 *
 *  Functions declarations for the CoreCom library. All functions exported by
 *  nCore and some Init functions.
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "../nShared/MonitorInfo.hpp"
#include "../Utilities/Versioning.h"
#include "../nCore/IParsedText.hpp"

class Window;

namespace nCore
{
    HRESULT Connect(VERSION minVersion);
    void Disconnect();
    bool Initialized();

    VERSION GetCoreVersion();

    namespace System
    {
        //
        MonitorInfo *FetchMonitorInfo();

        // Dynamic Text Service
        IParsedText *ParseText(LPCWSTR text);
        BOOL RegisterDynamicTextFunction(LPCWSTR name, UCHAR numArgs, FORMATTINGPROC formatter, bool dynamic);
        BOOL UnRegisterDynamicTextFunction(LPCWSTR name, UCHAR numArgs);
        BOOL DynamicTextChangeNotification(LPCWSTR name, UCHAR numArgs);

        // Window Registrar
        void RegisterWindow(LPCTSTR prefix, LPVOID window);
        void UnRegisterWindow(LPCTSTR prefix);
        Window *FindRegisteredWindow(LPCTSTR prefix);
        void AddWindowRegistrationListener(LPCTSTR prefix, Window *listener);
        void RemoveWindowRegistrationListener(LPCTSTR prefix, Window *listener);
    }
}
