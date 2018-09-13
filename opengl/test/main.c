#include <kernel/io.h>
#include <kernel/string.h>
#include <opengl/device.h>
#include <opengl/texture.h>
#include <opengl/pass.h>
#include <opengl/buffer.h>
#include <opengl/image.h>
#include <opengl/shader.h>

#if OS == WEB
#include <emscripten/emscripten.h>
#endif

const float vertices[] = {
        // positions          // colors           // texture coords
        0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
        0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left
};

const unsigned indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
};

struct {
        struct shader *shader;
        struct vbo *vbo;
        struct vbo *ebo;
        struct vao *vao;
        struct texture *diffuse;
        unsigned loaded : 1;
        unsigned draw : 1;
} game;

static void __init()
{
        struct string *vs = file_to_string("inner://res/texture.vs");
        struct string *fs = file_to_string("inner://res/texture.fs");
        game.shader = shader_new(vs->ptr, fs->ptr);
        ref_dec(&vs->base);
        ref_dec(&fs->base);

        game.vao = vao_new();
        game.vbo = vbo_new();
        game.ebo = vbo_new();

        glBindVertexArray(game.vao->id);

        glBindBuffer(GL_ARRAY_BUFFER, game.vbo->id);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, game.ebo->id);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // color attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        // texture coord attribute
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        vao_add_vbo(game.vao, game.vbo);
        vao_add_vbo(game.vao, game.ebo);

        struct image *img = image_new();
        image_load(img, "inner://res/cat.jpg");

        game.diffuse = texture_new();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, game.diffuse->id);
        switch (img->type) {
                case IMAGE_RGBA:
                        glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
                        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img->width,
                                img->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img->ptr);
                        break;
                case IMAGE_RGB:
                        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
                        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img->width,
                                img->height, 0, GL_RGB, GL_UNSIGNED_BYTE, img->ptr);
                        break;
        }
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glBindTexture(GL_TEXTURE_2D, 0);
        ref_dec(&img->base);
}

static void __exit()
{
        ref_dec(&game.shader->base);
        ref_dec(&game.vbo->base);
        ref_dec(&game.vao->base);
        ref_dec(&game.ebo->base);
        ref_dec(&game.diffuse->base);
}

static void __key(unsigned char key, int x, int y)
{
        switch (key) {
        case 27:
                exit(0);
        }
}

static void __draw()
{
        glClearColor(0.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(game.shader->id);
        glBindTexture(GL_TEXTURE_2D, game.diffuse->id);
        glBindVertexArray(game.vao->id);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glutSwapBuffers();
}

static void __update()
{
        if(game.draw) {
                glutPostRedisplay();
                game.draw = 0;
        }
}


static void __load_local()
{
        if(game.loaded) {
                __init();
                glutDisplayFunc(__draw);
                glutIdleFunc(__update);
        }
}

#if OS == WEB
EMSCRIPTEN_KEEPALIVE
void loaded() {
        game.loaded = 1;
}
#endif

int main(int argc, char **argv)
{
        atexit(__exit);

        game.loaded = 1;
        game.draw = 1;
#if OS == WEB
        game.loaded = 0;
        file_web_set_local_path("/test_local");
        EM_ASM(
                FS.mkdir('/test_local');
                FS.mount(IDBFS,{},'/test_local');
                FS.syncfs(true, function(err) {
                        assert(!err);
                        _loaded();
                });
        );
#endif

        unsigned type;
        glutInit(&argc, argv);
        type = GLUT_RGB;
        type |= GLUT_DOUBLE;

        glutInitDisplayMode(type);
        glutInitWindowSize(800, 640);
        glutCreateWindow("Game");
        glutKeyboardFunc(__key);
        glutIdleFunc(__load_local);
        glutMainLoop();

        return 0;
}
