#ifndef SCENEINFO_H
#define SCENEINFO_H

#include "Geometry.h"
#include "Light.h"
#include "OutputInfo.h"


#include "json.hpp"

#include <vector>


class SceneInfo 
{
    public:
    std::vector<Geometry> geometries;
    std::vector<OutputInfo> outputs;
    std::vector<Light> lights;

    SceneInfo(nlohmann::json &data);

};

#endif