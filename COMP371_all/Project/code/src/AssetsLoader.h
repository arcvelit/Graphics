#pragma once

#include "Types.h"
#include <string>
#include <vector>

void loadProjectModels(const std::string& directoryPath, std::vector<Entity> &entity_array);
void loadAssetProperties(const char* json_file_path, std::vector<Entity> &entity_array);
std::string readFile(const std::string& filePath);