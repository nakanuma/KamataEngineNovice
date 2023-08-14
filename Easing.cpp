#include "Easing.h"

float EaseInQuart(float t)
{
	return t * t * t * t;
}

float EaseInQuartPos(float start, float end, float t)
{
	float distance = end - start;
	distance *= EaseInQuart(t);

	return start + distance;
}

float EaseInQuad(float t)
{
	return t * t;
}

float EaseInQuadPos(float start, float end, float t)
{
	float distance = end - start;
	distance *= EaseInQuad(t);

	return start + distance;
}

