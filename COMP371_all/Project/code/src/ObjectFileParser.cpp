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

    std::vector<unsigned int> normal_indices;

    int model_index(-1);

    float max_height(0.0f);
    float min_height(0.0f);

    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;


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
            // For picking
            // +1 to avoid 0 case, background framebuffer is black
            model.selection_code = model_index + 1; 
            iss >> model.part_tag;
            entity.entity_parts.push_back(model);
        }

        switch (lineType) {
            case 'v': {
                // Position data
                if (line[1] == 'n') {
                    // Normal
                    glm::vec3 normal;
                    iss >> normal.x >> normal.y >> normal.z;
                    normals.push_back(normal);

                } else if (line[1] == 't') {
                    // Texture coordinate
                    float textCoord_x;
                    float textCoord_y;

                    iss >> textCoord_x >> textCoord_y;
                    //entity.textCoords.push_back(textCoord_x);
                    //entity.textCoords.push_back(textCoord_y);

                } else {
                    // Position
                    glm::vec3 position;
                    iss >> position.x >> position.y >> position.z;
                    positions.push_back(position);

                    max_height = std::max(max_height, position.y);
                    min_height = std::min(min_height, position.y);
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
                        }
                    }

                    verticesPerFace++;
                }


                if (verticesPerFace < 3) 
                {
                    std::cout << "Line segment encountered" << std::endl;
                    exit(1);
                }

                // Triangulate the face if there are more than 3 vertices
                std::vector<float> &vbo = entity.entity_parts[model_index].vertices;
                for (size_t i = 1; i < verticesPerFace - 1; ++i) 
                {
                    glm::vec3 p1 = positions[faceIndices[0]]; 
                    glm::vec3 n1 = normals[faceIndices[1]];

                    glm::vec3 p2 = positions[faceIndices[2*i]]; 
                    glm::vec3 n2 = normals[faceIndices[2*i+1]];

                    glm::vec3 p3 = positions[faceIndices[2*(i+1)]]; 
                    glm::vec3 n3 = normals[faceIndices[2*(i+1)+1]];

                    vbo.push_back(p1.x);vbo.push_back(p1.y);vbo.push_back(p1.z);
                    vbo.push_back(n1.x);vbo.push_back(n1.y);vbo.push_back(n1.z);

                    vbo.push_back(p2.x);vbo.push_back(p2.y);vbo.push_back(p2.z);
                    vbo.push_back(n2.x);vbo.push_back(n2.y);vbo.push_back(n2.z);

                    vbo.push_back(p3.x);vbo.push_back(p3.y);vbo.push_back(p3.z);
                    vbo.push_back(n3.x);vbo.push_back(n3.y);vbo.push_back(n3.z);

                }

                entity.entity_parts[model_index].face_count += verticesPerFace - 2;
                entity.entity_parts[model_index].number_of_vertices += 3 * (verticesPerFace - 2);
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
