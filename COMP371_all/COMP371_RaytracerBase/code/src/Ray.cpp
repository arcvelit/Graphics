#include "Ray.h"

#include "TracerUtils.h"
#include "Geometry.h"
#include "PairedRoot.h"
#include "Types.h"


#include <Eigen/Dense>

#include <vector>
#include <algorithm>
#include <cmath>

#include <iostream>



Ray::Ray(Point origin, Vec3 direction)
{
	this->origin = origin;
	this->direction = direction.normalized();
}

void Ray::intersectSphere(const Geometry &sphere, PairedRoot &hit)
{
    // Solve sphere intersect equation
    auto G_vec = origin - sphere.center;

    float r = sphere.radius;

    float a = direction.squaredNorm();
    float b = 2 * direction.dot(G_vec);
    float c = G_vec.squaredNorm() - r * r;

    float discriminant = b * b - 4 * a * c;

    // Single root with multiplicity 2
    if (tu_CloseToZero(discriminant))
    {
        float root = -b / (2 * a);
        if (root > 0 && (hit.root < 0 || hit.root > root))
        {
            hit.geometry = &sphere;
            hit.root = root;
        }
    }
    // Real roots
    else if (discriminant > 0)
    {
        float root1 = (-b + std::sqrt(discriminant)) / (2 * a);
        float root2 = (-b - std::sqrt(discriminant)) / (2 * a);

        float minroot = std::min(root1, root2);
        if (minroot > 0 && (hit.root < 0 || hit.root > minroot))
        {
            hit.geometry = &sphere;
            hit.root = minroot;
        }
    }

}

void Ray::intersectRectangle(const Geometry& rectangle, PairedRoot& hit)
{
    Vec3 normal = rectangle.normal;

    Vec3 G_vec = rectangle.p1 - origin;

    float VdotN = direction.dot(rectangle.normal);

    if (!tu_CloseToZero(VdotN))
    {
        float GdotN = G_vec.dot(rectangle.normal);
        float t = GdotN / VdotN;
        Vec3 p = origin + t * direction;

        if (tu_InsideRectangle(rectangle, p) && t > 1e-4 && (hit.root < 0 || hit.root > t))
        {
            hit.geometry = &rectangle;
            hit.root = t;
        }
    }
}

Point Ray::reach(float distance)
{
    return origin + distance * direction;
}