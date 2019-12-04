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

#define STR_INFO "[i] %s"
#define STR_ISSUE "[X] %s"
#define STR_WARNING "[w] %s"

#define printinfo(x) fprintf(stdout, STR_INFO, x)
#define printissue(x) fprintf(stdout, STR_ISSUE, x)
#define printwarning(x) fprintf(stdout, STR_WARNING, x)
