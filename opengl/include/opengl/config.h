#ifndef __opengl_config_h
#define __opengl_config_h

#include <kernel/platform.h>

#if OS == IOS
        #define GL_GLEXT_PROTOTYPES
        #include <OpenGLES/ES3/gl.h>
        #include <OpenGLES/ES3/glext.h>

        // #define glGenVertexArrays               glGenVertexArraysOES
        // #define glBindVertexArray               glBindVertexArrayOES
        // #define glDeleteVertexArrays            glDeleteVertexArraysOES
        //
        // #define glMapBuffer                     glMapBufferOES
        // #define glUnmapBuffer                   glUnmapBufferOES
        // #define glMapBufferRange                glMapBufferRangeEXT
        //
        // #define GL_MAP_WRITE_BIT                GL_MAP_WRITE_BIT_EXT
        // #define GL_MAP_READ_BIT                 GL_MAP_READ_BIT_EXT
        // #define GL_MAP_INVALIDATE_RANGE_BIT     GL_MAP_INVALIDATE_RANGE_BIT_EXT
        // #define GL_MAP_INVALIDATE_BUFFER_BIT    GL_MAP_INVALIDATE_BUFFER_BIT_EXT
        // #define GL_MAP_FLUSH_EXPLICIT_BIT       GL_MAP_FLUSH_EXPLICIT_BIT_EXT
        // #define GL_MAP_UNSYNCHRONIZED_BIT       GL_MAP_UNSYNCHRONIZED_BIT_EXT
        //
        // #define glDrawArraysInstanced           glDrawArraysInstancedEXT
        // #define glVertexAttribDivisor           glVertexAttribDivisorEXT
        #define GL_MAX_SAMPLES GL_MAX_SAMPLES_APPLE
#elif OS == OSX
        #define __gl_h_
        #define GL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED

        #include <OpenGL/gl3.h>
        #include <OpenGL/glu.h>
        #include <OpenGL/glext.h>
        #include <GLUT/glut.h>
#elif OS == DROID
        #define GL_GLEXT_PROTOTYPES
        #include <EGL/egl.h>
        #include <GLES3/gl3.h>
        #include <GLES3/gl3ext.h>

        // #define glGenVertexArrays glGenVertexArraysOES
        // #define glBindVertexArray glBindVertexArrayOES
        // #define glDeleteVertexArrays glDeleteVertexArraysOES
        //
        // #define glMapBuffer glMapBufferOES
        // #define glUnmapBuffer glUnmapBufferOES
#elif OS == WEB
        #define GL_GLEXT_PROTOTYPES
        #include <GL/glut.h>
        #include <GL/gl.h>
        #include <GL/glext.h>
#elif OS == WINDOWS
        #include <GL/glew.h>
        #include <GL/glut.h>
#else
        #define GL_GLEXT_PROTOTYPES
        #include <GL/glut.h>
        #include <GL/gl.h>
        #include <GL/glext.h>
#endif

#endif
