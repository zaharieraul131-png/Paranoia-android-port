#ifndef ANDROID_WINDOWS_WRAPPER_H
#define ANDROID_WINDOWS_WRAPPER_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef int BOOL;
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned int DWORD;
typedef unsigned int UINT;
typedef int LONG;
typedef void* HANDLE;
typedef void* HINSTANCE;
typedef void* HWND;
typedef void* HDC;
typedef void* HGLRC;
typedef void* HMODULE;
typedef const char* LPCSTR;
typedef char* LPSTR;

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

static inline DWORD GetTickCount(void) { return 0; }
static inline void Sleep(DWORD ms) {}

#endif // ANDROID_WINDOWS_WRAPPER_H
