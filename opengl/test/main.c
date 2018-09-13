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
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

const float vertices[] = {
        // positions          // colors           // texture coords
        1.0f,  1.0f, 0.0f,   1.0f, 1.0f, 1.0f,   1.0f, 1.0f, // top right
        1.0f, -1.0f, 0.0f,   1.0f, 1.0f, 1.0f,   1.0f, 0.0f, // bottom right
        -1.0f, -1.0f, 0.0f,   1.0f, 1.0f, 1.0f,   0.0f, 0.0f, // bottom left
        -1.0f,  1.0f, 0.0f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f  // top left
};

const unsigned indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
};

struct {
        struct pass_texture *pass_custom;
        struct pass *pass_main;
        struct shader *shader;
        struct shader *shader_normal;
        struct vbo *vbo;
        struct vbo *ebo;
        struct vao *vao;
        struct texture *diffuse;
        unsigned loaded : 1;
        unsigned draw : 1;

        void(*update)();
        void(*render)();

        SDL_Window* window;
        SDL_GLContext context;
} game;

static void __init()
{
        game.pass_custom = pass_texture_new(2,
                g_device.width, g_device.height,
                1, 1);
        game.pass_main = pass_main_new();

        struct string *vs = file_to_string("inner://res/test.vs");
        struct string *fs = file_to_string("inner://res/test.fs");
        game.shader = shader_new(vs->ptr, fs->ptr);
        ref_dec(&vs->base);
        ref_dec(&fs->base);

        vs = file_to_string("inner://res/texture.vs");
        fs = file_to_string("inner://res/texture.fs");
        game.shader_normal = shader_new(vs->ptr, fs->ptr);
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
        ref_dec(&game.shader_normal->base);
        ref_dec(&game.vbo->base);
        ref_dec(&game.vao->base);
        ref_dec(&game.ebo->base);
        ref_dec(&game.diffuse->base);
        ref_dec(&game.pass_main->base);
        ref_dec(&game.pass_custom->base.base);

        SDL_GL_DeleteContext(game.context);
        SDL_DestroyWindow(game.window);
        SDL_Quit();
}

static void __draw()
{
        struct pass *pass = &game.pass_custom->base;

        if(pass->begin) {
                pass->begin(pass);
        }
        glClearColor(0.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(game.shader->id);
        glBindTexture(GL_TEXTURE_2D, game.diffuse->id);
        glBindVertexArray(game.vao->id);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        if(pass->end) {
                pass->end(pass);
        }

        pass = game.pass_main;

        if(pass->begin) {
                pass->begin(pass);
        }
        glClearColor(0.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(game.shader_normal->id);
        struct texture *tex = cast(array_get(game.pass_custom->textures, 0), struct texture, base);
        glBindTexture(GL_TEXTURE_2D, tex->id);
        glBindVertexArray(game.vao->id);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        if(pass->end) {
                pass->end(pass);
        }
}

static void __update()
{

}

static void __load_local()
{
        if(game.loaded) {

                glGetIntegerv(GL_FRAMEBUFFER_BINDING, (signed *)&g_device.id_resolved);

                printf("%s\n", glGetString(GL_VERSION));
                printf("%s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

                __init();
                game.update = __update;
                game.render = __draw;
        }
}

static void main_tick()
{
        SDL_Event event;

        while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) exit(0);
                else if (event.type == SDL_KEYUP &&
                        event.key.keysym.sym == SDLK_ESCAPE) {
                                exit(0);
                }
        }

        if(game.update) {
                game.update();
        }
        if(game.render && game.draw) {
                game.render();
                game.draw = 0;
        }

        SDL_GL_SwapWindow(game.window);
}

void main_loop()
{
#if __EMSCRIPTEN__
        emscripten_set_main_loop(main_tick, -1, 1);
#else
        while (1)
        {
                main_tick();
        }
#endif
}

#if OS == WEB
EMSCRIPTEN_KEEPALIVE
void loaded() {
        game.loaded = 1;
}
#endif

int main(int args, char **argv)
{
        atexit(__exit);
        int i, j;
        SDL_Init(SDL_INIT_VIDEO);

#if OS != WEB
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 8);
#endif

        game.loaded = 1;
        game.draw = 1;
        game.update = __load_local;
        game.render = NULL;
        g_device.width = 800;
        g_device.height = 640;

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

        SDL_DisplayMode DM;
        SDL_GetCurrentDisplayMode(0, &DM);
        game.window = SDL_CreateWindow("Graphic Test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                g_device.width, g_device.height, SDL_WINDOW_OPENGL);
        game.context = SDL_GL_CreateContext(game.window);
        SDL_SetWindowSize(game.window, g_device.width, g_device.height);

        main_loop();

        return 0;
}
