/*
 * main.c
 * Part of Mánagarmr project
 * 28. 11. 2019.
 */
#include <stdio.h>
#include <string.h>
#include <SDL2/SDL.h>
#include "lua.h"

static SDL_Window *window;
static SDL_Renderer *renderer;
static const int WINDOW_WIDTH  = 800;
static const int WINDOW_HEIGHT = 600;

static const char * restrict const PROMPT_LUA = "Lua] ";

static void
window_close(void)
{
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
}

/*
 * Lua functions - window manipulation.
 */
#define LUAFUNC_RETURN_WINDOW_CLOSE (-1)
static int
lua_window_close(lua_State *state)
{
	window_close();
	lua_pushnumber(state, LUAFUNC_RETURN_WINDOW_CLOSE);
	return 1;
}

/*
 * Lua functions - interpreter manipulation.
 */
#define LUAFUNC_RETURN_INTERPRETER_CLOSE (-2)
static int
lua_interpreter_close(lua_State *state)
{
	lua_pushnumber(state, LUAFUNC_RETURN_INTERPRETER_CLOSE);
	return 1;
}

inline static void
prompt(FILE * const f)
{
	fputs(PROMPT_LUA, f);
}

inline static void
stdioinput(lua_State *lstate)
{
	char buffer[256];
	int error, ret, interpreter_open = 1;
	while (interpreter_open)
	{
		/* Push the entered line as a chunk and call (execute) it. */
		prompt(stdout);
		interpreter_open = (fgets(buffer, sizeof(buffer), stdin) != NULL);
		error            = luaL_loadbuffer(lstate, buffer, strlen(buffer), "command") || lua_pcall(lstate, 0, 2, 0);
		if (error)
		{
			prompt(stderr);
			fprintf(stderr, "%s\n", lua_tostring(lstate, -1));
			lua_pop(lstate, 1);
		}
		/* Check the return value (for special functionality) */
		ret = lua_tonumber(lstate, 0);
		printf("Retrived %d\n",  ret);
		switch (ret)
		{
		case LUAFUNC_RETURN_INTERPRETER_CLOSE:
			interpreter_open = 0;
			break;
		case LUAFUNC_RETURN_WINDOW_CLOSE:
		default:
			break;
		}
	}
}

int
main(int argc, char **argv)
{
	SDL_Event e;
	int r;
	lua_State *lstate;

	SDL_Init(SDL_INIT_EVERYTHING);
	window   = SDL_CreateWindow("$title", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);
	lstate = luaL_newstate();
	luaL_openlibs(lstate);
	lua_pushcfunction(lstate, lua_window_close);
	lua_setglobal(lstate, "window_close");
	lua_pushcfunction(lstate, lua_interpreter_close);
	lua_setglobal(lstate, "close");

	r = 1;
	while(r)
	{
		SDL_PumpEvents();
		while (SDL_PollEvent(&e))
		{
			if (e.type == SDL_QUIT)
			{
				r = 0;
			}
			else if (e.type == SDL_KEYDOWN)
			{
				switch (e.key.keysym.sym)
				{
				case SDLK_BACKQUOTE:
					stdioinput(lstate);
				default:
					break;
				}
			}
		}
	}

	lua_close(lstate);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
