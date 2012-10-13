/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  TextFormatting.h
 *  The nModules Project
 *
 *  Formats text strings.
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once
#include "../nShared/Macros.h"

typedef BOOL (__cdecl * FORMATTINGPROC)(LPCWSTR name, UCHAR numArgs, LPCWSTR arguments[], bool* isDynamic);

EXPORT_CDECL(BOOL) RegisterDynamicTextFunction(LPCWSTR name, UCHAR numArgs, FORMATTINGPROC formatter);
EXPORT_CDECL(BOOL) UnRegisterDynamicTextFunction(LPCWSTR name, UCHAR numArgs);

//----------------------------------------------------------------------------
// Dynamic text -- Syntax
//----------------------------------------------------------------------------
// Sample: "Text and stuff [function2(12) add function3('4', true)] some more text ..."
// 
// Text:
//  .*
//  Text[Expression]Text
//
// Expression:
//  Constant
//  Function
//  Function(List)
//  Expression Function Expression
//
// List:
//  Expression, List
//  Expression
//     
// Function:
//  ['A'-'Z' 'a'-'z']+['0'-'9' 'A'-'Z' 'a'-'z']*
// 
// Constant:
//  String
//  Floating
//  Integer
//  Boolean
//
// String:
//  '.*'
//
// Floating:
//  [+-]?[0-9]+\.[0-9]?
//
// Integer:
//  [+-]?[0-9]+
//
// Boolean:
//  true
//  false
//


//----------------------------------------------------------------------------
// Dynamic text -- Usage
//----------------------------------------------------------------------------
// User calls ParseText --> Returns a ParsedText Object
// User calls ParsedText->Evaluate() to get the "current" text
// ParsedText->Evaluate() also returns an "isDynamic" value which determines
// if the text might change on subsequent calls to Evaluate().
// User eventually calls ParsedText->Release().
//
// Function providers call RegisterDynamicTextFunction to add functions
// 
// Function providers call UnRegisterDynamicTextFunction to remove functions
//


//----------------------------------------------------------------------------
// Dynamic text -- Functions map
//----------------------------------------------------------------------------
// Maps Function Identifier -> FunctionProcedure. Contains all functions which
// have been used in an expression or were registered at some point.
// Functions which have not been registered yet, or have been unregistered
// have a FunctionProcedure of NULL.
//


//----------------------------------------------------------------------------
// Dynamic text -- Functions identifier
//----------------------------------------------------------------------------
// Wide string. First character = # of arguments
// 2nd character = return type
// Next # of args characters -> argument types, in order.
// Remainder, function name followed by terminating null.
// e.g. Boolean IAmAFunction(Integer, String) -> 2123IAmAFunction.
//
// 


//----------------------------------------------------------------------------
// Dynamic text -- Functions procedure
//----------------------------------------------------------------------------
// Takes the arguments of a function and returns a result as well as a boolean
// which indicates if this is a "dynamic" function -- indicating that its value
// may change if this function is called again.
// 
// Value ( * FunctionProc )( FunctionName, NumberOfArgs, ReturnType,
//                            ArgumentType[], Arguments[], *isDynamic )
//


//----------------------------------------------------------------------------
// Dynamic text -- ParseText
//----------------------------------------------------------------------------
// Takes a single LPCWSTR. Splits it into "Constant" text and "Function" parts,
// collecting the parts in a ParsedText object -- optimized for fast evaluation.
// Adds any non-registered functions to the Functions map.
//
