#ifndef ANDROID_GL_WRAPPER_H
#define ANDROID_GL_WRAPPER_H

typedef unsigned int GLenum;
typedef unsigned char GLboolean;
typedef unsigned int GLbitfield;
typedef void GLvoid;
typedef signed char GLbyte;
typedef short GLshort;
typedef int GLint;
typedef unsigned char GLubyte;
typedef unsigned short GLushort;
typedef unsigned int GLuint;
typedef int GLsizei;
typedef float GLfloat;
typedef float GLclampf;
typedef double GLdouble;
typedef double GLclampd;

#ifndef GL_TEXTURE_WIDTH
#define GL_TEXTURE_WIDTH 0x1000
#endif
#ifndef GL_TEXTURE_HEIGHT
#define GL_TEXTURE_HEIGHT 0x1001
#endif
#ifndef GL_ALPHA_TEST_FUNC
#define GL_ALPHA_TEST_FUNC 0x0BC1
#endif
#ifndef GL_ALPHA_TEST_REF
#define GL_ALPHA_TEST_REF 0x0BC2
#endif
#ifndef GL_TEXTURE0_ARB
#define GL_TEXTURE0_ARB 0x84C0
#define GL_TEXTURE1_ARB 0x84C1
#define GL_TEXTURE2_ARB 0x84C2
#define GL_TEXTURE3_ARB 0x84C3
#define GL_TEXTURE4_ARB 0x84C4
#define GL_TEXTURE5_ARB 0x84C5
#define GL_TEXTURE6_ARB 0x84C6
#define GL_TEXTURE7_ARB 0x84C7
#endif

#endif // ANDROID_GL_WRAPPER_H
