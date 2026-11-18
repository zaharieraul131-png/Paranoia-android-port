#ifndef ANDROID_PORT_FIXES_H
#define ANDROID_PORT_FIXES_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _WIN32
#define __declspec(x) __attribute__((visibility("default")))
#define __stdcall
#define __cdecl
#define __fastcall
#define WINAPI
#define CALLBACK
#endif

#ifdef __cplusplus
}
#endif

#endif // ANDROID_PORT_FIXES_H
