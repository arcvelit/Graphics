#pragma once

#include "Types.h"

#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h> 
#include <glm/glm.hpp>  

int glutils_CompileAndLinkShaders(std::string vertex_shader_path, std::string fragment_shader_path);
GLFWwindow* glutils_InitGL(const int SCREEN_WIDTH, const int SCREEN_HEIGHT);
void glutils_RenderModel(Model* model);
void glutils_RenderEntity(Entity* entity, unsigned int shader);
void glutils_InitializeBuffers(std::vector<Entity>& entities);
void glutils_SendLightUniforms(unsigned int entityShaderProgram, Light &light);
void glutils_SendCameraUniforms(unsigned int entityShaderProgram, Camera &camera);
void glutils_SendModelUniforms(unsigned int entityShaderProgram, Model &model);
int glutils_FindSelectedModel(std::vector<Model> &entity_parts, unsigned char* fragment);
void glutils_ResizeWindowCallback(GLFWwindow* window, int x, int y);