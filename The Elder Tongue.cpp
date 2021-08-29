// The Elder Tongue.cpp
//

#include "texture_2D.h"
#include "game.h"
#include <GLFW/glfw3.h>

Game Game::main;

int main(void)
{
    GLFWwindow* window;

    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindowHintString(GLFW_X11_CLASS_NAME, "OpenGL");
    glfwWindowHintString(GLFW_X11_INSTANCE_NAME, "OpenGL");

    Texture2D* whiteTexture = Texture2D::whiteTexture();
    // QuadRenderer quadRenderer{ whiteTexture->ID };

    Texture2D test{ "assets/sprites/test.png", true };
    // quadRenderer.textureIDs.push_back(test.ID);
    // Game::main.quadRenderer = &quadRenderer;

    window = glfwCreateWindow(640, 480, "The Elder Tongue", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}