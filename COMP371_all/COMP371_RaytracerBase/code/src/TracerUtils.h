#ifndef TRACERUTILS_H
#define TRACERUTILS_H

#include "Types.h"
#include "PairedRoot.h"
#include "Ray.h"
#include "Geometry.h"
#include "SceneInfo.h"
#include "Light.h"

#include <vector>
#include <Eigen/Dense>

bool tu_InsideRectangle(const Geometry& rectangle, Point &p);
bool tu_CloseToZero(float value);
PairedRoot tu_IntersectSceneGeometries(Ray& ray, SceneInfo& scene);
Eigen::Matrix3f tu_GetLocalCoordinatesFrame(Vec3 &normal, Vec3 &V);
bool tu_PointInShadow(Point &light_center, PairedRoot &eclipse, PairedRoot &hit, Point &p);
Vec3 tu_GetNormal(Point &p, const Geometry *geometry);
void tu_GammaCorrection(Color &color);

#endif
