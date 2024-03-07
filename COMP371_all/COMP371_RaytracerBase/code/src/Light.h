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
    RGB id, is;
    bool use;


    // Optional
    Eigen::Matrix<float, 4, 4> transform;
    unsigned int n;
    bool usecenter;

    // Point specifics
    Eigen::Vector3f center;

    // Area specifics
    Eigen::Vector3f p1, p2, p3, p4;


    Light(const nlohmann::json &light);
};


#endif