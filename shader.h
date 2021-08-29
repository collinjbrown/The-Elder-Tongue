#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <GLFW/glfw3.h>

class Shader
{
public:
    GLuint ID;

    Shader(const char* vertexPath, const char* fragmentPath);
    void use();
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
};

#endif
