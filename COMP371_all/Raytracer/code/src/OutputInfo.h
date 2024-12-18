#ifndef OUTPUTINFO_H
#define OUTPUTINFO_H

#include "Types.h"

#include "json.hpp"

#include <Eigen/Dense>
#include <string>
#include <vector>

class OutputInfo
{
    public:
    Point center;
    Vec3 up, lookat;
    Color bkc, ai;
    std::string filename;
    unsigned int size[2];
    float fov;

    // Optional fields
    std::vector<unsigned int> raysperpixel;
    bool antialiasing, twosiderender, globalillum;
    float probterminate;
    unsigned int maxbounces;


    OutputInfo(const nlohmann::json &output);
};

#endif