#ifndef TRACERUTILS_H
#define TRACERUTILS_H

#include "Types.h"
#include "PairedRoot.h"
#include "Ray.h"
#include "SceneInfo.h"
#include "Light.h"

#include <vector>
#include <Eigen/Dense>

bool tu_InsideRectangle(const Geometry& rectangle, Point &p);
PairedRoot tu_SmallestPositiveRoot(std::vector<PairedRoot>& roots);
bool tu_CloseToZero(float value);
PairedRoot tu_IntersectSceneGeometries(Ray& ray, SceneInfo& scene);

#endif
