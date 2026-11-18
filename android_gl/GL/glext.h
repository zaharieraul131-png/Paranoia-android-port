#ifndef ANDROID_GLEXT_WRAPPER_H
#define ANDROID_GLEXT_WRAPPER_H

#include <stddef.h>

typedef char GLchar;
typedef char GLcharARB;
typedef unsigned int GLhandleARB;
typedef ptrdiff_t GLintptr;
typedef ptrdiff_t GLsizeiptr;
typedef ptrdiff_t GLintptrARB;
typedef ptrdiff_t GLsizeiptrARB;

#ifndef APIENTRY
#define APIENTRY
#endif

#ifndef APIENTRYP
#define APIENTRYP APIENTRY *
#endif

#ifndef GLAPI
#define GLAPI extern
#endif

#endif // ANDROID_GLEXT_WRAPPER_H

