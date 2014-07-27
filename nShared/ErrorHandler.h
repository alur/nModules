/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  ErrorHandler.h
 *  The nModules Project
 *
 *  Functions for dealing with errors.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "../Utilities/Common.h"

namespace ErrorHandler
{
    /// <summary>
    /// The severity of an error.
    /// </summary>
    enum class Level
    {
        Critical,
        Warning,
        Notice,
        Debug
    };

    void Error(Level level, LPCTSTR format, ...);
    void ErrorHR(Level level, HRESULT hr, LPCTSTR format = nullptr, ...);
    void SetLevel(Level level);
    void Initialize(LPCTSTR moduleName);
}
