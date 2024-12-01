#ifndef LIGHT_H
#define LIGHT_H

#include "Types.h"

#include "json.hpp"
#include <Eigen/Dense>


enum LightType
{
    AREA,
    POINT
};

class Light 
{
    public:
    LightType type;
    Color id, is;
    bool use;


    // Optional
    Eigen::Matrix<float, 4, 4> transform;
    unsigned int n;
    bool usecenter;

    // Point specifics
    Point center;

    // Area specifics
    Point p1, p2, p3, p4;


    Light(const nlohmann::json &light);
};


#endif