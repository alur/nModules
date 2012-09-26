/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Macros.h	                                                  August, 2012
 *  The nModules Project
 *
 *  Utility macros
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#ifndef MACROS_H
#define MACROS_H

// Lets you skip to a certain part in the function if a CHECKHR fails
#define CHECKHR(var,exp) if (FAILED(var = (exp))) { goto chkeckhr_end; }
#define CHECKHR_END() chkeckhr_end:

// Safe way to delete objects
#define SAFEDELETE(obj) if (obj != NULL) { delete obj; obj = NULL; }

// Safe way to release objects which require ->Release()
#define SAFERELEASE(x) if (x != NULL) { (x)->Release(); x = NULL; }

#endif /* MACROS_H */
