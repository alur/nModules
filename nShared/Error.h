/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Error.h
 *  The nModules Project
 *
 *  Functions for dealing with errors.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

enum {
    E_LVL_ERROR = 1,
    E_LVL_WARNING,
    E_LVL_NOTICE,
    E_LVL_DEBUG
};

typedef unsigned __int8 ERRORLVL;

void ErrorMessage(ERRORLVL nLevel, LPCSTR pszFormat, ...);
void SetErrorLevel(ERRORLVL nLevel);
