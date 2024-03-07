#include "PairedRoot.h"

#include "Geometry.h"

PairedRoot::PairedRoot(const Geometry* geometry, float root) : geometry(geometry)
{
	this->root = root;
}