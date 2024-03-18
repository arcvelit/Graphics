#include "AssetsLoader.h"
#include "ObjectFileParser.h"

#include "Types.h"

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