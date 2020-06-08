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

Rectangle RectFromPoints(const Point* leftTop, const Point* rightBottom)
{
	Rectangle rect;
	rect->left_top = leftTop;
	rect->right_bottom = rightBottom;
	return rect;
}

Rectangle RectFromCoord(uint16_t left, uint16_t top, uint16_t right, uint16_t bottom)
{
	Rectangle rect;
	rect->left = left;
	rect->top = top;
	rect->right = right;
	rect->bottom = bottom;
	return rect;
}

Rectangle RectFromBounds(uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
	Rectangle rect;
	rect->left = x;
	rect->top = y;
	rect->right = x+width;
	rect->bottom = y+height;
	return rect;
}

bool IsRectEmpty(const Rectangle *r)
{
	return (rect->left < rect->rigth && rect->top < rect.bottom);
}
