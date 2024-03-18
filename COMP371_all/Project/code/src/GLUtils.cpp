#include "GLUtils.h"
#include "AssetsLoader.h"
#include "Types.h"

#include <iostream>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h> 
#include <glm/glm.hpp>  

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Paths
#define vertex_shader_path "../assets/Shaders/Vertex.glsl"
#define fragment_shader_path "../assets/Shaders/Fragment.glsl"



int glutils_CompileAndLinkShaders()
{
    // Find shaders
    std::string vertex_shader_str = readFile(vertex_shader_path);
    std::string fragment_shader_str = readFile(fragment_shader_path);

    const char* vertex_shader_source = vertex_shader_str.c_str();
    const char* fragment_shader_source = fragment_shader_str.c_str();

    int success;

    // Vertex Shader
    int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertex_shader_source, NULL);
    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED" << std::endl;
        exit(1);
    }

    // Fragment shader
    int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragment_shader_source, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED" << std::endl;
        exit(1);
    }

    // link shaders
    int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED" << std::endl;
        exit(1);
    }

    return shaderProgram;
}

GLFWwindow* glutils_InitGL(const int SCREEN_WIDTH, const int SCREEN_HEIGHT)
{
    // Initialize GLFW and OpenGL version
    glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); 
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); 
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // Create Window and rendering context using GLFW
    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Guitar Showcase", NULL, NULL);
    if (window == NULL)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(1);
    }
    glfwMakeContextCurrent(window);
    

    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to create GLEW" << std::endl;
        glfwTerminate();
        exit(1);
    }

    return window;
}

void glutils_InitializeBuffers(std::vector<Entity>& entities)
{
    for (Entity& entity : entities)
    {
        for (Model& m : entity.entity_parts)
        {
            glGenVertexArrays(1, &m.VAO);
            glGenBuffers(1, &m.VBO);
            //glGenBuffers(1, &m.NBO);
            //glGenBuffers(1, &m.TVBO);
            glGenBuffers(1, &m.IBO);
            
            // Bind bind the position VBO
            glBindVertexArray(m.VAO);

            glBindBuffer(GL_ARRAY_BUFFER, m.VBO);
            glBufferData(GL_ARRAY_BUFFER, entity.positions.size() * sizeof(float), &entity.positions[0], GL_STATIC_DRAW);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);
        /*
            glGenBuffers(1, &model.NBO);
            glBindBuffer(GL_ARRAY_BUFFER, m.NBO);
            glBufferData(GL_ARRAY_BUFFER, entity.normals.size() * sizeof(float), &entity.normals[0], GL_STATIC_DRAW);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(1);
        */
            // glBindBuffer(GL_ARRAY_BUFFER, m.TBO);
            // glBufferData(GL_ARRAY_BUFFER, entity.textCoords.size() * sizeof(float), &entity.textCoords[0], GL_STATIC_DRAW);
            // glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
            // glEnableVertexAttribArray(2);

            // Generate and bind IBO
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m.IBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, m.indices.size() * sizeof(unsigned int), &m.indices[0], GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glBindVertexArray(0);
        }
    }
}

void glutils_RenderModel(Model* model)
{
    glBindVertexArray(model->VAO);
    glDrawElements(GL_TRIANGLES, model->face_count * 3, GL_UNSIGNED_INT, (void*)0);
    glBindVertexArray(0);
}

void glutils_RenderEntity(Entity* entity)
{
    for (Model& model : entity->entity_parts)
    {
        glutils_RenderModel(&model);
    }
}

void glutils_ResizeWindowCallback(GLFWwindow* window, int x, int y)
{
    Camera* camera = static_cast<Camera*>(glfwGetWindowUserPointer(window));
    camera->aspectRatio = (float) x / y;
    glViewport(0, 0, x, y);
}