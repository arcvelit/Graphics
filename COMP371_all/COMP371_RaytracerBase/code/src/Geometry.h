#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "Types.h"


#include <string>
#include <Eigen/Dense>
#include "json.hpp"


enum Shape
{
    RECTANGLE,
    SPHERE
};

class Geometry {
public:
    Shape type;
    float ka, kd, ks, pc;
    RGB ac, dc, sc;
    bool visible;
    std::string comment;


    // Rectangle specifics
    Point p1, p2, p3, p4;
    Vec3 normal;


    // Sphere specifics
    Point center;
    float radius;


    // Optional
    Eigen::Matrix<float, 4, 4> transform;

    Geometry(const nlohmann::json &geometry);
};

#endif
