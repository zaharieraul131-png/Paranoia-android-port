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

typedef struct tagPOINT { LONG x; LONG y; } POINT, *LPPOINT;
typedef struct tagRECT { LONG left; LONG top; LONG right; LONG bottom; } RECT, *LPRECT;

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

static inline DWORD GetTickCount(void) { return 0; }
static inline void Sleep(DWORD ms) {}
static inline BOOL GetCursorPos(LPPOINT lpPoint) { if (lpPoint) { lpPoint->x = 0; lpPoint->y = 0; } return TRUE; }
static inline BOOL SetCursorPos(int X, int Y) { return TRUE; }
static inline int ShowCursor(BOOL bShow) { return 0; }
static inline BOOL GetWindowRect(HWND hWnd, LPRECT lpRect) { return FALSE; }
static inline HWND GetFocus(void) { return NULL; }
static inline short GetAsyncKeyState(int vKey) { return 0; }

#endif // ANDROID_WINDOWS_WRAPPER_H
