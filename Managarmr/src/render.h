#pragma once
#include <SDL2/SDL.h>
#include "being.h"

extern void render_beingtarget(SDL_Renderer *renderer, const struct Being * restrict being);
extern void render_being(SDL_Renderer *renderer, const struct Being * restrict being, int iscaught);
