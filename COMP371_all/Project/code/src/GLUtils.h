#pragma once

#include "Types.h"

#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h> 
#include <glm/glm.hpp>  

int glutils_CompileAndLinkShaders();
GLFWwindow* glutils_InitGL(const int SCREEN_WIDTH, const int SCREEN_HEIGHT);
void glutils_RenderModel(Model* model);
void glutils_RenderEntity(Entity* entity);
void glutils_InitializeBuffers(std::vector<Entity>& entities);
void glutils_ResizeWindowCallback(GLFWwindow* window, int x, int y);