#ifndef RENDERER_H
#define RENDERER_H

#include <array>
#include <vector>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "shader.h"
// #include "texture_2D.h"
#include "animation_2D.h"

class PositionComponent;
class ColliderComponent;

struct Vertex
{
    float xCoord;
    float yCoord;

    float rColor;
    float gColor;
    float bColor;
    float aColor;

    float sCoord;
    float tCoord;

    float textureIndex;
    float mapIndex;
};

struct Quad
{
    Vertex topRight;
    Vertex bottomRight;
    Vertex bottomLeft;
    Vertex topLeft;
};

// Store the quads before a draw call
class Batch
{
public:
    static constexpr int MAX_QUADS = 10000;

    // TODO: Look into decoupling # of quads that can be rendered with # of textures that can be rendered in one batch
    std::array<Quad, MAX_QUADS> quadBuffer;
    int quadIndex = 0;
};

// A batch renderer for quads with a color and sprite
class Renderer
{
public:
    // Should be GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS for release, but
    // would need to figure out how to use that value in the fragment shader.
    // NOTE: Fragment shader also has hard-coded value that must match this.
    static constexpr int MAX_TEXTURES_PER_BATCH = 48;

    std::vector<GLuint> textureIDs;
    float whiteTextureIndex;

    GLuint VAO;
    GLuint VBO;

    GLuint whiteTextureID;

    Renderer(GLuint whiteTexture);
    void prepareQuad(PositionComponent* pos, float width, float height, float tWidth, float tHeight, glm::vec4 rgb, int textureID, int mapID, bool tiled, bool flipped);
    void prepareQuad(PositionComponent* pos, ColliderComponent* col, float width, float height, glm::vec4 rgb, int textureID, int mapID);
    void prepareQuad(glm::vec2 topRight, glm::vec2 bottomRight, glm::vec2 bottomLeft, glm::vec2 topLeft, glm::vec4 rgb, int textureID, int mapID);
    void prepareQuad(glm::vec2 position, float width, float height, glm::vec4 rgb, int textureID, int mapID); // Specify texture ID rather than index?
    // NOTE: Directly sending a texture index rather than ID can result in the wrong texture being drawn (due to being in the wrong batch)
    void prepareQuad(PositionComponent* pos, float width, float height, glm::vec4 rgb, int animID, int mapID, int cellX, int cellY, int cols, int rows, bool flipped);
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
