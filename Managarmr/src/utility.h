/*
 * utility.h
 * Part of Mánagarmr project
 * 4. 12. 2019.
 *
 * Definition of utilites: functions, constants and macros.
 * Defines functions for easy printing of information, issues and warnings.
 *   (printinfo, printissue, printwarning)
 */
#pragma once
#include <stdio.h>

#define STR_INFO "[i] %s \n"
#define STR_ISSUE "[X] %s \n"
#define STR_WARNING "[w] %s \n"
#define STR_LUAPROMPT "Lua] "
#define STR_LUA "%s \n"

#define printinfo(x) fprintf(stdout, STR_INFO, x)
#define printissue(x) fprintf(stdout, STR_ISSUE, x)
#define printwarning(x) fprintf(stdout, STR_WARNING, x)
#define printluaprompt fputs(STR_LUAPROMPT, stdout);
#define printlua(x) fprintf(stdout, STR_LUA, x)
