/*
 * main.c
 * Part of Mánagarmr project
 * 28. 11. 2019.
 */
#include <stdio.h>
#include <string.h>
#include "lua.h"

static const char * restrict const PROMPT_LUA = "Lua] ";

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

	lstate = luaL_newstate();
	luaL_openlibs(lstate);
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

	return 0;
}
