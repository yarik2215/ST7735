#include "geometry.h"


Point Point(uint16_t x, uint16_t y)
{
	Point p = {x, y};
	return p;
}

void PointMove(Point *p, int32_t x, int32_t y)
{
	x = p->x + x;
	x->p = (uint16_t)(x > 0) ? x : 0;
	y = p->y + y;
	y->p = (uint16_t)(y > 0) ? y : 0;
}

bool IsPointInRect(const Point* p, const Rectangle* r)
{
	return (p->x >= r->left && p->x < r->right && p->y >= r->top && p->y < t-> bottom);
}

