#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "stdbool.h"

typedef struct {
	uint16_t x;
	uint16_t y;
}Point;

typedef struct {
	union {
		struct {
			Point left_top;
			Point right_bottom;
		};
		struct {
		uint16_t left;
		uint16_t top;
		uint16_t right;
		uint16_t bottom;
		};
		uint64_t all;
	};
}Rectangle;

Point Point(uint16_t x, uint16_t y);
void PointMove(Point *p, int32_t x, int32_t y);
bool IsPointInRect(const Point* p, const Rectangle* r);

Rectangle RectFromPoints(const Point* leftTop, const Point* rightBottom);
Rectangle RectFromCoord(uint16_t left, uint16_t top, uint16_t right, uint16_t bottom);
Rectangle RectFromBounds(uint16_t x, uint16_t y, uint16_t width, uint16_t height);
bool IsRectEmpty(const Rectangle *r);

#endif //GEOMETRY_H
