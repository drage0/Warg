/*
 * utility.h
 * Part of Warg project
 * 4. 12. 2019.
 *
 * Definition of utilites: functions, constants and macros.
 * Defines functions for easy printing of information, issues and warnings.
 *   (printinfo, printissue, printwarning)
 */
#pragma once
#include <stdio.h>

#define STR_INFO "[i] "
#define STR_ISSUE "[X] "
#define STR_WARNING "[w] "
#define STR_LUA "Lua] "
#define STR_NEWLINE "\n"

#define printinfo(x,...) fprintf(stdout, STR_INFO x STR_NEWLINE, __VA_ARGS__)
#define printissue(x,...) fprintf(stdout, STR_ISSUE x STR_NEWLINE, __VA_ARGS__)
#define printwarning(x,...) fprintf(stdout, STR_WARNING x STR_NEWLINE, __VA_ARGS__)
#define printluaprompt fputs(STR_LUA, stdout);
#define printlua(x,...) fprintf(stdout, STR_LUA x STR_NEWLINE, __VA_ARGS__)

