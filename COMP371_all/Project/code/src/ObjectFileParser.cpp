#include "ObjectFileParser.h"
#include "Types.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

bool parseOBJFile(const std::string& filePath, Entity& entity) {
    std::ifstream file(filePath);

    if (!file.is_open()) {
        std::cerr << "Failed to open OBJ file: " << filePath << std::endl;
        return false;
    }

    int model_index(-1);

    float max_height(0.0f);
    float min_height(0.0f);


    std::string line;
    char lineType;
    std::istringstream iss;

    while (std::getline(file, line)) {
        iss.clear();
        iss.str(line);
        iss >> lineType;

        if (lineType == 'o')
        {
            model_index++;
            Model model;
            iss >> model.part_tag;
            entity.entity_parts.push_back(model);
        }

        switch (lineType) {
            case 'v': {
                // Position data
                if (line[1] == 'n') {
                    // Normal
                    float normal_x;
                    float normal_y;
                    float normal_z;
                    iss >> normal_x >> normal_y >> normal_z;
                    //entity.normals.push_back(normal_x);
                    //entity.normals.push_back(normal_y);
                    //entity.normals.push_back(normal_z);

                } else if (line[1] == 't') {
                    // Texture coordinate
                    float textCoord_x;
                    float textCoord_y;

                    iss >> textCoord_x >> textCoord_y;
                    //entity.textCoords.push_back(textCoord_x);
                    //entity.textCoords.push_back(textCoord_y);

                } else {
                    // Position
                    float position_x;
                    float position_y;
                    float position_z;

                    iss >> position_x >> position_y >> position_z;
                    entity.positions.push_back(position_x);
                    entity.positions.push_back(position_y);
                    entity.positions.push_back(position_z);

                    max_height = std::max(max_height, position_y);
                    min_height = std::min(min_height, position_y);
                    //centroid += glm::vec3(position_x, position_y, position_z);
                }
            }
                break;
            case 'f': {
                // Face
                std::vector<unsigned int> faceIndices;
                int verticesPerFace = 0;

                // Process each token in the line
                while (iss >> line) {
                    std::istringstream tokenStream(line);
                    std::string token;

                    // Process each part of the token separated by '/'
                    while (std::getline(tokenStream, token, '/')) {
                        if (!token.empty()) {
                            unsigned int index = std::stoi(token) - 1;
                            faceIndices.push_back(index);
                            // ==================================================================
                            break;// TO GET POSITION INDEX BUFFER ONLY
                            // ==================================================================
                        }
                    }

                    verticesPerFace++;
                }

                // Triangulate the face if there are more than 3 vertices
                if (verticesPerFace > 3) {
                    for (size_t i = 1; i < verticesPerFace - 1; ++i) {
                        entity.entity_parts[model_index].indices.push_back(faceIndices[0]);
                        //model.indices.push_back(faceIndices[1]);
                        //model.indices.push_back(faceIndices[2]);

                        entity.entity_parts[model_index].indices.push_back(faceIndices[i]);
                        //model.indices.push_back(faceIndices[3*i+1]);
                        //model.indices.push_back(faceIndices[3*i+2]);

                        entity.entity_parts[model_index].indices.push_back(faceIndices[i+1]);
                        //model.indices.push_back(faceIndices[3*(i+1)+1]);
                        //model.indices.push_back(faceIndices[3*(i+1)+2]);
                    }
                } else {
                    // Otherwise, just add the indices to the model
                    for (unsigned int index : faceIndices) {
                        entity.entity_parts[model_index].indices.push_back(index);
                    }

                }
                entity.entity_parts[model_index].face_count += verticesPerFace - 2;
                break;
            }
            default:
                // Ignore other lines
                break;
        }
    }

    //centroid /= model.positions.size();
    //model.centroid = centroid;
    float mid_point = (max_height+min_height)/2;
    entity.middle = glm::vec3(0.0f, mid_point, 0.0f);


    file.close();

    return true;
}
