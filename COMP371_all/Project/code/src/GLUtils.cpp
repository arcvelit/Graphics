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


int glutils_CompileAndLinkShaders(std::string vertex_shader_path, std::string fragment_shader_path)
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
        GLint logLength;
        glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &logLength);
        std::vector<GLchar> log(logLength);
        glGetShaderInfoLog(vertexShader, logLength, NULL, log.data());

        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED" << std::endl;
        std::cerr << log.data() << std::endl;
        exit(1);
    }

    // Fragment shader
    int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragment_shader_source, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        GLint logLength;
        glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &logLength);
        std::vector<GLchar> log(logLength);
        glGetShaderInfoLog(fragmentShader, logLength, NULL, log.data());

        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED" << std::endl;
        std::cerr << log.data() << std::endl;
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
            
            glBindVertexArray(m.VAO);

            glBindBuffer(GL_ARRAY_BUFFER, m.VBO);
            glBufferData(GL_ARRAY_BUFFER, m.vertices.size() * sizeof(float), &m.vertices[0], GL_STATIC_DRAW);
            
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3*sizeof(float)));

            glEnableVertexAttribArray(0);
            glEnableVertexAttribArray(1);

            glBindVertexArray(0);
        }
    }
}

void glutils_RenderModel(Model* model)
{
    glBindVertexArray(model->VAO);
    glDrawArrays(GL_TRIANGLES, 0, model->number_of_vertices);
    glBindVertexArray(0);
}

void glutils_RenderEntity(Entity* entity, unsigned int shader)
{
    for (Model& model : entity->entity_parts)
    {
        glutils_SendModelUniforms(shader, model);
        glutils_RenderModel(&model);
    }
}

void glutils_ResizeWindowCallback(GLFWwindow* window, int x, int y)
{
    Camera* camera = static_cast<Camera*>(glfwGetWindowUserPointer(window));
    camera->screen_width = x;
    camera->screen_height = y;
    camera->aspectRatio = (float) x / y;
    glViewport(0, 0, x, y);
}

void glutils_SendLightUniforms(unsigned int entityShaderProgram, Light &light)
{
    glUniform3fv(glGetUniformLocation(entityShaderProgram, "LightPosition_worldspace"), 1, &light.position[0]);
}

void glutils_SendCameraUniforms(unsigned int entityShaderProgram, Camera &camera)
{
    glUniform3fv(glGetUniformLocation(entityShaderProgram, "CameraPosition_worldspace"), 1, glm::value_ptr(camera.position));
}

void glutils_SendModelUniforms(unsigned int entityShaderProgram, Model &model)
{
    glUniform1f(glGetUniformLocation(entityShaderProgram, "ka"), model.ka);
    glUniform1f(glGetUniformLocation(entityShaderProgram, "kd"), model.kd);
    glUniform1f(glGetUniformLocation(entityShaderProgram, "ks"), model.ks);

    glUniform1f(glGetUniformLocation(entityShaderProgram, "pc"), model.pc);

    glUniform3fv(glGetUniformLocation(entityShaderProgram, "ac"), 1, &model.ac[0]);
    glUniform3fv(glGetUniformLocation(entityShaderProgram, "dc"), 1, &model.dc[0]);

    glUniform1i(glGetUniformLocation(entityShaderProgram, "picked"), model.picked);

    glUniform1i(glGetUniformLocation(entityShaderProgram, "selection_code"), model.selection_code);
}

int glutils_FindSelectedModel(std::vector<Model> &entity_parts, unsigned char* fragment)
{
    if (fragment[0] == fragment[1] && fragment[1] == fragment[2])
    for (int i = 0; i < entity_parts.size(); i++)
    {
        if (entity_parts[i].selection_code == fragment[0])
        {
            return i;
        }
    }

    return -1;
}