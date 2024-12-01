#ifndef RAY_H
#define RAY_H


#include "Geometry.h"
#include "PairedRoot.h"

#include <vector>


class Ray 
{
public:
	Point origin;
	Vec3 direction;

	Ray(Point origin, Vec3 direction);
	void intersectSphere(const Geometry& sphere, PairedRoot& root);
	void intersectRectangle(const Geometry& rectangle, PairedRoot& root);
	Point reach(float distance);

};


#endif