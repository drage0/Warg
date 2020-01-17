/*
 * main.c
 * Part of Warg project
 * 28. 11. 2019.
 */
#include <stdio.h>
#include <string.h>
#include <SDL2/SDL.h>
#include "render.h"
#include "lua.h"
#include "utility.h"
#include "being.h"

#define KEYBIND_SEQUENCE_MAX_LENGTH 256
#define KEYBIND_MAX 64
/*
 * Structure holding the key and its assosiated command.
 * key- the key code of the bound key.
 * sequence- Lua script to be executed (given as a string.)
 */
struct KeyBind
{
	SDL_Keycode key;
	char sequence[KEYBIND_SEQUENCE_MAX_LENGTH];
};
static struct KeyBind keybinds[KEYBIND_MAX];
static unsigned int keybind_count = 0;

/* Window system */
#define WINDOW_TITLE_MAX 64
static SDL_Window *window;
static SDL_Renderer *renderer;

/*
 * Variables bound with lua script(s).
 * selection_coour- the colour of the selection rectangle.
 * interpreter_open- is the lua interpreter open?
 * sys_running- should the main loop still be running?
 * scene_drawtargets- draw the beings' target destination.
 * window_width- window's width.
 * window_height- window's height.
 * window_title- the title of the window (caption).
 */
typedef int  intbound;
typedef char charbound;
static intbound selection_colour[4] = {255, 0, 0, 255};
static intbound interpreter_open = 0;
static intbound sys_running = 1;
static intbound scene_drawtargets;
static intbound window_width  = 800;
static intbound window_height = 600;
static intbound window_vsync  = 1;
static charbound window_title[WINDOW_TITLE_MAX] = "~mánagarmr";

static void
window_close(void)
{
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
}

inline static void
sys_exit(void)
{
	printinfo("%s", "Called sys_exit.");
	sys_running = 0;
}

inline static void
scene_toggletargets(void)
{
	scene_drawtargets = 1-scene_drawtargets;
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

/*
 * Lua functions - scene.
 */
static int
lua_scene_toggletargets(lua_State *lstate)
{
	scene_toggletargets();
	return 0;
}

/*
 * Call sys_exit() and lua_interpreter_close().
 * Effectively setting the variable to exit the program and close the
 * interpreter.
 */
static int
lua_system_exit(lua_State *lstate)
{
	sys_exit();
	lua_interpreter_close(lstate);
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
	int i;
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
	lua_getglobal(lstate, "selection_colour");
	if (!lua_istable(lstate, -1))
	{
		printissue("'%s' is not a valid table.", "selection_colour");
		return 1;
	}
	lua_pushnil(lstate);
	for (i = 0; i < 4; i++)
	{
		if (lua_next(lstate, -2) == 0)
		{
			printissue("'%s' table/array/ does not have enough elements!", "selection_colour");
			return 1;
		}
		selection_colour[i] = lua_tointeger(lstate, -1);
		lua_pop(lstate, 1);
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
 * Set the being count and initialize the array that's holding all beings.
 * BEING_MAX_COUNT is defined in `being.h`.
 */
static struct Being beings[BEING_MAX_COUNT];
static int being_count;
static void
beings_spawn(void)
{
	struct BeingCreateInfo info;
	being_count = 4;
	/* Prepare the info and create new beings. */
	info.position.x = 44;
	info.position.y = 144;
	info.size.x     = 16;
	info.size.y     = 24;
	info.radius     = 16.0f;
	info.alignment  = BEING_ALIGNMENT_BLU;
	beings[0] = being_create(info);
	info.position.x = 44;
	info.position.y = 200;
	beings[1] = being_create(info);
	info.position.x = 24;
	info.position.y = 188;
	beings[2] = being_create(info);
	info.position.x = 54;
	info.position.y = 388;
	beings[3] = being_create(info);
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

/*
 * Set a new movement target to all selected units.
 */
static void
unit_selectedcommandmove(int targetx, int targety)
{
	float radius = BEING_TARGETRADIUS_MOVEMENT;
	int numberselected = 0;
	for (int i = 0; i < being_count; i++)
	{
		if (being_hasflag(&beings[i], BEING_FLAG_SELECTED))
		{
			being_setmovetarget(&beings[i], targetx, targety, radius);
			radius         = radius+beings[i].body.radius;
			numberselected = numberselected+1;
		}
	}
}

/*
 * TODO /ideas/
 */
void
catchunits(SDL_Rect net, struct Being *beings, int being_count)
{
	int i;
	printinfo("net- %d %d %d %d", net.x, net.y, net.w, net.h);
	for (i = 0; i < being_count; i++)
	{
		SDL_Rect br, intr;
		br.x = (int) beings[i].body.position.x;
		br.y = (int) beings[i].body.position.y;
		br.w = (int) beings[i].body.size.x;
		br.h = (int) beings[i].body.size.y;
		if (SDL_IntersectRect(&net, &br, &intr))
		{
			printinfo("%s %d %d %d %d", "Caught.", intr.x, intr.y, intr.w, intr.h);
			being_setflag(&beings[i], BEING_FLAG_SELECTED);
		}
		else
		{
			being_clearflag(&beings[i], BEING_FLAG_SELECTED);
		}
	}
	return;
}

int
main(int argc, char **argv)
{
	Uint32 rendererflags;
	SDL_Event e;
	lua_State *lstate;
	SDL_Rect catch_net;
	SDL_Rect fade_net;
	int catching;
	int fade_net_alpha;

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
	lua_pushcfunction(lstate, lua_scene_toggletargets);
	lua_setglobal(lstate, "toggletargets");

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
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

	catching = 0;
	fade_net_alpha = selection_colour[3];
	beings_spawn();
	while(sys_running)
	{
		const Uint8 *heldkeys;
		heldkeys = SDL_GetKeyboardState(NULL);
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
			else if (e.type == SDL_MOUSEBUTTONDOWN)
			{
				if (e.button.button == SDL_BUTTON_LEFT)
				{
					catch_net.x = e.button.x;
					catch_net.y = e.button.y;
					catch_net.w = 0;
					catch_net.h = 0;
					/* Only start catching if the mouse was clicked on the board! */
					catching = (catch_net.y < STATUSBAR_Y);
				}
			}
			else if (e.type == SDL_MOUSEBUTTONUP)
			{
				if (catching && e.button.button == SDL_BUTTON_LEFT)
				{
					if (catch_net.w < 0)
					{
						catch_net.x += catch_net.w;
						catch_net.w *= -1;
					}
					if (catch_net.h < 0)
					{
						catch_net.y += catch_net.h;
						catch_net.h *= -1;
					}
					fade_net = catch_net;
					catchunits(catch_net, beings, being_count);
					catching = 0;
					fade_net_alpha = selection_colour[3];
				}
				else if (e.button.button == SDL_BUTTON_RIGHT)
				{
					int mx, my, i;
					SDL_GetMouseState(&mx, &my);
					if (my < STATUSBAR_Y)
					{
						unit_selectedcommandmove(mx, my);
					}
				}
			}
			else if (e.type == SDL_MOUSEMOTION)
			{
				catch_net.w = e.motion.x-catch_net.x;
				if (e.motion.y <= STATUSBAR_Y)
				{
					catch_net.h = e.motion.y-catch_net.y;
				}
			}
		}
		/* Update and render the frame. */
		SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
		SDL_RenderClear(renderer);
		statusbar_draw();
		for (int i = 0; i < being_count; i++)
		{
			being_act(&beings[i]);
			render_being(renderer, &beings[i], being_hasflag(&beings[i], BEING_FLAG_SELECTED));
			if(scene_drawtargets && !being_reachedtarget(&beings[i]))
			{
				render_beingtarget(renderer, &beings[i]);
			}
		}
		if (catching)
		{
			SDL_SetRenderDrawColor(renderer, selection_colour[0], selection_colour[1], selection_colour[2], selection_colour[3]);
			SDL_RenderFillRect(renderer, &catch_net);
		}
		else
		{
			SDL_SetRenderDrawColor(renderer, selection_colour[0], selection_colour[1], selection_colour[2], fade_net_alpha);
			SDL_RenderFillRect(renderer, &fade_net);
			if (fade_net_alpha-1 > 0)
			{
				fade_net_alpha = fade_net_alpha-1;
			}
		}
		SDL_RenderPresent(renderer);
	}

	lua_close(lstate);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
