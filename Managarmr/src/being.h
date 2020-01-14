#pragma once
#define BEING_MAX_COUNT 512
#define BEING_COLOUR 0xFF, 0xFF, 0xFF, 0xFF
#define BEING_COORDINATE_COLOUR 0xFF, 0x00, 0x00, 0xFF
#define BEING_TARGET_COLOUR 0xDA, 0x8A, 0x00, 0xFF
#define BEING_ALIGNMENT_BLU 1
#define BEING_ALIGNMENT_RED 2
#define BEING_TARGETRADIUS_MOVEMENT 2.0f
#define BEING_FLAG_NONE 0
#define BEING_FLAG_SELECTED (1<<0)

typedef unsigned int beingflag;

struct Point2D
{
  float x, y;
};

struct Body
{
	struct Point2D position;
	struct Point2D base;
	struct Point2D size;
};

struct Brain
{
	struct Point2D target; /* Chasing target position. */
	int target_radius;     /* The target destination is satisfied in this radius. */
	int alignment;
	beingflag flags;
};

struct Being
{
	struct Body body;
	struct Brain brain;
};

/*
 * Information passed when creating a new Being.
 */
struct BeingCreateInfo
{
	struct Point2D position;
	struct Point2D size;
	int alignment;
};

extern void being_act(struct Being *b);
extern void being_settarget(struct Being *b, float x, float y, float radius);
extern int  being_reachedtarget(const struct Being * restrict b);
extern int  being_hasflag(const struct Being * restrict b, const beingflag flag);
extern void being_toggleflag(struct Being * restrict b, const beingflag flag);
extern void being_setflag   (struct Being * restrict b, const beingflag flag);
extern void being_clearflag (struct Being * restrict b, const beingflag flag);
extern struct Being being_create(const struct BeingCreateInfo info);
