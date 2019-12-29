#include "render.h"

void
render_beingtarget(SDL_Renderer *renderer, const struct Being * restrict being)
{
	SDL_Point start, end;
	start.x = being->body.position.x+being->body.base.x;
	start.y = being->body.position.y+being->body.base.y;
	end.x   = being->brain.target.x;
	end.y   = being->brain.target.y;
	SDL_SetRenderDrawColor(renderer, BEING_TARGET_COLOUR);
	SDL_RenderDrawLine(renderer, start.x, start.y, end.x, end.y);
}

void
render_being(SDL_Renderer *renderer, const struct Being * restrict being, int iscaught)
{
	SDL_Rect br;
	br.x = (int) being->body.position.x;
	br.y = (int) being->body.position.y;
	br.w = (int) being->body.size.x;
	br.h = (int) being->body.size.y;
	SDL_SetRenderDrawColor(renderer, BEING_COLOUR);
	SDL_RenderFillRect(renderer, &br);
	if (iscaught)
	{
		br.x -= 4;
		br.y -= 4;
		br.w += 8;
		br.h += 8;
		SDL_RenderDrawRect(renderer, &br);
	}
}
