#include "renderer.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <cstring>
#include <cmath>
#include <algorithm>
#include "check_error.h"
#include "game.h"

Renderer::Renderer(GLuint whiteTexture) : batches(1), shader("assets/shaders/quad.vert", "assets/shaders/quad.frag"), whiteTextureID(whiteTexture)
{
    GLuint quadIBO;
    glCheckError();
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glGenBuffers(1, &quadIBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadIBO);

    glCheckError();

    glBufferData(GL_ARRAY_BUFFER, Batch::MAX_QUADS * sizeof(Quad), nullptr, GL_DYNAMIC_DRAW);

    glCheckError();

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, xCoord));
    glEnableVertexAttribArray(0);
    // rgba values for color
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, rColor));
    glEnableVertexAttribArray(1);
    // s and t coordinates for texture
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, sCoord));
    glEnableVertexAttribArray(2);
    // Texture index
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, textureIndex));
    glEnableVertexAttribArray(3);

    glCheckError();

    // unsigned int quadVertices[] = {
    //     0, 1, 3,
    //     1, 2, 3,
    //     4, 5, 7,
    //     5, 6, 7
    // };
    unsigned int quadIndices[Batch::MAX_QUADS * 6];
    for (int i = 0; i < Batch::MAX_QUADS; i++)
    {
        const int rightOffset = 4 * i;
        const int leftOffset = 6 * i;

        quadIndices[leftOffset + 0] = rightOffset + 0;
        quadIndices[leftOffset + 1] = rightOffset + 1;
        quadIndices[leftOffset + 2] = rightOffset + 3;

        quadIndices[leftOffset + 3] = rightOffset + 1;
        quadIndices[leftOffset + 4] = rightOffset + 2;
        quadIndices[leftOffset + 5] = rightOffset + 3;
    }
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glUseProgram(shader.ID);
    GLint location = glGetUniformLocation(shader.ID, "batchQuadTextures");
    int samplers[MAX_TEXTURES_PER_BATCH];
    for (int i = 0; i < MAX_TEXTURES_PER_BATCH; i++)
    {
        samplers[i] = i;
    }
    glUniform1iv(location, MAX_TEXTURES_PER_BATCH, samplers);

    // Use white texture as the first texture
    // -----------------------------------------
    this->textureIDs.push_back(whiteTexture);
    whiteTextureIndex = 0.0f;
}

void Renderer::prepareQuad(glm::vec2 position, float width, float height,
    glm::vec4 rgb, int textureID)
{
    // Figure out which batch should be written to
    // -------------------------------------------
    auto result = std::find(textureIDs.begin(), textureIDs.end(), textureID);
    int location;
    if (result == textureIDs.end())
    {
        location = textureIDs.size();
        textureIDs.push_back(textureID);
    }
    else
    {
        location = result - textureIDs.begin();
    }

    int batchIndex = location / MAX_TEXTURES_PER_BATCH;
    float glTextureIndex = location % MAX_TEXTURES_PER_BATCH;
    Batch& batch = batches[batchIndex];

    // Initialize the data for the quad
    // --------------------------------
    Quad& quad = batch.quadBuffer[batch.quadIndex];
    batch.quadIndex++;

    const float rightX = position.x + (width / 2.0f);
    const float leftX = position.x - (width / 2.0f);
    const float topY = position.y + (height / 2.0f);
    const float bottomY = position.y - (height / 2.0f);

    const float r = rgb.r;
    const float g = rgb.g;
    const float b = rgb.b;
    const float a = rgb.a;

    quad.topRight = { rightX, topY,      r, g, b, a,   1.0, 1.0,    glTextureIndex };
    quad.bottomRight = { rightX, bottomY,   r, g, b, a,   1.0, 0.0,    glTextureIndex };
    quad.bottomLeft = { leftX,  bottomY,   r, g, b, a,   0.0, 0.0,    glTextureIndex };
    quad.topLeft = { leftX,  topY,      r, g, b, a,   0.0, 1.0,    glTextureIndex };
}

void Renderer::prepareQuad(int batchIndex, Quad& input)
{
    Batch& batch = batches[batchIndex];
    batch.quadBuffer[batch.quadIndex] = input;
    batch.quadIndex++;
}

void Renderer::sendToGL()
{
    shader.use();
    shader.setMatrix("MVP", Game::main.projection * Game::main.view);

    int currentBatch = 0;
    int texUnit = 0;
    for (int i = 0; i < textureIDs.size(); i++)
    {
        // std::cout << "texUnit: " << texUnit << std::endl;

        glActiveTexture(GL_TEXTURE0 + texUnit);
        glBindTexture(GL_TEXTURE_2D, textureIDs[i]);

        if (texUnit >= MAX_TEXTURES_PER_BATCH - 1)
        {
            flush(batches[currentBatch]);

            currentBatch++;
            texUnit = 0;
        }
        else
        {
            texUnit++;
        }
    }

    flush(batches[currentBatch]);
}

void Renderer::prepareDownLine(float x, float y, float height)
{
    constexpr float halfWidth = 0.5f;
    Quad quad;
    quad.topRight = { x + halfWidth, y,            1.0f, 1.0f, 1.0f, 1.0f,   1.0f, 0.0f,    whiteTextureIndex };
    quad.bottomRight = { x + halfWidth, y - height,   1.0f, 1.0f, 1.0f, 1.0f,   1.0f, 0.0f,    whiteTextureIndex };
    quad.bottomLeft = { x - halfWidth, y - height,   1.0f, 1.0f, 1.0f, 1.0f,   0.0f, 0.0f,    whiteTextureIndex };
    quad.topLeft = { x - halfWidth, y,            1.0f, 1.0f, 1.0f, 1.0f,   0.0f, 1.0f,    whiteTextureIndex };
    prepareQuad(0, quad);
}

void Renderer::prepareRightLine(float x, float y, float width)
{
    constexpr float halfHeight = 0.5f;
    Quad quad;
    quad.topRight = { x + width, y + halfHeight, 1.0f, 1.0f, 1.0f, 1.0f,   1.0f, 0.0f,    whiteTextureIndex };
    quad.bottomRight = { x + width, y - halfHeight, 1.0f, 1.0f, 1.0f, 1.0f,   1.0f, 0.0f,    whiteTextureIndex };
    quad.bottomLeft = { x        , y - halfHeight, 1.0f, 1.0f, 1.0f, 1.0f,   0.0f, 0.0f,    whiteTextureIndex };
    quad.topLeft = { x        , y + halfHeight, 1.0f, 1.0f, 1.0f, 1.0f,   0.0f, 1.0f,    whiteTextureIndex };
    prepareQuad(0, quad);
}


void Renderer::flush(const Batch& batch)
{
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO); // Must bind VBO before glBufferSubData
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadIBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, batch.quadIndex * sizeof(Quad), &batch.quadBuffer[0]);
    glDrawElements(GL_TRIANGLES, batch.quadIndex * 6, GL_UNSIGNED_INT, nullptr);
}

void Renderer::resetBuffers()
{
    for (Batch& batch : batches)
    {
        batch.quadIndex = 0;
    }
}
