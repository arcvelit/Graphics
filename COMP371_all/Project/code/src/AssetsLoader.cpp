#include "AssetsLoader.h"
#include "ObjectFileParser.h"

#include "Types.h"

#include "json.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <dirent.h>
#include <vector>


void loadProjectModels(const std::string& directory_path, std::vector<Entity> &entity_array) 
{
    DIR *dir;
    struct dirent *entry;

    if ((dir = opendir(directory_path.c_str())) != nullptr) {
        while ((entry = readdir(dir)) != nullptr) {
            std::string filename = entry->d_name;
            if (filename.size() > 4 && filename.substr(filename.size() - 4) == ".obj") {
                Entity entity;
                entity.name = filename.substr(0, filename.size() - 4);
                if (parseOBJFile(directory_path + filename, entity)) {
                    entity_array.push_back(entity);
                    std::cout << "Loaded entity: " << filename << std::endl;
                } else {
                    std::cerr << "Failed to load entity: " << filename << std::endl;
                }
            }
        }
        closedir(dir);
    } else {
        std::cerr << "Error opening directory: " << directory_path << std::endl;
    }
}

void loadAssetProperties(const char* json_file_path, std::vector<Entity> &entity_array)
{    
    std::ifstream t(json_file_path);
    if(!t)
    {
        std::cout <<"File "<< json_file_path <<" does not exist!"<< std::endl;
        exit(1);
    }
        
    std::stringstream buffer;
    buffer << t.rdbuf();
        
    nlohmann::json props = nlohmann::json::parse(buffer.str());

    for(Entity &entity : entity_array)
    {
        // Get entity from json
        // matching entity.name
        auto guitar = props[entity.name];
        if (guitar.contains("Description")) entity.description = guitar["Description"];
        else entity.description = "???";

        for(Model &model : entity.entity_parts)
        {
            // Get model from json
            // matching model.part_tag
            auto part = guitar[model.part_tag];

            if (part.contains("name")) model.name = part["name"];
            else model.name = "???";
            if (part.contains("ka")) model.ka = part["ka"];
            if (part.contains("kd")) model.kd = part["kd"];
            if (part.contains("ks")) model.ks = part["ks"];
            if (part.contains("pc")) model.pc = part["pc"];
            if (part.contains("description")) model.description = part["description"];
            else model.description = "???";
            if (part.contains("ac")) model.ac = glm::vec3(part["ac"][0], part["ac"][1], part["ac"][2]);
            if (part.contains("dc")) model.dc = glm::vec3(part["dc"][0], part["dc"][1], part["dc"][2]);
        }
    }


    std::cout << "Loaded entity properties" << std::endl;
}

std::string readFile(const std::string& filePath) {
    std::ifstream file(filePath);

    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filePath << std::endl;
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();

    return buffer.str();
}