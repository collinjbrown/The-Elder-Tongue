// main.cpp
//

#include <iostream>
#include <filesystem>
#include <map>
#include <stack>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "game.h"
#include "check_error.h"
#include "texture_2D.h"
#include "renderer.h"
#include "entity.h"
#include "ecs.h"

Game Game::main;
ECS ECS::main;

// This is the hub which handles updates and setup.
// In an attempt to keep this from getting cluttered, we're keeping some information
// regarding game and world states in the game and engine class respectively.
// We don't want this to be so cluttered that you have to dig to find what you want
// so don't put stuff here unless it really belongs.

static int windowMoved = 0;
double startMoveTime = 0.0;
double endMoveTime = 0.0;
void WindowPosCallback(GLFWwindow* window, int xpos, int ypos)
{
    windowMoved = 1;
}

int main(void)
{
    #pragma region GL Rendering Setup
    int windowWidth = Game::main.windowWidth;
    int windowHeight = Game::main.windowHeight;

    // Here we're initiating all the stuff related to rendering.
    GLFWwindow* window;

    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindowHintString(GLFW_X11_CLASS_NAME, "OpenGL");
    glfwWindowHintString(GLFW_X11_INSTANCE_NAME, "OpenGL");

    window = glfwCreateWindow(windowWidth, windowHeight, "The Elder Tongue", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << '\n';
        return -1;
    }

    glfwSetWindowPosCallback(window, WindowPosCallback);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Game::main.window = window;
    #pragma endregion

    #pragma region World Setup

    ECS::main.Init();

    #pragma endregion

    #pragma region Camera & Texture Setup
    // Now that we've finished that, we'll set up the camera
    // and textures that we'll need later.
    Game::main.updateOrtho();

    Texture2D* whiteTexture = Texture2D::whiteTexture();

    Renderer renderer{ whiteTexture->ID };

    Texture2D dot{ "assets/sprites/dot.png", true, GL_NEAREST };
    renderer.textureIDs.push_back(dot.ID);
    Game::main.textureMap.emplace("dot", &dot);

    Texture2D test{ "assets/sprites/test.png", true, GL_NEAREST };
    renderer.textureIDs.push_back(test.ID);
    Game::main.textureMap.emplace("test", &test);

    Texture2D blank{ "assets/sprites/blank.png", true, GL_NEAREST };
    renderer.textureIDs.push_back(blank.ID);
    Game::main.textureMap.emplace("blank", &blank);

    #pragma region Player Animations

    Animation2D baseIdle{ "assets/animations/base/baseIdle.png", true, 2, 2, 1.0f, { 2, 2 }, true, GL_NEAREST };
    renderer.textureIDs.push_back(baseIdle.ID);
    Game::main.animationMap.emplace("baseIdle", &baseIdle);

    Animation2D baseWalk{ "assets/animations/base/baseWalk.png", true, 3, 3, 0.1f, { 2, 3, 3 }, true, GL_NEAREST };
    renderer.textureIDs.push_back(baseWalk.ID);
    Game::main.animationMap.emplace("baseWalk", &baseWalk);

    Animation2D baseJumpPrep{ "assets/animations/base/baseJumpPrep.png", true, 1, 1, 5.0f, { 1 }, true, GL_NEAREST };
    renderer.textureIDs.push_back(baseJumpPrep.ID);
    Game::main.animationMap.emplace("baseJumpPrep", &baseJumpPrep);

    Animation2D baseJumpUp{ "assets/animations/base/baseJumpUp.png", true, 1, 1, 5.0f, { 1 }, true, GL_NEAREST };
    renderer.textureIDs.push_back(baseJumpUp.ID);
    Game::main.animationMap.emplace("baseJumpUp", &baseJumpUp);

    Animation2D baseJumpDown{ "assets/animations/base/baseJumpDown.png", true, 2, 2, 1.0f, { 2, 2 }, true, GL_NEAREST };
    renderer.textureIDs.push_back(baseJumpDown.ID);
    Game::main.animationMap.emplace("baseJumpDown", &baseJumpDown);

    Animation2D baseDeath{ "assets/animations/base/baseDying.png", true, 4, 4, 1.0f, { 2, 4, 4, 4 }, false, GL_NEAREST };
    renderer.textureIDs.push_back(baseDeath.ID);
    Game::main.animationMap.emplace("baseDeath", &baseDeath);


    #pragma endregion

    #pragma region Test Character Animations

    Animation2D testIdle{ "assets/animations/test/testIdle.png", true, 2, 2, 1.0f, { 2, 2 }, true, GL_NEAREST };
    renderer.textureIDs.push_back(testIdle.ID);
    Game::main.animationMap.emplace("testIdle", &testIdle);

    Animation2D testWalk{ "assets/animations/test/testWalk.png", true, 3, 3, 0.1f, { 2, 3, 3 }, true, GL_NEAREST };
    renderer.textureIDs.push_back(testWalk.ID);
    Game::main.animationMap.emplace("testWalk", &testWalk);

    Animation2D testJumpPrep{ "assets/animations/test/testJumpPrep.png", true, 1, 1, 5.0f, { 1 }, true, GL_NEAREST };
    renderer.textureIDs.push_back(testJumpPrep.ID);
    Game::main.animationMap.emplace("testJumpPrep", &testJumpPrep);

    Animation2D testJumpUp{ "assets/animations/test/testJumpUp.png", true, 1, 1, 5.0f, { 1 }, true, GL_NEAREST };
    renderer.textureIDs.push_back(testJumpUp.ID);
    Game::main.animationMap.emplace("testJumpUp", &testJumpUp);

    Animation2D testJumpDown{ "assets/animations/test/testJumpDown.png", true, 2, 2, 1.0f, { 2, 2 }, true, GL_NEAREST };
    renderer.textureIDs.push_back(testJumpDown.ID);
    Game::main.animationMap.emplace("testJumpDown", &testJumpDown);

    Animation2D testDeath{ "assets/animations/test/testDying.png", true, 4, 4, 1.0f, { 1, 4, 4, 4 }, false, GL_NEAREST };
    renderer.textureIDs.push_back(testDeath.ID);
    Game::main.animationMap.emplace("testDeath", &testDeath);

    #pragma endregion

    Game::main.renderer = &renderer;
    #pragma endregion

    #pragma region Game Loop
    // This is the loop where the game runs, duh.
    // Everything that should happen each frame should occur here,
    // or nested somewhere within methods called in here.
    double lastTime = glfwGetTime();
    int frameCount = 0;

    float checkedTime = glfwGetTime();
    float elapsedTime = 0.0f;

    bool fullscreen = false;
    float lastChange = glfwGetTime();

    bool slowTime = false;
    float slowLastChange = glfwGetTime();

    while (!glfwWindowShouldClose(window))
    {
        #pragma region Elapsed Time

        float deltaTime = glfwGetTime() - checkedTime;
        // std::cout << "Delta Time: " + std::to_string(deltaTime) + "\n";
        checkedTime = glfwGetTime();

        #pragma endregion

        #pragma region FPS
        double currentTime = glfwGetTime();
        frameCount++;
        // If a second has passed.
        if (currentTime - lastTime >= 1.0)
        {
            // Display the frame count here any way you want.
            std::cout << "Frame Count: " + std::to_string(frameCount) + "\n";

            frameCount = 0;
            lastTime = currentTime;
        }

        #pragma endregion

        #pragma region Update Worldview

        int w, h;
        glfwGetWindowSize(window, &w, &h);

        Game::main.windowWidth = w;
        Game::main.windowHeight = h;

        if (glfwGetKey(window, GLFW_KEY_F11) == GLFW_PRESS && glfwGetTime() > lastChange + 0.5f)
        {
            lastChange = glfwGetTime();

            if (fullscreen)
            {
                fullscreen = false;
                glfwSetWindowMonitor(window, NULL, 0, 0, 1280, 960, GLFW_REFRESH_RATE);
            }
            else
            {
                fullscreen = true;
                glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, 1920, 1080, GLFW_REFRESH_RATE);
            }
        }

        // Here, we make sure the camera is oriented correctly.
        glm::vec3 cam = glm::vec3(Game::main.camX, Game::main.camY, Game::main.camZ);
        glm::vec3 center = cam + glm::vec3(0.0f, 0.0f, -1.0f);
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
        Game::main.view = glm::lookAt(cam, center, up);

        // Here we manage the game window and view.
        const float halfWindowHeight = Game::main.windowHeight * Game::main.zoom * 0.5f;
        const float halfWindowWidth = Game::main.windowWidth * Game::main.zoom * 0.5f;
        Game::main.topY = Game::main.camY + halfWindowHeight;
        Game::main.bottomY = Game::main.camY - halfWindowHeight;
        Game::main.rightX = Game::main.camX + halfWindowWidth;
        Game::main.leftX = Game::main.camX - halfWindowWidth;
        #pragma endregion

        #pragma region Input
        double mPosX;
        double mPosY;
        glfwGetCursorPos(window, &mPosX, &mPosY);

        const double xNDC = (mPosX / (Game::main.windowWidth / 2.0f)) - 1.0f;
        const double yNDC = 1.0f - (mPosY / (Game::main.windowHeight / 2.0f));
        glm::mat4 VP = Game::main.projection * Game::main.view;
        glm::mat4 VPinv = glm::inverse(VP);
        glm::vec4 mouseClip = glm::vec4((float)xNDC, (float)yNDC, 1.0f, 1.0f);
        glm::vec4 worldMouse = VPinv * mouseClip;
        Game::main.deltaMouseX = worldMouse.x - Game::main.mouseX;
        Game::main.deltaMouseY = worldMouse.y - Game::main.mouseY;
        Game::main.mouseX = worldMouse.x;
        Game::main.mouseY = worldMouse.y;

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, true);
        }

        if (glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS)
        {
            if (Game::main.zoom - 5.0f * deltaTime > 0.1f)
            {
                Game::main.zoom -= 5.0f * deltaTime;

                Game::main.updateOrtho();
            }
        }
        else if (glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS)
        {
            if (Game::main.zoom + 5.0f * deltaTime < 2.5f)
            {
                Game::main.zoom += 5.0f * deltaTime;

                Game::main.updateOrtho();
            }
        }

        #pragma endregion

        #pragma region GL Color & Clear
        // Here we update the background color.
        // We want it black for now, so that's what it is.
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        #pragma endregion

        #pragma region Update World State

        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && glfwGetTime() > slowLastChange + 0.5f)
        {
            slowLastChange = glfwGetTime();

            if (!slowTime)
            {
                slowTime = true;
            }
            else
            {
                slowTime = false;
            }
        }

        if (slowTime)
        {
            deltaTime *= 0.5f;
        }

        int focus = glfwGetWindowAttrib(window, GLFW_FOCUSED);

        if (focus && !windowMoved)
        {
            ECS::main.Update(deltaTime);
        }

        #pragma endregion;

        #pragma region Render
        // This is where we finally render and reset buffers.
        Game::main.renderer->sendToGL();
        Game::main.renderer->resetBuffers();

        glfwSwapBuffers(window);

        windowMoved = 0;
        glfwPollEvents();
        glCheckError();
        #pragma endregion
    }
    #pragma endregion

    #pragma region Shutdown
    delete whiteTexture;

    glfwTerminate();
    return 0;
    #pragma endregion
}
