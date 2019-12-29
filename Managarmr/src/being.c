#include "being.h"
#include <math.h>

void
being_act(struct Being *b)
{
	struct Point2D d;
	d.x = b->brain.target.x-(b->body.position.x+b->body.base.x);
	d.y = b->brain.target.y-(b->body.position.y+b->body.base.y);
	float len = sqrt(d.x*d.x+d.y*d.y);
	if (len > b->brain.target_radius)
	{
		d.x = d.x/len;
		d.y = d.y/len;
		b->body.position.x += d.x;
		b->body.position.y += d.y;
	}
}

void being_settarget(struct Being *b, float x, float y)
{
	b->brain.target.x = x;
	b->brain.target.y = y;
}
