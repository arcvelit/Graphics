#include "TracerUtils.h"

#include "Types.h"
#include "PairedRoot.h"
#include "Geometry.h"
#include "SceneInfo.h"
#include "Ray.h"

#include <Eigen/Dense>

#include <cmath>
#include <vector>
#include <algorithm>


bool tu_InsideRectangle(const Geometry& rectangle, Point &p)
{
    Vec3 n = rectangle.normal;

    Vec3 BA = rectangle.p2 - rectangle.p1;
    Vec3 CB = rectangle.p3 - rectangle.p2;
    Vec3 DC = rectangle.p4 - rectangle.p3;
    Vec3 AD = rectangle.p1 - rectangle.p4;

    Vec3 PA = p - rectangle.p1;
    Vec3 PB = p - rectangle.p2;
    Vec3 PC = p - rectangle.p3;
    Vec3 PD = p - rectangle.p4;

    bool s1 = std::signbit(BA.cross(PA).dot(n));
    bool s2 = std::signbit(CB.cross(PB).dot(n));
    bool s3 = std::signbit(DC.cross(PC).dot(n));
    bool s4 = std::signbit(AD.cross(PD).dot(n));

    return (s1 == s2 && s2 == s3 && s3 == s4);
}

bool tu_CloseToZero(float value)
{
    const float TOLERANCE = 1e-4;
    return std::fabs(value) < TOLERANCE;
}

PairedRoot tu_IntersectSceneGeometries(Ray &ray, SceneInfo &scene)
{
    PairedRoot hit = PairedRoot(nullptr, -1.0f);
    for (Geometry &geometry : scene.geometries)
    {
        if (geometry.type == SPHERE)
        {
            ray.intersectSphere(geometry, hit);
        }
        else if (geometry.type == RECTANGLE)
        {
            ray.intersectRectangle(geometry, hit);
        }
    }
    return hit;
}

Eigen::Matrix3f tu_GetLocalCoordinatesFrame(Vec3 &normal)
{
        int i = normal.array().abs().maxCoeff();
        Vec3 V;
        V.setZero();
        V.coeffRef(i) = 1.0f;

        Eigen::Matrix3f model_frame;
        Vec3 x_plane = V.cross(normal).normalized();
        Vec3 y_plane = normal.cross(x_plane).normalized();

        model_frame.col(0) = x_plane;
        model_frame.col(1) = normal;
        model_frame.col(2) = y_plane;

        return model_frame;
}

bool tu_PointInShadow(Point &light_center, PairedRoot &eclipse, PairedRoot &hit, Point &p)
{
    return (eclipse.geometry != nullptr && eclipse.geometry != hit.geometry) && ((light_center - p).squaredNorm() > eclipse.root * eclipse.root);
}