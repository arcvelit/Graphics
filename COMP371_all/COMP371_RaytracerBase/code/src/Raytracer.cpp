#include "Raytracer.h"
#include "json.hpp"
#include "simpleppm.h"

#include "SceneInfo.h"
#include "OutputInfo.h"
#include "Types.h"

#include "RenderTechniques.h"

#include <iostream>
#include <string>


RayTracer::RayTracer(nlohmann::json data) : scene(data) {}

void RayTracer::run() {
    std::cout << "Running - run()" << std::endl;

    for (OutputInfo output : scene.outputs)
    {
        const int CHANNELS = 3;
        const int X_DIMENSION = output.size[0];
        const int Y_DIMENSION = output.size[1];
        const std::string FILENAME = output.filename;

        Buffer buffer(CHANNELS * X_DIMENSION * Y_DIMENSION);

        // Ignore area light 
        if (output.antialiasing) for (Light light : scene.lights)
        {
            if (light.type == AREA) { output.antialiasing = false; break; }
        }


        std::string renderTechnique = output.antialiasing ? "antialiasing" : (output.globalillum ? "global illumination" : "local illumination");
        std::cout << "Rendering [" << output.filename << "] with [" << renderTechnique << "]" << std::endl;

        if (output.globalillum || output.antialiasing)
        {
            renderSceneGlobal(buffer, scene, output);
        }
        else
        {
            renderSceneLocal(buffer, scene, output);
        }


        save_ppm(FILENAME, buffer, X_DIMENSION, Y_DIMENSION);
    }

    
    std::cout << "Done - run()" << std::endl;
}