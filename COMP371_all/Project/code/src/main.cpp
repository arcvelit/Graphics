#include "GLUtils.h"
#include "ModelUtils.h"
#include "AssetsLoader.h"
#include "Types.h"

#define GLEW_STATIC 1

#include <iostream>
#include <string>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h> 
#include <glm/glm.hpp>  

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "glm/ext.hpp" 



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
    // Translate entities to center of gravity
    modelutils_CenterEntitiesToMiddle(entities);


    // Tell them how they look
    loadAssetProperties("../assets/Guitars/Properties.json", entities);

    // Camera setup
    Camera camera;
    {
        camera.aspectRatio  = (float) SCREEN_WIDTH / SCREEN_HEIGHT;
        camera.nearPlane    = 0.1f;
        camera.farPlane     = 100.0f;
        camera.fov          = 45.0f;
        camera.moveSpeed    = 0.3f;
        camera.rotateSpeed  = 1.0f;
        camera.modelAngle   = 0.0f;
        camera.screen_width = SCREEN_WIDTH;
        camera.screen_height = SCREEN_HEIGHT;
    }

    // Let there be light
    Light light;
    light.position = glm::vec3(0.0,0.0, -1.5);

    modelutils_SetCameraToStarting(camera);

    // Window :)
    GLFWwindow* window = glutils_InitGL(SCREEN_WIDTH, SCREEN_HEIGHT);

    // Callbacks
    glfwSetWindowSizeCallback(window, glutils_ResizeWindowCallback);
    glfwSetWindowUserPointer(window, &camera);

    // Entity shaders
    const std::string vertex_shader_path = "../assets/Shaders/Phong.vertex.glsl";
    const std::string fragment_shader_path = "../assets/Shaders/Phong.fragment.glsl";
    unsigned int entityShaderProgram = glutils_CompileAndLinkShaders(vertex_shader_path, fragment_shader_path);

    // Picking shaders
    const std::string picking_vertex_path = "../assets/Shaders/Picking.vertex.glsl";
    const std::string picking_fragment_path = "../assets/Shaders/Picking.fragment.glsl";
    unsigned int pickingShaderProgram = glutils_CompileAndLinkShaders(picking_vertex_path, picking_fragment_path);


    // Set GL Model buffers
    const size_t NUMBER_OF_ENTITIES(entities.size());
    size_t activeEntityIndex(0);
    size_t activeModelIndex(0);
    bool selecting(false);
    Entity* entity = &entities[activeEntityIndex];
    Model* model = nullptr;

    // Avoid switching entities too fast
    const int SWITCH_COOLDOWN_DURATION = 200;
    auto lastModelChangeTime = std::chrono::steady_clock::now();
    auto lastViewChangeTime = std::chrono::steady_clock::now();

    // Init model buffers
    glutils_InitializeBuffers(entities);

    glClearColor(0.5f, 0.5f, 1.0f, 1.0f);
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    
    // GL Settings
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);  
    glCullFace(GL_BACK);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");


    float delta_time = 0.0f;
    float last_frame = 0.0f;

    while(!glfwWindowShouldClose(window))
    {
        // Deltas
        float current_time = glfwGetTime();
        delta_time = current_time - last_frame;
        last_frame = current_time;

        // Clearing screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Handle events
        glfwPollEvents();

        if (camera.closeup) {
            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
                camera.position.y += delta_time*camera.moveSpeed;
                camera.position.y = (camera.position.y > 0.4f) ? 0.4 : camera.position.y;
            }
            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
                camera.position.y -= delta_time*camera.moveSpeed;
                camera.position.y = (camera.position.y < -0.4f) ? -0.4 : camera.position.y;
            }
        }
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
            camera.modelAngle -= delta_time*camera.rotateSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
            camera.modelAngle += delta_time*camera.rotateSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
            modelutils_SetCameraToStarting(camera);
        }
        if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) {

            auto currentTime = std::chrono::steady_clock::now();
            auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastViewChangeTime).count();

            if (elapsedTime > SWITCH_COOLDOWN_DURATION)
            {
                if (camera.closeup) modelutils_SetCameraToZoomOut(camera);
                else modelutils_SetCameraToCloseUp(camera);
                lastViewChangeTime = currentTime;
            }
        }
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            selecting = false;
            if (model != nullptr)
                model->picked = 0;
            activeModelIndex = -1;
        }
        if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
        {
            loadAssetProperties("../assets/Guitars/Properties.json", entities);
        }

        // Update view
        modelutils_UpdateCameraLookat(camera);
        //light.position = camera.position;


        // ENTITY DRAWING =================================================
        
        // T Matrices are sent to the shaders
        glm::mat4 ProjectionT = glm::perspective(glm::radians(camera.fov), camera.aspectRatio, camera.nearPlane, camera.farPlane);
        glm::mat4 ViewT = glm::lookAt(camera.position, camera.center, camera.up);

        glm::mat4 ModelT = glm::rotate(glm::mat4(1.0f), camera.modelAngle, glm::vec3(0.0f, 1.0f, 0.0f));
        //glm::scale(glm::mat4(1.0f), 2 * glm::vec3(1.0f, 1.0f, 1.0f));

        glm::mat4 ModelViewT = ViewT * ModelT;
        glm::mat4 ModelViewProjectionT = ProjectionT * ModelViewT;

        // Camera and light uniforms
        glUseProgram(pickingShaderProgram);
        glutils_SendCameraUniforms(pickingShaderProgram, camera);
        glutils_SendLightUniforms(pickingShaderProgram, light);
        glUniformMatrix4fv(glGetUniformLocation(pickingShaderProgram, "ModelViewProjection"), 1, GL_FALSE, glm::value_ptr(ModelViewProjectionT));

        glUseProgram(entityShaderProgram);
        glutils_SendCameraUniforms(entityShaderProgram, camera);
        glutils_SendLightUniforms(entityShaderProgram, light);
        glUniformMatrix4fv(glGetUniformLocation(entityShaderProgram, "ModelViewProjection"), 1, GL_FALSE, glm::value_ptr(ModelViewProjectionT));
        //glUniformMatrix4fv(glGetUniformLocation(entityShaderProgram, "ModelView"), 1, GL_FALSE, glm::value_ptr(ModelViewT));
        //glUniformMatrix4fv(glGetUniformLocation(entityShaderProgram, "View"), 1, GL_FALSE, glm::value_ptr(ViewT));
        glUniformMatrix4fv(glGetUniformLocation(entityShaderProgram, "Model"), 1, GL_FALSE, glm::value_ptr(ModelT));


        // ================================================================

        // Get selected model 'just in time'
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
        {
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);

            glUseProgram(pickingShaderProgram);
            glutils_RenderEntity(entity, pickingShaderProgram);

            unsigned char pixelColor[4];
            glReadPixels(xpos, camera.screen_height - ypos, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixelColor);

            int model_index = glutils_FindSelectedModel(entity->entity_parts, pixelColor);
            if (model_index != -1)
            {
                if (model != nullptr)
                    model->picked = 0;
                
                selecting = true;
                model = &entity->entity_parts[model_index];
                model->picked = 1;
                activeModelIndex = model_index;
            }

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        }

        glUseProgram(entityShaderProgram);
        glutils_RenderEntity(entity, entityShaderProgram);

        // GUI
        {        
            constexpr ImGuiWindowFlags imgui_default_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;

            const ImVec2 entity_box_size = ImVec2(260, 560);
            const ImVec2 entity_box_pos = ImVec2(8, 10);

            const ImVec2 model_box_size = ImVec2(entity_box_size.x, entity_box_size.y / 2);
            const ImVec2 model_box_pos = ImVec2(camera.screen_width - model_box_size.x - entity_box_pos.x , entity_box_pos.y);


            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            // Conditional parameters
            int entity_change(0);
            int model_change(0);

            // START - Entity box
            ImGui::Begin(entity->name.c_str(), nullptr, imgui_default_flags);
            ImGui::SetWindowPos(entity_box_pos);
            ImGui::SetWindowSize(entity_box_size);
            ImGui::TextWrapped("%s", entity->description.c_str());

            ImGui::SetCursorPosY(entity_box_size.y - 2 * ImGui::GetTextLineHeightWithSpacing());
            if (ImGui::Button("< Back")) { entity_change = -1; } 

            ImGui::SameLine();
            ImGui::SetCursorPosX(ImGui::GetWindowSize().x - 2 * ImGui::GetStyle().ItemSpacing.x - ImGui::CalcTextSize("Next >").x);
            if (ImGui::Button("Next >")) { entity_change = 1; }

            // END - Entity box
            ImGui::End();

            if(selecting && model != nullptr)
            {
                // START - Model box
                ImGui::Begin(model->name.c_str(), nullptr, imgui_default_flags);
                ImGui::TextWrapped("%s", model->description.c_str());
                ImGui::SetWindowPos(model_box_pos);
                ImGui::SetWindowSize(model_box_size);

                ImGui::SetCursorPosY(model_box_size.y - 2 * ImGui::GetTextLineHeightWithSpacing());
                if (ImGui::Button("< Back")) { model_change = -1; } 

                ImGui::SameLine();
                ImGui::SetCursorPosX(ImGui::GetWindowSize().x - 2 * ImGui::GetStyle().ItemSpacing.x - ImGui::CalcTextSize("Next >").x);
                if (ImGui::Button("Next >")) { model_change = 1; }

                // END - Model box
                ImGui::End();
            }


            if (entity_change != 0)
            {
                // Change entity
                if (selecting && model != nullptr) model->picked = 0;
                activeEntityIndex = (activeEntityIndex + NUMBER_OF_ENTITIES + entity_change) % NUMBER_OF_ENTITIES;
                selecting = false;
                entity = &entities[activeEntityIndex]; 

                // Replace camera
                modelutils_SetCameraToStarting(camera);
            }
            if (model_change != 0)
            { 
                if (model != nullptr) model->picked = 0;
                const size_t NUMBER_OF_MODELS = entity->entity_parts.size(); 
                activeModelIndex = (activeModelIndex + NUMBER_OF_MODELS + model_change) % NUMBER_OF_MODELS;
                model = &entity->entity_parts[activeModelIndex];
                model->picked = 1;
            }

        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);        
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
    
}
