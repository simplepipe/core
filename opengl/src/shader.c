#include <stdlib.h>
#include <kernel/io.h>
#include <opengl/shader.h>

static void __shader_free(const struct ref *ref);

struct shader *shader_new(const char *vert, const char *frag)
{
        return shader_init(malloc(sizeof(struct shader)), vert, frag);
}

struct shader *shader_init(struct shader *p,
        const char *vert, const char *frag)
{
        signed r;
        char info[512];

        p->base = (struct ref){__shader_free, 1};
        p->id = 0;

        unsigned vs = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vs, 1, &vert, NULL);
        glCompileShader(vs);
        glGetShaderiv(vs, GL_COMPILE_STATUS, &r);
        if(!r) {
                glGetShaderInfoLog(vs, 512, NULL, info);
                debug("vertex shader error:\n%s\n", info);
                glDeleteShader(vs);
                goto finish;
        }

        unsigned fs = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fs, 1, &frag, NULL);
        glCompileShader(fs);
        glGetShaderiv(fs, GL_COMPILE_STATUS, &r);
        if(!r) {
                glGetShaderInfoLog(fs, 512, NULL, info);
                debug("fragment shader error:\n%s\n", info);
                glDeleteShader(vs);
                glDeleteShader(fs);
                goto finish;
        }

        unsigned vf = glCreateProgram();
        glAttachShader(vf, vs);
        glAttachShader(vf, fs);
        glLinkProgram(vf);
        glGetProgramiv(vf, GL_LINK_STATUS, &r);
        if(!r) {
                glGetProgramInfoLog(vf, 512, NULL, info);
                debug("link shader error:\n%s\n", info);
                glDeleteShader(vs);
                glDeleteShader(fs);
                goto finish;
        }

        glDeleteShader(vs);
        glDeleteShader(fs);

        p->id = vf;

finish:
        return p;
}

void shader_release(struct shader *p)
{
        if(p->id > 0) {
                glDeleteProgram(p->id);
                p->id = 0;
        }
}

static void __shader_free(const struct ref *ref)
{
        struct shader *p = cast(ref, struct shader, base);
        shader_release(p);
        free(p);
}
