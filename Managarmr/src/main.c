/*
 * main.c
 * Part of Warg project
 * 28. 11. 2019.
 */
#include <stdio.h>
#include <string.h>
#include <SDL2/SDL.h>
#include "lua.h"
#include "utility.h"

#define KEYBIND_SEQUENCE_MAX_LENGTH 256
#define KEYBIND_MAX 64
struct KeyBind
{
	SDL_Keycode key;
	char sequence[KEYBIND_SEQUENCE_MAX_LENGTH];
};

#define WINDOW_TITLE_MAX 64
static SDL_Window *window;
static SDL_Renderer *renderer;
static int window_width  = 800;
static int window_height = 600;
static int window_vsync  = 1;
static char window_title[WINDOW_TITLE_MAX] = "~mánagarmr";
static struct KeyBind keybinds[KEYBIND_MAX];
static unsigned int keybind_count = 0;

static int interpreter_open = 0;
static int sys_running = 1;

static void
window_close(void)
{
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
}

inline static void
sys_exit(void)
{
	sys_running = 0;
}

/*
 * Lua functions - interpreter manipulation.
 */
static int
lua_interpreter_close(lua_State *state)
{
	interpreter_open = 0;
	return 0;
}

/*
 * Lua functions - system.
 */
static int
lua_system_bind(lua_State *lstate)
{
	struct KeyBind bind;
	const char *key, *sequence;
	/*
	lua_CFunction function;
	key      = lua_tostring(lstate, -2);
	function = lua_tocfunction(lstate, -1);
	printf("Binding %-6s to %p.\n", key, function);
	lua_pop(lstate, 2);
	function(lstate);
	*/
	key      = lua_tostring(lstate, -2);
	sequence = lua_tostring(lstate, -1);
	printinfo("Binding %-6s to \"%s\".", key, sequence);
	bind.key = SDL_GetKeyFromName(key);
	if (bind.key == SDLK_UNKNOWN)
	{
		printwarning("%s", "The key is unknown. Not binding.");
		return 0;
	}
	strncpy(bind.sequence, sequence, KEYBIND_SEQUENCE_MAX_LENGTH);
	/* Copy the created structure. */
	memcpy(&keybinds[keybind_count], &bind, sizeof(struct KeyBind));
	keybind_count += 1;
	return 0;
}

static int
lua_system_exit(lua_State *lstate)
{
	sys_exit();
	return 0;
}

static void
stdioinput(lua_State *lstate)
{
	char buffer[256];
	int error, ret;
	interpreter_open = 1;
	while (interpreter_open)
	{
		/* Push the entered line as a chunk and call (execute) it. */
		printluaprompt;
		interpreter_open = (fgets(buffer, sizeof(buffer), stdin) != NULL);
		error            = luaL_loadbuffer(lstate, buffer, strlen(buffer), "command") || lua_pcall(lstate, 0, 1, 0);
		if (error)
		{
			printluaprompt;
			printlua("%s\n", lua_tostring(lstate, -1));
			lua_pop(lstate, 1);
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
	printinfo("%s", "- Warg project -");
	printinfo("%s", "- ---- ------- -");
	printinfo("- Compiler-  %s", __VERSION__);
	printinfo("- Timestamp- %s @ %s", __DATE__, __TIME__);
	printinfo("- SDL-       %s", WARG_SDLVERSION);
}

/*
 * Execute the lua script contained in the given file.
 * Returns 0 on success, 1 on error.
 */
static int
executescript(lua_State *lstate, const char* path)
{
	int ret = luaL_dofile(lstate, path);
	if (ret != 0)
	{
		printwarning("Execution of \"%s\" failed. luaL_dofile returned %d.", path, ret);
		printluaprompt;
		printlua("%s", lua_tostring(lstate, -1));
		lua_pop(lstate, 1);
		return 1;
	}
	return 0;
}

/* Return the integer field value on success and the passed default value on error. */
static int
fieldvalue_int(lua_State *lstate, const char *field, int def)
{
	int value = def;
	lua_pushstring(lstate, field);
	lua_gettable(lstate, -2);
	if (!lua_isnumber(lstate, -1))
	{
		printissue("Field %s doesn't hold a number.", field);
	}
	else
	{
		value = (int) (lua_tonumber(lstate, -1));
	}
	lua_pop(lstate, 1);
	return value;
}

/* Return the string field value on success and the passed default value on error. */
static const char*
fieldvalue_str(lua_State *lstate, const char *field, const char *def)
{
	const char *value = def;
	lua_pushstring(lstate, field);
	lua_gettable(lstate, -2);
	if (!lua_isstring(lstate, -1))
	{
		printissue("Field %s doesn't hold a string.", field);
	}
	else
	{
		value = (const char*) (lua_tostring(lstate, -1));
	}
	lua_pop(lstate, 1);
	return value;
}

/*
 * Parse the lua state for the specific values.
 * The special varaibles such as the table 'window' can be used to configure the program.
 */
static int
parseconfiguration(lua_State *lstate)
{
	lua_getglobal(lstate, "window");
	if (!lua_istable(lstate, -1))
	{
		printissue("%s", "'window' is not a valid table.");
		return 1;
	}
	else
	{
		const char *stringpointer;
		window_width  = fieldvalue_int(lstate, "width",  window_width);
		window_height = fieldvalue_int(lstate, "height", window_height);
		window_vsync  = fieldvalue_int(lstate, "vsync",  window_vsync);
		stringpointer = fieldvalue_str(lstate, "title", window_title);
		strncpy(window_title, stringpointer, WINDOW_TITLE_MAX);
	}
	return 0;
}

/*
 * Execute the lua script contained in the given string.
 * Returns 0 on success, 1 on error.
 */
static int
executesequence(lua_State *lstate, const char *command)
{
	int ret;
	printlua("Execute %s!", command);
	ret = luaL_loadstring(lstate, command) || lua_pcall(lstate, 0, 0, 0);
	if (ret)
	{
		printlua("%s", lua_tostring(lstate, -1));
		lua_pop(lstate, 1);
	}
	return ret;
}

/*
 * Draw the statusbar.
 */
#define STATUSBAR_COLOUR 0xFF, 0x00, 0x00, 0xFF
#define STATUSBAR_Y ((int)(window_height*4.0/5.0))
#define STATUSBAR_H (window_height-STATUSBAR_Y)
void
statusbar_draw(void)
{
	SDL_Rect statusbar;
	statusbar.x = 0;
	statusbar.y = STATUSBAR_Y;
	statusbar.w = window_width;
	statusbar.h = STATUSBAR_H;
	SDL_SetRenderDrawColor(renderer, STATUSBAR_COLOUR);
	SDL_RenderFillRect(renderer, &statusbar);
}

int
main(int argc, char **argv)
{
	Uint32 rendererflags;
	SDL_Event e;
	lua_State *lstate;

	welcomemessage();
	SDL_Init(SDL_INIT_EVERYTHING);

	/* Begin the Lua state */
	lstate = luaL_newstate();
	luaL_openlibs(lstate);
	lua_pushcfunction(lstate, lua_interpreter_close);
	lua_setglobal(lstate, "close");
	lua_pushcfunction(lstate, lua_system_bind);
	lua_setglobal(lstate, "bind");
	lua_pushcfunction(lstate, lua_system_exit);
	lua_setglobal(lstate, "exit");
	lua_pushcfunction(lstate, lua_system_exit);
	lua_setglobal(lstate, "quit");

	/* Execute the configuration script. */
	executescript(lstate, "./data/scripts/configuration.lua");
	if (parseconfiguration(lstate))
	{
		printissue("%s", "Configuration file was not parsed correctly!");
	}

	rendererflags = SDL_RENDERER_ACCELERATED;
	if (window_vsync)
	{
		rendererflags = rendererflags | SDL_RENDERER_PRESENTVSYNC;
	}
	window   = SDL_CreateWindow(window_title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window_width, window_height, SDL_WINDOW_SHOWN);
	renderer = SDL_CreateRenderer(window, -1, rendererflags);

	while(sys_running)
	{
		SDL_PumpEvents();
		while (SDL_PollEvent(&e))
		{
			if (e.type == SDL_QUIT)
			{
				sys_exit();
			}
			else if (e.type == SDL_KEYDOWN)
			{
				unsigned int i;
				SDL_Keycode keycode = e.key.keysym.sym;
				switch (keycode)
				{
				case SDLK_BACKQUOTE:
					stdioinput(lstate);
				default:
					break;
				}
				for (i = 0; i < keybind_count; i++)
				{
					if (keycode == keybinds[i].key)
					{
						executesequence(lstate, keybinds[i].sequence);
					}
				}
			}
		}
		/* Render the frame. */
		statusbar_draw();
		SDL_RenderPresent(renderer);
	}

	lua_close(lstate);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
