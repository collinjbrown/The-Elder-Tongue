#include "textrenderer.h"
#include "renderer.h"
#include "game.h"

#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

glm::vec2 TextRenderer::CalculateAlignment(std::string text, float scaleX, float scaleY, TextAlignment alignment)
{
    float x = 0;
    float y = 14;

    int di = text.size();

    if (alignment == TextAlignment::left) di = 1;

    for (int i = 0; i < di; i++)
    {
        const char c = text[i];
        Character ch = characters[c];

        float w = ch.size.x * scaleX;
        float h = ch.size.y * scaleY;

        x += (ch.advance >> 6) * scaleX;
    }

    if (alignment == TextAlignment::left)
    {
        return glm::vec2(-x, -(y / 2.0f));
    }
    else if (alignment == TextAlignment::center)
    {
        return glm::vec2(-(x / 2.0f), -(y / 2.0f));
    }
    else if (alignment == TextAlignment::right)
    {
        return glm::vec2(-x, -(y / 2.0f));
    }
}

void TextRenderer::RenderText(std::string text, float x, float y, float scaleX, float scaleY, glm::vec4 color)
{
    float wMod = Game::main.renderer->CalculateModifier(characters['c'].size.x);
    float hMod = Game::main.renderer->CalculateModifier(characters['c'].size.y);

    for (int i = 0; i < text.size(); i++)
    {
        const char c = text[i];
        Character ch = characters[c];

        float xPos = x + ch.bearing.x * scaleX;
        float yPos = y - (ch.size.y - ch.bearing.y) * scaleY;

        float w = ch.size.x * scaleX;
        float h = ch.size.y * scaleY;

        float leftX = ch.textureX / atlasWidth;
        float rightX = (ch.textureX + (float)ch.size.x) / atlasWidth;
        float topY = ch.size.y / (float)atlasHeight;

        Quad quad;

        quad.topLeft        = {     xPos + w,   yPos + h,   -100.0f,   color.r, color.g, color.b, color.a,     rightX, 0.0f, 0, 0, wMod, hMod };
        quad.bottomLeft     = {     xPos + w,   yPos,       -100.0f,   color.r, color.g, color.b, color.a,     rightX, topY, 0, 0, wMod, hMod };
        quad.bottomRight    = {     xPos,       yPos,       -100.0f,   color.r, color.g, color.b, color.a,     leftX,  topY, 0, 0, wMod, hMod };
        quad.topRight       = {     xPos,       yPos + h,   -100.0f,   color.r, color.g, color.b, color.a,     leftX,  0.0f, 0, 0, wMod, hMod };

        Game::main.renderer->prepareQuad(quad, textureAtlas, mapAtlas);

        x += (ch.advance >> 6) * scaleX;
    }
    
    return;
}

TextRenderer::TextRenderer(const std::string& fontpath, FT_UInt pixelSize)
{
    FT_Library ft;

    if (FT_Init_FreeType(&ft))
    {
        std::cout << "Error::Freetype: Could not initiate the FreeType Library." << std::endl;
    }

    FT_Face face;
    if (FT_New_Face(ft, fontpath.c_str(), 0, &face))
    {
        std::cout << "Error::Freetype: Could not load selected font." << std::endl;
    }

    FT_Set_Pixel_Sizes(face, 0, pixelSize);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    int combinedWidth = 0;
    unsigned int maxHeight = 0;

    constexpr int ftLoadFlags = FT_LOAD_RENDER | FT_LOAD_COLOR;
    constexpr int imageFormat = GL_RGBA;

    std::cout << "Freetype: Successfully initiated." << std::endl;

    int totalPixels = 0;
    for (unsigned char c = 0; c < 128; c++)
    {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cout << "Error::Freetype: Failed to load Glyph." << std::endl;
            continue;
        }

        totalPixels += face->glyph->bitmap.width * face->glyph->bitmap.rows;

        combinedWidth += face->glyph->bitmap.width;
        maxHeight = std::max(maxHeight, face->glyph->bitmap.rows);
    }

    atlasHeight = maxHeight;

    glGenTextures(1, &textureAtlas);
    glBindTexture(GL_TEXTURE_2D, textureAtlas);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexImage2D(GL_TEXTURE_2D, 0, imageFormat, combinedWidth, maxHeight, 0, imageFormat, GL_UNSIGNED_BYTE, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    std::vector<int> usedAlphas;
    usedAlphas.push_back(0);

    int mapWidth = 0;
    int mapHeight = 0;

    int x = 0;
    int j = 0;
    unsigned char* data = new unsigned char[256 * 4];
    for (unsigned char c = 0; c < 128; c++)
    {
        if (FT_Load_Char(face, c, ftLoadFlags))
        {
            std::cout << "Error::Freetype: Failed to load Glyph." << std::endl;
            continue;
        }

        const int newBitmapLength = face->glyph->bitmap.width * face->glyph->bitmap.rows * 4;

        unsigned char* rgbaBitmap = new unsigned char[newBitmapLength];

        for (int i = 0; i < newBitmapLength; i += 4)
        {
            auto result = std::find(usedAlphas.begin(), usedAlphas.end(), (int)face->glyph->bitmap.buffer[i / 4]);
            int alphaIndex;
            if (result == usedAlphas.end())
            {
                alphaIndex = usedAlphas.size();
                usedAlphas.push_back((int)face->glyph->bitmap.buffer[i / 4]);
            }
            else
            {
                alphaIndex = result - usedAlphas.begin();
            }

            int nx = alphaIndex % 256;
            int ny = round(alphaIndex / 256);

            rgbaBitmap[i] = nx;
            rgbaBitmap[i + 1] = ny;
            rgbaBitmap[i + 2] = 0;
            rgbaBitmap[i + 3] = face->glyph->bitmap.buffer[i / 4];

            mapWidth = std::max(mapWidth, alphaIndex % 256);
            mapHeight = std::max(mapHeight, (int)round(alphaIndex / 256));

            // std::cout << "Coords: " + std::to_string(rgbaBitmap[i]) + " / " + std::to_string(rgbaBitmap[i + 1]) + " :: " + std::to_string(face->glyph->bitmap.buffer[i / 4]) << std::endl;

            data[((ny * 255 + nx) * 4) + 0] = UCHAR_MAX;
            data[((ny * 255 + nx) * 4) + 1] = UCHAR_MAX;
            data[((ny * 255 + nx) * 4) + 2] = UCHAR_MAX;
            data[((ny * 255 + nx) * 4) + 3] = face->glyph->bitmap.buffer[i / 4];

            j++;
        }

        glTexSubImage2D(GL_TEXTURE_2D, 0, x, 0, face->glyph->bitmap.width, face->glyph->bitmap.rows, imageFormat, GL_UNSIGNED_BYTE, rgbaBitmap);
        delete[] rgbaBitmap;

        Character character = {
            glm::vec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::vec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            face->glyph->advance.x,
            (float)x
        };

        this->characters.insert(std::pair<char, Character>(c, character));
        x += face->glyph->bitmap.width;
    }

    glBindTexture(GL_TEXTURE_2D, 0);

    glGenTextures(1, &mapAtlas);
    glBindTexture(GL_TEXTURE_2D, mapAtlas);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexImage2D(GL_TEXTURE_2D, 0, imageFormat, mapWidth + 1, mapHeight + 1, 0, imageFormat, GL_UNSIGNED_BYTE, data);
    delete[] data;

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glBindTexture(GL_TEXTURE_2D, 0);

    atlasWidth = x;

    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    Game::main.renderer->textureIDs.push_back(textureAtlas);
    Game::main.renderer->textureIDs.push_back(mapAtlas);

    this->texWidth = combinedWidth;
    this->texHeight = maxHeight;

}