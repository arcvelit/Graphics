#include "SceneInfo.h"
#include "Geometry.h"
#include "OutputInfo.h"
#include "json.hpp"

#include <iostream>

SceneInfo::SceneInfo(nlohmann::json &data)
{
    // Stack on geometries
    std::cout << "Geometries: " << data["geometry"].size() << "\n";
    for (const auto &geo_element : data["geometry"])
    {
        Geometry geo = Geometry(geo_element);
        if (geo.visible)
        {
            geometries.push_back(geo);
        }
    }

    // Stack on lights
    std::cout << "Lights: " << data["light"].size() << "\n";
    for (const auto &light_element : data["light"])
    {
        Light light = Light(light_element);
        if (light.use)
        {
            lights.push_back(light);
        }
    }

    // Output files
    std::cout << "Outputs: " << data["output"].size() << "\n";
    for (const auto &output_element : data["output"])
    {
        OutputInfo out = OutputInfo(output_element);
        outputs.push_back(out);
    }

    // Mandatory check
    if (geometries.empty() /* || lights.empty() || outputs.empty() */)
    {
        std::cout << "ERROR: Empty geometry, light or output in JSON file\n";
        std::cout << "Exiting program..." << std::endl;
        exit(1);
    }

    

}