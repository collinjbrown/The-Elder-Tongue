#ifndef RENDERER_H
#define RENDERER_H

#include <array>
#include <vector>
#include <GLFW/glfw3.h>
#include "texture_2D.h"
#include "shader.h"

struct Point
{
    float x;
    float y;
};

struct Color
{
    float r;
    float g;
    float b;
    float a;
};

struct Vertex
{
    Point loc;

    Color color;

    // Texture coordinates
    float sCoord;
    float tCoord;

    float textureIndex;
};

struct Quad
{
    Vertex topRight;
    Vertex bottomRight;
    Vertex bottomLeft;
    Vertex topLeft;
};

class Batch
{
public:
    static constexpr int MAX_QUADS = 10000;

    std::array<Quad, MAX_QUADS> quadBuffer;
    int quadIndex = 0;
};

class Renderer
{
    static constexpr int MAX_TEXTURES_PER_BATCH = 48;

    std::vector<GLuint> textureIDs;
    float whiteTextureIndex;

    GLuint VAO;
    GLuint VBO;

    GLuint whiteTextureID;

    Renderer(GLuint whiteTexture);
    void prepareQuad(Point position, float width, float height, Color color, int textureID);
    void prepareQuad(int batchIndex, Quad& input);
    void prepareDownLine(float x, float y, float height);
    void prepareRightLine(float x, float y, float width);
    void sendToGL();
    void resetBuffers();

private:
    std::vector<Batch> batches;
    Shader shader;

    void flush(const Batch& batch);
};

#endif
