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
    Eigen::Vector3f center, up, lookat;
    RGB bkc, ai;
    std::string filename;
    unsigned int size[2];
    float fov;

    // Optional fields
    std::vector<unsigned int> raysperpixel;
    bool antialiasing, twosiderender, globalillum;
    float probterminate, maxbounces;


    OutputInfo(const nlohmann::json &output);
};

#endif