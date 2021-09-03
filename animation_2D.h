#ifndef ANIMATION_2D_H
#define ANIMATION_2D_H

#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>

class Animation2D
{
private:
    Animation2D();

public:
    GLuint              ID;
    unsigned int        width;
    unsigned int        height;
    unsigned int        columns;
    unsigned int        rows;
    float               speed;
    std::vector<int>    rowsToCols;
    GLuint              internalFormat;
    GLuint              imageFormat;
    GLuint              wrapS;
    GLuint              wrapT;
    GLuint              filterMin;
    GLuint              filterMax;

    Animation2D(const char* file, bool alpha, int columns, int rows, float speed, std::vector<int> rowsToColls, int filter = GL_LINEAR);

    void bind() const;
};

#endif