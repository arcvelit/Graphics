#include "OutputInfo.h"

#include "json.hpp"

#include <Eigen/Dense>

#include <vector>

#define _USE_MATH_DEFINES

#include <cmath>


OutputInfo::OutputInfo(const nlohmann::json &output)
{
    lookat = Eigen::Vector3f(output["lookat"][0], output["lookat"][1], output["lookat"][2]);
    center = Eigen::Vector3f(output["centre"][0], output["centre"][1], output["centre"][2]);
    up = Eigen::Vector3f(output["up"][0], output["up"][1], output["up"][2]);
    
    bkc = Color(output["bkc"][0], output["bkc"][1], output["bkc"][2]);
    ai  = Color(output["ai"][0], output["ai"][1], output["ai"][2]);

    filename = output["filename"];
    size[0] = output["size"][0];
    size[1] = output["size"][1];

    // Back to radians
    fov = output["fov"];
    fov *= (M_PI / 180.0f);

    // Normalise
    lookat.normalize();
    up.normalize();

    // Optional fields

    // Default values
    globalillum = false;
    antialiasing = false;
    twosiderender = true;
    probterminate = 0.0;
    maxbounces = 0;

    //
    if (output.contains("probterminate")) { probterminate = output["probterminate"]; }
    if (output.contains("twosiderender")) { twosiderender = output["twosiderender"]; }
    if (output.contains("globalillum")) { globalillum = output["globalillum"]; }
    if (output.contains("maxbounces")) { maxbounces = output["maxbounces"]; }
    if (output.contains("raysperpixel"))
    {
        nlohmann::json rpp = output["raysperpixel"];
        int size = rpp.size();
        for (int i = 0; i < size; i++) raysperpixel.push_back(rpp[i]);
    }
    if (!globalillum && output.contains("antialiasing"))
    { 
        antialiasing = output["antialiasing"]; 
        maxbounces = 0;
    }
}