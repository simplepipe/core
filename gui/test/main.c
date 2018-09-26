#include <ctype.h>
#include <kernel/io.h>
#include <kernel/string.h>
#include <kernel/hash_table.h>
#include <kernel/utf8.h>
#include <math/math.h>
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
// FreeType
#include <ft2build.h>
#include FT_FREETYPE_H

struct character{
	struct ref base;
	
	struct texture *texture;
	signed width, height;
	signed bearing_width, bearing_height;
	signed advance;
};

static void character_free(const struct ref *ref)
{
	struct character *p = cast(ref, struct character, base);
	if(p->texture) {
		ref_dec(&p->texture->base);
	}
	free(p);
}

struct character *character_new(FT_Face face, const char *c)
{
	if(FT_Load_Char(face, utf8_code(c, utf8_width(c)), FT_LOAD_RENDER)) {
		fprintf(stderr, "%s : not found!", c);
		return NULL;
	}
        
	struct character *p = malloc(sizeof(struct character));
	p->base = (struct ref){character_free, 1};
	p->texture = texture_new();
	
	glBindTexture(GL_TEXTURE_2D, p->texture->id);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); 
	glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );     
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	p->texture->width = face->glyph->bitmap.width;
	p->texture->height = face->glyph->bitmap.rows;
	p->width = face->glyph->bitmap.width;
	p->height = face->glyph->bitmap.rows;
	p->bearing_width = face->glyph->bitmap_left;
	p->bearing_height = face->glyph->bitmap_top;
	p->advance = face->glyph->advance.x;
	glBindTexture(GL_TEXTURE_2D, 0);
	return p;
}

struct {
        struct pass *pass_main;
        struct shader *shader;
        struct vbo *vbo;
        struct vao *vao;
        struct hash_table *characters;
        struct string *text;
        char *test_ptr;
        FT_Library ft;
        FT_Face face;
        unsigned loaded : 1;
        unsigned draw : 1;

        void(*update)();
        void(*render)();

        SDL_Window* window;
        SDL_GLContext context;
} game;

static void __init()
{
#define BUFF_SIZE 1024
        char vs[BUFF_SIZE];
        char fs[BUFF_SIZE];
        char version[20];
        unsigned count, version_len;
        const char *ptr;
        char *str;

        game.pass_main = pass_main_new();

        ptr = glGetString(GL_VERSION);
        version[0] = '\0';
        if(strstr(ptr, "ES")) {
                strcat(version, "#version 300 es");
        } else {
                strcat(version, "#version ");
                ptr = glGetString(GL_SHADING_LANGUAGE_VERSION);
                while(*ptr) {
                        if(*ptr != '.') {
                                strncat(version, ptr, 1);
                        }
                        ptr++;
                }
        }
        strcat(version, "\n");
        version_len = strlen(version);

        vs[0] = '\0';
        strcat(vs, version);
        str = vs + version_len;
        count = file_to_buffer("inner://res/font.vs", str, BUFF_SIZE);
        if(count < BUFF_SIZE) {
                str[count] = '\0';
        }

        fs[0] = '\0';
        strcat(fs, version);
        strcat(fs, "precision highp float;\n");
        str = fs + version_len + sizeof("precision highp float;\n") - 1;
        count = file_to_buffer("inner://res/font.fs", str, BUFF_SIZE);
        if(count < BUFF_SIZE) {
                str[count] = '\0';
        }
        game.shader = shader_new(vs, fs);
        
        /*
         * load font
         */
        game.characters = hash_table_new();
	FT_Init_FreeType(&game.ft);
	FT_New_Face(game.ft, "res/RobotoMono-Regular.ttf", 0, &game.face);
	FT_Set_Pixel_Sizes(game.face, 0, 12);

        game.text = file_to_string("inner://res/text");
        game.test_ptr = game.text->ptr;
	/*
	 * load vbo
	 */
        game.vao = vao_new();
        game.vbo = vbo_new();

        glBindVertexArray(game.vao->id);

        glBindBuffer(GL_ARRAY_BUFFER, game.vbo->id);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        vao_add_vbo(game.vao, game.vbo);
        
	glViewport(0, 0, g_device.width, g_device.height);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glUseProgram(game.shader->id);
	union mat4 projection;
	mat4_new_ortho(&projection, 
		0, g_device.width,
		0, g_device.height,
		-200, 100);
	 glUniformMatrix4fv(glGetUniformLocation(game.shader->id, "projection"), 
	 	1, GL_FALSE, projection.m);
}

static void __exit()
{
	FT_Done_Face(game.face);
	FT_Done_FreeType(game.ft); 
	
	ref_dec(&game.characters->base);
        ref_dec(&game.shader->base);
        ref_dec(&game.vbo->base);
        ref_dec(&game.vao->base);
        ref_dec(&game.pass_main->base);
        ref_dec(&game.text->base);

        SDL_GL_DeleteContext(game.context);
        SDL_DestroyWindow(game.window);
        SDL_Quit();
}

static struct character *__get_character(const char *c)
{
	unsigned len = utf8_width(c) * sizeof(char);
	struct ref *ref = hash_table_get(game.characters, c, len);

	if(ref) {
		return cast(ref, struct character, base);
	}
	
	struct character *p = character_new(game.face, c);
	if(!p) return NULL;
	
	hash_table_set(game.characters, &p->base, c, len);
	ref_dec(&p->base);
	return p;	
}

static void __draw_text_left(const char *text, const float x, const float y, const float scale, union vec3 color)
{
	glUseProgram(game.shader->id);
	glUniform3f(glGetUniformLocation(game.shader->id, "textColor"), color.x, color.y, color.z);
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(game.shader->id, "text"), 0);
	glBindVertexArray(game.vao->id);
	
	float xx = x, yy = y;
	unsigned check_count = 0;
	float xpos, ypos, w, h;
	float lx;
	struct character *c;
	
	const char *ptr = text;
	const char *lptr;
	while(*ptr) {
	
		c = __get_character(ptr);
		if(!c) goto next;
		
		switch(*ptr) {
			case '\t':
				xx += (c->advance >> 6) * scale * 4;
				goto next;
			case '\n':
				yy -= game.face->size->metrics.height >> 6;
				xx = x;
				goto next;
			case ' ': case '\v': case '\f': case '\r':
				xx += (c->advance >> 6) * scale;
				goto next;
			default:
				lptr = ptr;
				lx = xx;
				while(*lptr && !isspace(*lptr)) {
					c = __get_character(lptr);
					if(c) {
						xpos = lx + c->bearing_width * scale;
						w = c->width * scale;
						if(xpos + w > g_device.width - 6) {
							yy -= game.face->size->metrics.height >> 6;
							xx = x;
							break;
						}
						lx += (c->advance >> 6) * scale;
					}
					lptr += utf8_width(lptr);
				}
				break;
		}
		
		c = __get_character(ptr);
		
		check_count = 1;
	check:	
		xpos = xx + c->bearing_width * scale;
		ypos = yy - (c->height - c->bearing_height) * scale;

		w = c->width * scale;
		h = c->height * scale;
		
		if(xpos + w > g_device.width && check_count == 1) {
			xx = x;
			yy -= game.face->size->metrics.height >> 6;
			
			check_count++;
			goto check;
		}

		float vertices[6][4] = {
			{ xpos,     ypos + h,   0.0, 0.0 },            
			{ xpos,     ypos,       0.0, 1.0 },
			{ xpos + w, ypos,       1.0, 1.0 },

			{ xpos,     ypos + h,   0.0, 0.0 },
			{ xpos + w, ypos,       1.0, 1.0 },
			{ xpos + w, ypos + h,   1.0, 0.0 }           
		};
		
		glBindTexture(GL_TEXTURE_2D, c->texture->id);
		glBindBuffer(GL_ARRAY_BUFFER, game.vbo->id);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		
		xx += (c->advance >> 6) * scale;
		
	next:
		ptr += utf8_width(ptr);
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

static void __draw_text_center(const char *text, const float x, const float y, const float scale, union vec3 color)
{
	glUseProgram(game.shader->id);
	glUniform3f(glGetUniformLocation(game.shader->id, "textColor"), color.x, color.y, color.z);
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(game.shader->id, "text"), 0);
	glBindVertexArray(game.vao->id);
	
	float xx = x, yy = y;
	unsigned check_count = 0;
	float xpos, ypos, w, h;
	float xoffset;
	struct character *c;
	
	const char *ptr = text;
	const char *lptr = NULL;
	const char *lend = NULL;
	const char *lprev = NULL;
	const char *lprev_space = NULL;
	while(*ptr) {
		xoffset = 0;
		if(lptr == NULL) {
			lptr = ptr;
			lprev = lptr;
			w = x;
			h = 0;
			ypos = 0;
			while(*lptr) {
				c = __get_character(lptr);
				if(c) {
					switch(*lptr) {
						case '\t':
							xpos = (c->advance >> 6) * scale * 4; 
							if(w + xpos > g_device.width - 6) {
								lptr = lprev;
								goto finish_check;
							}
							w += xpos;
							if(isspace(*lprev) == 0) {
								lprev_space = lptr;
								ypos = w - xpos;
							}
							break;
						case ' ': case '\v': case '\f': case '\r':
							xpos = (c->advance >> 6) * scale; 
							if(w + xpos > g_device.width - 6) {
								lptr = lprev;
								goto finish_check;
							}
							w += xpos;
							if(isspace(*lprev) == 0) {
								lprev_space = lptr;
								ypos = w - xpos;
							}
							break;
						case '\n':
							goto finish_check;
						default:
							xpos = (c->advance >> 6) * scale; 
							if(w + xpos > g_device.width - 6) {
								lend = lptr + utf8_width(lptr);
								if((*lend) 
									&& ((isspace(*lend) == 0) || isspace(*lprev) == 0) 
									&& lprev_space) {						
									lptr = lprev_space;
									h = ypos;
								} else {
									lptr = lprev;
								}						
								goto finish_check;
							}
							w += xpos;
							h = w;
							break;
					}	
				}
				lend = lptr + utf8_width(lptr);
				if(!*lend) {
					break;
				}
				
				lprev = lptr;
				lptr = lend;
			}
		finish_check:
			xoffset = floor((g_device.width - 6 - h) / 2);		
		}
		
		c = __get_character(ptr);
		if(!c) goto next;
		
		switch(*ptr) {
			case '\t':
				xx += (c->advance >> 6) * scale * 4 + xoffset;
				goto next;
			case '\n':
				yy -= game.face->size->metrics.height >> 6;
				xx = x;
				goto next;
			case ' ': case '\v': case '\f': case '\r':
				xx += (c->advance >> 6) * scale + xoffset;
				goto next;
			default:
				break;
		}
		
		xpos = xx + c->bearing_width * scale + xoffset;
		ypos = yy - (c->height - c->bearing_height) * scale;

		w = c->width * scale;
		h = c->height * scale;

		float vertices[6][4] = {
			{ xpos,     ypos + h,   0.0, 0.0 },            
			{ xpos,     ypos,       0.0, 1.0 },
			{ xpos + w, ypos,       1.0, 1.0 },

			{ xpos,     ypos + h,   0.0, 0.0 },
			{ xpos + w, ypos,       1.0, 1.0 },
			{ xpos + w, ypos + h,   1.0, 0.0 }           
		};
		
		glBindTexture(GL_TEXTURE_2D, c->texture->id);
		glBindBuffer(GL_ARRAY_BUFFER, game.vbo->id);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		
		xx += (c->advance >> 6) * scale + xoffset;
		
	next:
		if(lptr && ptr >= lptr) {
			if(*ptr != '\n') {
				yy -= game.face->size->metrics.height >> 6;
				xx = x;
			}
			lptr = NULL;
		}
	
		ptr += utf8_width(ptr);
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

static void __draw_text_justified(const char *text, const float x, const float y, const float scale, union vec3 color)
{
	glUseProgram(game.shader->id);
	glUniform3f(glGetUniformLocation(game.shader->id, "textColor"), color.x, color.y, color.z);
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(game.shader->id, "text"), 0);
	glBindVertexArray(game.vao->id);
	
	float xx = x, yy = y;
	unsigned check_count = 0;
	float xpos, ypos, w, h, xtest;
	float xoffset, xoffset_last;
	int space_count, space_count_temp, space_count_prev;
	struct character *c, *cbs;
	
	const char *ptr = text;
	const char *lptr = NULL;
	const char *lend = NULL;
	const char *lprev = NULL;
	const char *lprev_space = NULL;
	const char *lptr_start, *lptr_stop;
	xoffset = 0;
	xoffset_last = 0;
	while(*ptr) {
		if(lptr == NULL) {
			lptr = ptr;
			lprev = lptr;
			w = x;
			h = 0;
			ypos = 0;
			xpos = 0;
			space_count = 0;
			space_count_temp = 0;
			space_count_prev = 0;
			lptr_start = NULL;
			lptr_stop = NULL;
			while(*lptr) {
				xoffset = -1;
				c = __get_character(lptr);
				if(c) {
					switch(*lptr) {
						case '\t':
							if(lptr_start) {
								xpos += (c->advance >> 6) * scale * 4; 
								if(w + xpos > g_device.width - 6) {
									if(lprev_space) {
										lptr = lprev_space;
										h = ypos;
										lptr_stop = lptr;
										space_count = space_count_prev;
									} else {
										lptr = lprev;
									}								
									goto finish_check;
								}
								space_count_temp++;
								if(isspace(*lprev) == 0) {
									lprev_space = lprev;
									ypos = w;
									space_count_prev = space_count;
									
									cbs = __get_character(lprev);
									ypos -= (cbs->advance >> 6) * scale;
									ypos += (cbs->bearing_width + cbs->width) * scale;
								}
							}
							break;
						case ' ': case '\v': case '\f': case '\r':
							if(lptr_start) {
								xpos += (c->advance >> 6) * scale; 
								if(w + xpos > g_device.width - 6) {
									if(lprev_space) {
										lptr = lprev_space;
										h = ypos;
										lptr_stop = lptr;
										space_count = space_count_prev;
									} else {
										lptr = lprev;
									}									
									goto finish_check;
								}
								space_count_temp++;
								if(isspace(*lprev) == 0) {
									lprev_space = lprev;
									ypos = w;
									space_count_prev = space_count;
									
									cbs = __get_character(lprev);
									ypos -= (cbs->advance >> 6) * scale;
									ypos += (cbs->bearing_width + cbs->width) * scale;
								}
							}
							
							break;
						case '\n':
							xoffset = 1;
							if(!lptr_start) {
								lptr_start = lptr;
								lptr_stop = lptr;
							}
							lptr_stop = lptr;
							goto finish_check;
						default:
							if(!lptr_start) {
								lptr_start = lptr;
								lptr_stop = lptr;
								xpos = 0;
								space_count_temp = 0;
							}
							xtest = (c->advance >> 6) * scale; 
							if(w + xpos + xtest > g_device.width - 6) {
								lend = lptr + utf8_width(lptr);
								if((*lend) 
									&& ((isspace(*lend) == 0) || isspace(*lprev) == 0) 
									&& lprev_space) {						
									lptr = lprev_space;
									h = ypos;
									lptr_stop = lptr;
									space_count = space_count_prev;
								} else {
									lptr = lprev;
									lptr_stop = lptr;
								}						
								goto finish_check;
							}
							w += xpos + xtest;
							h = w;
							xpos = 0;
							space_count += space_count_temp;
							space_count_temp = 0;
							lptr_stop = lptr;
							break;
					}	
					lprev = lptr;
				}
				lend = lptr + utf8_width(lptr);
				if(!*lend) {
					break;
				}
				lptr = lend;
			}
		finish_check:
			if(xoffset < 0 && space_count) {				
				xoffset = floor((g_device.width - 6 - h) / space_count);
				xoffset_last = 	(g_device.width - 6 - h) - xoffset * (space_count - 1);
			} else {
				xoffset = 0;
				xoffset_last = 0;
				lptr_stop = NULL;
			}
			space_count_temp = 0;					
		}
		
		c = __get_character(ptr);
		if(!c) goto next;
		
		if(lptr_start && ptr < lptr_start) goto next;
		
		if(lptr_stop && ptr > lptr_stop) goto next;
		
		switch(*ptr) {
			case '\t':				
				space_count_temp++;			
				if(space_count_temp < space_count) {
					xx += (c->advance >> 6) * scale * 4 + xoffset;
				} else {
					xx += (c->advance >> 6) * scale * 4 + xoffset_last;
					printf("last\n");
				}	
				
				goto next;
			case '\n':
				yy -= game.face->size->metrics.height >> 6;
				xx = x;
				goto next;
			case ' ': case '\v': case '\f': case '\r':				
				space_count_temp++;
				if(space_count_temp < space_count) {
					xx += (c->advance >> 6) * scale + xoffset;
				} else {
					xx += (c->advance >> 6) * scale + xoffset_last;
					printf("last\n");
				}	
				goto next;
			default:
				break;
		}
		
		xpos = xx + c->bearing_width * scale;
		ypos = yy - (c->height - c->bearing_height) * scale;

		w = c->width * scale;
		h = c->height * scale;

		float vertices[6][4] = {
			{ xpos,     ypos + h,   0.0, 0.0 },            
			{ xpos,     ypos,       0.0, 1.0 },
			{ xpos + w, ypos,       1.0, 1.0 },

			{ xpos,     ypos + h,   0.0, 0.0 },
			{ xpos + w, ypos,       1.0, 1.0 },
			{ xpos + w, ypos + h,   1.0, 0.0 }           
		};
		
		glBindTexture(GL_TEXTURE_2D, c->texture->id);
		glBindBuffer(GL_ARRAY_BUFFER, game.vbo->id);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		
		xx += (c->advance >> 6) * scale;
		
	next:
		if(lptr && ptr >= lptr) {
			if(*ptr != '\n') {
				yy -= game.face->size->metrics.height >> 6;
				xx = x;
			}
			lptr = NULL;
		}
	
		ptr += utf8_width(ptr);
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

static void __draw()
{
	struct pass *pass;
	
        pass = game.pass_main;

        if(pass->begin) {
                pass->begin(pass);
        }
        
        glClearColor(1, 1, 1, 1);
        glClear(GL_COLOR_BUFFER_BIT);
        
        if(game.test_ptr && *game.test_ptr) {
        	__draw_text_justified(game.test_ptr, 6, g_device.height - (game.face->size->metrics.height >> 6), 
        		1.0f, (union vec3){0, 0, 0});
        }           

        if(pass->end) {
                pass->end(pass);
        }
}

static unsigned enter = 0;

static void __update()
{
	if(enter) {
		if(*game.test_ptr) {
			game.test_ptr += utf8_width(game.test_ptr);
			game.draw = 1;
		}
		enter = 0;
	}
}

static void __load_local()
{
        if(game.loaded) {

                glGetIntegerv(GL_FRAMEBUFFER_BINDING, (signed *)&g_device.id_resolved);
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
                } else if (event.type == SDL_KEYUP &&
                        event.key.keysym.sym == SDLK_n) {
                                enter = 1;
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
