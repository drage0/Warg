/*
 * main.c
 * Part of Mánagarmr project
 * 28. 11. 2019.
 */
#include <stdio.h>
#include <string.h>
#include <SDL2/SDL.h>
#include "lua.h"
#include "utility.h"

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

/*
 * Lua functions - system.
 */
#define LUAFUNC_RETURN_SYSTEM_BIND (-3)
static int
lua_system_bind(lua_State *lstate)
{
	const char* key;
	const char* function;
	key      = lua_tostring(lstate, -2);
	function = lua_tostring(lstate, -1);
	printf("Binding %s to %s.\n", key, function);
	lua_pop(lstate, 2);
	lua_pushnumber(lstate, LUAFUNC_RETURN_SYSTEM_BIND);
	return 1;
}

static void
stdioinput(lua_State *lstate)
{
	char buffer[256];
	int error, ret, interpreter_open = 1;
	while (interpreter_open)
	{
		/* Push the entered line as a chunk and call (execute) it. */
		printluaprompt;
		interpreter_open = (fgets(buffer, sizeof(buffer), stdin) != NULL);
		error            = luaL_loadbuffer(lstate, buffer, strlen(buffer), "command") || lua_pcall(lstate, 0, 2, 0);
		if (error)
		{
			printluaprompt;
			printlua(lua_tostring(lstate, -1));
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
		case LUAFUNC_RETURN_SYSTEM_BIND:
		default:
			break;
		}
	}
}

/*
 * Welcome message, the first message that is printed.
 */
#define AS_STR_EX(s) #s
#define AS_STR(s) AS_STR_EX(s)
#define WARG_SDLVERSION AS_STR(SDL_MAJOR_VERSION)"."AS_STR(SDL_MINOR_VERSION)"."AS_STR(SDL_PATCHLEVEL)
inline static void
welcomemessage(void)
{
	printinfo("- Warg project -");
	printinfo("- ---- ------- -");
	printinfo("- Compiler-  "__VERSION__);
	printinfo("- Timestamp- "__DATE__" @ "__TIME__);
	printinfo("- SDL-       "WARG_SDLVERSION);
}

int
main(int argc, char **argv)
{
	SDL_Event e;
	int r;
	lua_State *lstate;

	welcomemessage();
	SDL_Init(SDL_INIT_EVERYTHING);
	window   = SDL_CreateWindow("$title", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);
	lstate = luaL_newstate();
	luaL_openlibs(lstate);
	lua_pushcfunction(lstate, lua_window_close);
	lua_setglobal(lstate, "window_close");
	lua_pushcfunction(lstate, lua_interpreter_close);
	lua_setglobal(lstate, "close");
	lua_pushcfunction(lstate, lua_system_bind);
	lua_setglobal(lstate, "bind");

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
