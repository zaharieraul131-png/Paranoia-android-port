#ifndef ANDROID_GL_WRAPPER_H
#define ANDROID_GL_WRAPPER_H

#include <GLES/gl.h>
#include <GLES/glext.h>

typedef double GLdouble;
typedef double GLclampd;

#ifndef GL_QUADS
#define GL_QUADS 0x0007
#endif

#ifndef GL_POLYGON
#define GL_POLYGON 0x0009
#endif

#ifndef GL_CLAMP
#define GL_CLAMP 0x2900
#endif

#ifndef GL_TEXTURE_3D
#define GL_TEXTURE_3D 0x806F
#endif

#ifndef GL_TEXTURE_CUBE_MAP_ARB
#define GL_TEXTURE_CUBE_MAP_ARB 0x8513
#endif

#ifndef GL_TEXTURE_MAX_ANISOTROPY_EXT
#define GL_TEXTURE_MAX_ANISOTROPY_EXT 0x84FE
#endif

#ifndef GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT
#define GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT 0x84FF
#endif

#endif // ANDROID_GL_WRAPPER_H
