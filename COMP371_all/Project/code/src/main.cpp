#include "GLUtils.h"
#include "ModelUtils.h"
#include "AssetsLoader.h"
#include "Types.h"


#define GLEW_STATIC 1   // This allows linking with Static Library on Windows, without DLL

#include <iostream>
#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h> 
#include <glm/glm.hpp>  

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <chrono>
#include <cmath>

#define SCREEN_WIDTH   1000
#define SCREEN_HEIGHT  600


int main() 
{
    // Summon guitars
    std::vector<Entity> entities;
    loadProjectModels("../assets/Guitars/", entities);
    if (entities.size() == 0)
    {
        std::cout << "OBJ files not found" << std::endl;
        exit(1);
    }

    // Filming and stuff probably
    Camera camera;
    camera.aspectRatio  = (float) SCREEN_WIDTH / SCREEN_HEIGHT;
    camera.nearPlane    = 0.1f;
    camera.farPlane     = 100.0f;
    camera.fov          = 45.0f;
    camera.moveSpeed    = 0.1f;
    camera.rotateSpeed  = 0.7f;
    camera.modelAngle   = 0.0;

    modelutils_SetCameraToStarting(camera);
    
    // Init everything and stuff and yeah mhmh
    GLFWwindow* window = glutils_InitGL(SCREEN_WIDTH, SCREEN_HEIGHT);
    // Callbacks
    glfwSetWindowSizeCallback(window, glutils_ResizeWindowCallback);
    glfwSetWindowUserPointer(window, &camera);


    // Compile everything
    int shaderProgram  = glutils_CompileAndLinkShaders();

    
    // Set GL Model buffers
    const size_t NUMBER_OF_ENTITIES(entities.size());
    size_t activeEntityIndex(0);
    Entity* entity = &entities[0];
    std::cout << "Showing [" << entity->name << "]" << std::endl;

    // Avoid switching too fast
    const int SWITCH_COOLDOWN_DURATION = 100; // Half a second
    auto lastModelChangeTime = std::chrono::steady_clock::now();


    glutils_InitializeBuffers(entities);

    // Background
    glClearColor(0.5f, 0.5f, 1.0f, 1.0f);
    // Viewport frame
    glViewport(0,0,SCREEN_WIDTH, SCREEN_HEIGHT);
    

    while(!glfwWindowShouldClose(window))
    {
        // Specifics
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        // Handle events
        glfwPollEvents();

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            camera.position.y += camera.moveSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            camera.position.y -= camera.moveSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            camera.position.x -= camera.moveSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            camera.position.x += camera.moveSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
            camera.modelAngle -= camera.rotateSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
            camera.modelAngle += camera.rotateSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS) {
            // Avoid loading each millisecond
            auto currentTime = std::chrono::steady_clock::now();
            auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastModelChangeTime).count();
            
            if (elapsedTime > SWITCH_COOLDOWN_DURATION)
            {
                // Change model
                activeEntityIndex = (activeEntityIndex + 1) % NUMBER_OF_ENTITIES;
                entity = &entities[activeEntityIndex];
                std::cout << "Showing [" << entity->name << "]" << std::endl;
                lastModelChangeTime = currentTime;

                // Clean up camera
                modelutils_SetCameraToStarting(camera);
            }
        }

        // Update camera view
        modelutils_UpdateCameraLookat(camera);

        // Shaders
        glUseProgram(shaderProgram);
        
        // ModelViewProjection is passed to the vertex shader
        glm::mat4 ModelViewProjection = 
        glm::perspective(glm::radians(camera.fov), camera.aspectRatio, camera.nearPlane, camera.farPlane) *
        glm::lookAt(camera.position, camera.center, camera.up) *
        glm::translate(glm::mat4(1.0f), -entity->middle) * 
        glm::rotate(glm::mat4(1.0f), camera.modelAngle, glm::vec3(0.0f, 1.0f, 0.0f));

        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "MVP"), 1, GL_FALSE, glm::value_ptr(ModelViewProjection));


        glutils_RenderEntity(entity);


        glfwSwapBuffers(window);        
    }

    glfwTerminate();
    
    return 0;
    
}