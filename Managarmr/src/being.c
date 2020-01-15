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

void
being_setmovetarget(struct Being *b, float x, float y, float radius)
{
	b->brain.target.x      = x;
	b->brain.target.y      = y;
	b->brain.target_radius = radius;
}

int
being_reachedtarget(const struct Being * restrict b)
{
	struct Point2D d;
	d.x = b->brain.target.x-(b->body.position.x+b->body.base.x);
	d.y = b->brain.target.y-(b->body.position.y+b->body.base.y);
	float len = sqrt(d.x*d.x+d.y*d.y);
	return (len <= b->brain.target_radius);
}

int
being_hasflag(const struct Being * restrict b, const beingflag flag)
{
	return (b->brain.flags & flag);
}

void
being_toggleflag(struct Being * restrict b, const beingflag flag)
{
	b->brain.flags ^= flag;
}

void
being_setflag(struct Being * restrict b, const beingflag flag)
{
	b->brain.flags |= flag;
}

void
being_clearflag(struct Being * restrict b, const beingflag flag)
{
	b->brain.flags &= ~flag;
}

struct Being being_create(const struct BeingCreateInfo info)
{
	struct Being being;
	being.body.position       = info.position;
	being.body.size           = info.size;
	being.body.base.x         = being.body.size.x/2;
	being.body.base.y         = being.body.size.y/2;
	being.body.radius         = info.radius;
	being.brain.target_radius = BEING_TARGETRADIUS_MOVEMENT;
	being.brain.target        = info.position;
	being.brain.alignment     = info.alignment;
	being.brain.flags         = BEING_FLAG_NONE;
	return being;
}
