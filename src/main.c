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

static void window_close(void)
{
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
}

/*
 * Lua functions - window manipulation.
 */
static int lua_window_close(lua_State *state)
{
	window_close();
	lua_pushnumber(state, 0);
	return 1;
}

inline static void prompt(FILE * const f)
{
	fputs(PROMPT_LUA, f);
}

int
main(int argc, char **argv)
{
	char buffer[256];
	int havecommand, error;
	lua_State *lstate;

	SDL_Init(SDL_INIT_EVERYTHING);
	window   = SDL_CreateWindow("$title", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);

	lstate = luaL_newstate();
	luaL_openlibs(lstate);
	lua_pushcfunction(lstate, lua_window_close);
	lua_setglobal(lstate, "window_close");
	havecommand = 1;
	while (havecommand)
	{
		/* Push the entered line as a chunk and call (execute) it. */
		prompt(stdout);
		havecommand = (fgets(buffer, sizeof(buffer), stdin) != NULL);
		error = luaL_loadbuffer(lstate, buffer, strlen(buffer), "command") || lua_pcall(lstate, 0, 0, 0);
		if (error)
		{
			prompt(stderr);
			fprintf(stderr, "%s\n", lua_tostring(lstate, -1));
			lua_pop(lstate, 1);
		}
	}
	lua_close(lstate);

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
