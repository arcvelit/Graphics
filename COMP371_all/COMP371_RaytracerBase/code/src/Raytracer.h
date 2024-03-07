#ifndef RAYTRACER_H
#define RAYTRACER_H

#include "SceneInfo.h"

#include "json.hpp"


class RayTracer 
{
    public: 
    SceneInfo scene;

    // Constructor
    RayTracer(nlohmann::json data);

    // Methods
    void run();

};

#endif
