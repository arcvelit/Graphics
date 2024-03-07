#ifndef PAIREDROOT_H
#define PAIREDROOT_H


#include "Geometry.h"

class PairedRoot
{
public:
	const Geometry *geometry;
	float root;

	PairedRoot(const Geometry *geometry, float root);
};


#endif