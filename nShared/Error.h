/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *	Error.h															July, 2012
 *	The nModules Project
 *
 *	Functions for dealing with errors.
 *      
 *													             Erik Welander
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#ifndef ERROR_H
#define ERROR_H

#define E_LVL_ERROR 1
#define E_LVL_WARNING 2
#define E_LVL_NOTICE 3
#define E_LVL_DEBUG 4

typedef unsigned __int8 ERRORLVL;

void ErrorMessage(ERRORLVL nLevel, LPCSTR pszFormat, ...);
void SetErrorLevel(ERRORLVL nLevel);

#endif /* ERROR_H */
