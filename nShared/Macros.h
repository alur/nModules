/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Macros.h	                                                  August, 2012
 *  The nModules Project
 *
 *  Utility macros
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#ifndef MACROS_H
#define MACROS_H

#define CHECKHR(hr,exp) if (FAILED(hr = exp)) { goto chkeckhr_end; }
#define CHECKHR_END() chkeckhr_end:

#endif /* MACROS_H */
