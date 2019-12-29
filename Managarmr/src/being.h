#pragma once
#define BEING_MAX_COUNT 512
#define BEING_COLOUR 0xFF, 0xFF, 0xFF, 0xFF
#define BEING_COORDINATE_COLOUR 0xFF, 0x00, 0x00, 0xFF
#define BEING_TARGET_COLOUR 0xDA, 0x8A, 0x00, 0xFF

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
};

struct Being
{
	struct Body body;
	struct Brain brain;
};

extern void being_act(struct Being *b);
extern void being_settarget(struct Being *b, float x, float y);
