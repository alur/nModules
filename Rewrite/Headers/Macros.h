#pragma once

// Easy DLL import/export
#define EXPORT_CDECL(type) EXTERN_C __declspec(dllexport) type __cdecl
#define EXPORT_STDCALL(type) EXTERN_C __declspec(dllexport) type __stdcall

// Deletes the given object if it's not a nullptr, and then sets the pointer to nullptr.
#define SAFEDELETE(obj) if (obj != nullptr) { delete obj; obj = nullptr; }
#define SAFERELEASE(x) if (x != nullptr) { (x)->Release(); x = nullptr; }
#define SAFEFREE(x) if (x != nullptr) { free(x); x = nullptr; }

#define CHECKFLAG(flags, flag) (((flags) & (flag)) == flag)
