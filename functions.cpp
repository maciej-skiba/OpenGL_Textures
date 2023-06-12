#include "functions.hpp" 
#include "stb_image.h"
#include "texture.hpp"

void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// Change fragment shader's color by time
void DynamicFragmentShader(unsigned int &program)
{
    float divider = 3.0f;

    float timeValue = glfwGetTime();
    float redValue = fabs((float)(sin(timeValue)) / divider);
    float greenValue = fabs((float)(sin(timeValue)) / divider);
    float blueValue = fabs((float)(sin(timeValue)) / divider);

    int vertexColorLocation = glGetUniformLocation(program, "colorUniform");
    glUniform3f(vertexColorLocation, redValue, greenValue, blueValue);
}

void SetupTexture(const char* path, int &width, int &height, int &nrChannels, int colorType, bool linearFiltering)
{
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    if (linearFiltering)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }

    // load and generate the texture
    unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, colorType, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }

    stbi_image_free(data);
}

// Unity's Input.GetKeyDown() implementation
void ProcessInput(GLFWwindow *window, float &textureMixFactor, Shader* shader, std::map<int, bool> &keyPressedTrackingMap, bool &linearFiltering)
{
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        if (textureMixFactor < 1.0f && !keyPressedTrackingMap[GLFW_KEY_UP])
        {
            textureMixFactor += 0.1f;
            if (textureMixFactor > 1.0f) textureMixFactor = 1.0f;
            shader->SetUniformFloat("textureMixFactor", textureMixFactor);
            std::cout << "Texture mix factor: " << textureMixFactor << std::endl;
            keyPressedTrackingMap[GLFW_KEY_UP] = true;
        }
    }
    else (keyPressedTrackingMap[GLFW_KEY_UP] = false);

    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        if (textureMixFactor > 0.0f && !keyPressedTrackingMap[GLFW_KEY_DOWN])
        {
            textureMixFactor -= 0.1f;
            if (textureMixFactor < 0.0f) textureMixFactor = 0.0f;
            shader->SetUniformFloat("textureMixFactor", textureMixFactor);
            std::cout << "Texture mix factor: " << textureMixFactor << std::endl;
            keyPressedTrackingMap[GLFW_KEY_DOWN] = true;
        } 
    }
    else (keyPressedTrackingMap[GLFW_KEY_DOWN] = false);

    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
        linearFiltering = true;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
        linearFiltering = false;
    }
}

void Texture::ReloadTexture(bool linFiltering)
{
    linearFiltering = linFiltering;
    SetupTexture(_path, _width, _height, _nrChannels, _colorType, linearFiltering);
}