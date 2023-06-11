#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <list>
#include <memory>

#include "texture.hpp"
#include "functions.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

float vertices[] = {
    // positions          // colors           // texture coords
     0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
     0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
    -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
    -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
};

unsigned int indices[] = {
    0, 1, 3,   // first triangle
    1, 2, 3    // second triangle
};  

int main(void)
{ 
    GLFWwindow* window;

    if (!glfwInit()) return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(640, 480, "Shaders", NULL, NULL);

    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) std::cout << "Can't load GLEW\n";
    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);  

    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    unsigned int EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    Shader myShader("shaders/3.3.shader.vshad", "shaders/3.3.shader.fshad");

    int width1, height1, nrChannels1, width2, height2, nrChannels2;

    bool linearFiltering = false;
    
    GLuint texture1obj;
    GLuint texture2obj;

    glGenTextures(1, &texture1obj);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1obj);

    Texture texture1("textures/container.jpg", linearFiltering, GL_RGB, &texture1obj);    

    glGenTextures(1, &texture2obj);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2obj);

    Texture texture2("textures/awesomeface.png", linearFiltering, GL_RGBA, &texture2obj);

    float textureMixFactor = 0.2f;
    std::map<int, bool> keyPressedTracker = {{GLFW_KEY_UP, false}, {GLFW_KEY_DOWN, false}};

    myShader.UseProgram();
    myShader.SetUniformInt("myTexture1", 0);
    myShader.SetUniformInt("myTexture2", 1);
    myShader.SetUniformFloat("textureMixFactor", textureMixFactor);
    
    while (!glfwWindowShouldClose(window))
    {
        ProcessInput(window, textureMixFactor, &myShader, keyPressedTracker, linearFiltering);
        
        glClearColor (0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1obj);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2obj);
               
        myShader.UseProgram();

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}