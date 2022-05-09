// main.cpp
//

#include <iostream>
#include <filesystem>
#include <map>
#include <stack>
#include <thread>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "game.h"
#include "check_error.h"
// #include "texture_2D.h"
// #include "renderer.h"
#include "entity.h"
#include "particleengine.h"
#include "ecs.h"

Game Game::main;
ECS ECS::main;
ParticleEngine ParticleEngine::main;

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


void MessageCallback(GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    const void* userParam)
{
    fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
        (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
        type, severity, message);

    std::cout << "\n";
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

    window = glfwCreateWindow(windowWidth, windowHeight, "The Moonlight Tongue", NULL, NULL);
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

    /*glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, 0);*/

    Game::main.window = window;
    
    printf("OpenGL version supported by this platform (%s): \n", glGetString(GL_VERSION));
    #pragma endregion

    #pragma region World Setup

    srand(time(NULL));
    ECS::main.Init();
    ParticleEngine::main.Init(0.05f);

    #pragma endregion

    #pragma region Camera & Texture Setup
    // Now that we've finished that, we'll set up the camera
    // and textures that we'll need later.
    Game::main.updateOrtho();

    Texture2D* whiteTexture = Texture2D::whiteTexture();

    Renderer renderer{ whiteTexture->ID };

    // I should talk about textures. Every texture has a source and a map.
    // The source textures are the same across all the sprites and animations for an object or character,
    // so instead of altering a source, one creates a map for any alternative forms of sprites or animations.
    // Each pixel in the source sprite has an r and a b value that denotes some x and y value on the map
    // which is used to find the color of the pixel.

    //Texture2D dot{ "assets/sprites/dot.png", true, GL_NEAREST };
    //renderer.textureIDs.push_back(dot.ID);
    //Game::main.textureMap.emplace("dot", &dot);

    //Texture2D test{ "assets/sprites/test.png", true, GL_NEAREST };
    //renderer.textureIDs.push_back(test.ID);
    //Game::main.textureMap.emplace("test", &test);

    //Texture2D blank{ "assets/sprites/blank.png", true, GL_NEAREST };
    //renderer.textureIDs.push_back(blank.ID);
    //Game::main.textureMap.emplace("blank", &blank);

    /*Texture2D moonlightSlashMap{ "assets/animations/slash/moonlight_slash_map.png", true, GL_NEAREST };
    renderer.textureIDs.push_back(moonlightSlashMap.ID);
    Game::main.textureMap.emplace("moonlightSlashMap", &moonlightSlashMap);*/

    Texture2D wreathedSlashMap{ "assets/animations/slash/wreathed_slash_map.png", true, GL_NEAREST };
    renderer.textureIDs.push_back(wreathedSlashMap.ID);
    Game::main.textureMap.emplace("wreathed_slashMap", &wreathedSlashMap);

    Animation2D wreathedSlashUp{ "assets/animations/slash/wreathed_slashOne.png", true, 3, 3, 0.02f, { 2, 3, 3 }, false, GL_NEAREST };
    renderer.textureIDs.push_back(wreathedSlashUp.ID);
    Game::main.animationMap.emplace("wreathed_slashUp", &wreathedSlashUp);

    Animation2D wreathedSlashDown{ "assets/animations/slash/wreathed_slashTwo.png", true, 3, 3, 0.02f, { 2, 3, 3 }, false, GL_NEAREST };
    renderer.textureIDs.push_back(wreathedSlashDown.ID);
    Game::main.animationMap.emplace("wreathed_slashDown", &wreathedSlashDown);

    /*Texture2D slashMap{ "assets/animations/slash/slash_map.png", true, GL_NEAREST };
    renderer.textureIDs.push_back(slashMap.ID);
    Game::main.textureMap.emplace("slashMap", &slashMap);

    Animation2D slashUp{ "assets/animations/slash/slashUp.png", true, 2, 3, 0.01f, { 1, 2, 2 }, false, GL_NEAREST};
    renderer.textureIDs.push_back(slashUp.ID);
    Game::main.animationMap.emplace("slashUp", &slashUp);

    Animation2D slashDown{ "assets/animations/slash/slashDown.png", true, 2, 3, 0.01f, { 1, 2, 2 }, false, GL_NEAREST };
    renderer.textureIDs.push_back(slashDown.ID);
    Game::main.animationMap.emplace("slashDown", &slashDown);*/

    Texture2D bullet{ "assets/sprites/bullets/bullet.png", true, GL_NEAREST };
    renderer.textureIDs.push_back(bullet.ID);
    Game::main.textureMap.emplace("bullet", &bullet);

    Texture2D aetherBullet{ "assets/sprites/bullets/bullet_aether.png", true, GL_NEAREST };
    renderer.textureIDs.push_back(aetherBullet.ID);
    Game::main.textureMap.emplace("aether_bullet", &aetherBullet);

    Texture2D wallMap{ "assets/sprites/world/test/wall_map.png", true, GL_NEAREST };
    renderer.textureIDs.push_back(wallMap.ID);
    Game::main.textureMap.emplace("wallMap", &wallMap);

    Texture2D wall{ "assets/sprites/world/test/wall.png", true, GL_NEAREST };
    renderer.textureIDs.push_back(wall.ID);
    Game::main.textureMap.emplace("wall", &wall);

    /*Texture2D test{ "assets/sprites/test.png", true, GL_NEAREST };
    renderer.textureIDs.push_back(test.ID);
    Game::main.textureMap.emplace("test", &test);

    Texture2D testMap{ "assets/sprites/test_map.png", true, GL_NEAREST };
    renderer.textureIDs.push_back(testMap.ID);
    Game::main.textureMap.emplace("testMap", &testMap);*/

    Texture2D alphaWatermark{ "assets/sprites/alpha_watermark/alpha_watermark.png", true, GL_NEAREST };
    renderer.textureIDs.push_back(alphaWatermark.ID);
    Game::main.textureMap.emplace("watermark", &alphaWatermark);

    Texture2D alphaWatermarkMark{ "assets/sprites/alpha_watermark/alpha_watermark_map.png", true, GL_NEAREST };
    renderer.textureIDs.push_back(alphaWatermarkMark.ID);
    Game::main.textureMap.emplace("watermarkMap", &alphaWatermarkMark);

    Texture2D blank{ "assets/sprites/blank.png", true, GL_NEAREST };
    renderer.textureIDs.push_back(blank.ID);
    Game::main.textureMap.emplace("blank", &blank);

    Texture2D blankMap{ "assets/sprites/blank_map.png", true, GL_NEAREST };
    renderer.textureIDs.push_back(blankMap.ID);
    Game::main.textureMap.emplace("base_map", &blankMap);

    Texture2D moonlightBlade{ "assets/sprites/blade/moonlight_blade.png", true, GL_NEAREST };
    renderer.textureIDs.push_back(moonlightBlade.ID);
    Game::main.textureMap.emplace("moonlightBlade", &moonlightBlade);

    Texture2D moonlightBladeMap{ "assets/sprites/blade/moonlight_blade_map.png", true, GL_NEAREST };
    renderer.textureIDs.push_back(moonlightBladeMap.ID);
    Game::main.textureMap.emplace("moonlightBladeMap", &moonlightBladeMap);

    Texture2D moonlightBladeIncorporealMap{ "assets/sprites/blade/moonlight_blade_incorporealMap.png", true, GL_NEAREST };
    renderer.textureIDs.push_back(moonlightBladeIncorporealMap.ID);
    Game::main.textureMap.emplace("moonlightBladeIncorporealMap", &moonlightBladeIncorporealMap);

    Texture2D skull{ "assets/sprites/skull/skull.png", true, GL_NEAREST };
    renderer.textureIDs.push_back(skull.ID);
    Game::main.textureMap.emplace("skull", &skull);

    Texture2D skullMap{ "assets/sprites/skull/skull_map.png", true, GL_NEAREST };
    renderer.textureIDs.push_back(skullMap.ID);
    Game::main.textureMap.emplace("skullMap", &skullMap);

    #pragma region Base Player Animations

    Texture2D lilyMap{ "assets/animations/lily/lily_base_map.png", true, GL_NEAREST };
    renderer.textureIDs.push_back(lilyMap.ID);
    Game::main.textureMap.emplace("lilyMap", &lilyMap);

    Animation2D baseIdle{ "assets/animations/lily/lily_idle.png", true, 2, 2, 0.5f, { 2, 2 }, true, GL_NEAREST };
    renderer.textureIDs.push_back(baseIdle.ID);
    Game::main.animationMap.emplace("baseIdle", &baseIdle);

    Animation2D baseWalk{ "assets/animations/lily/lily_run.png", true, 3, 4, 0.05f, { 1, 3, 3, 3 }, true, GL_NEAREST };
    renderer.textureIDs.push_back(baseWalk.ID);
    Game::main.animationMap.emplace("baseWalk", &baseWalk);

    Animation2D baseJumpUp{ "assets/animations/lily/lily_up.png", true, 1, 1, 5.0f, { 1 }, true, GL_NEAREST };
    renderer.textureIDs.push_back(baseJumpUp.ID);
    Game::main.animationMap.emplace("baseJumpUp", &baseJumpUp);

    Animation2D baseJumpDown{ "assets/animations/lily/lily_down.png", true,  2, 2, 1.0f, { 2, 2 }, true, GL_NEAREST };
    renderer.textureIDs.push_back(baseJumpDown.ID);
    Game::main.animationMap.emplace("baseJumpDown", &baseJumpDown);

    Animation2D baseWallRun{ "assets/animations/lily/lily_wallRun.png", true,  3, 4, 0.1f, { 1, 3, 3, 3 }, true, GL_NEAREST };
    renderer.textureIDs.push_back(baseWallRun.ID);
    Game::main.animationMap.emplace("baseWallRun", &baseWallRun);

    Animation2D baseSlideDown{ "assets/animations/lily/lily_slideDown.png", true,  1, 1, 5.0f, { 1 }, true, GL_NEAREST };
    renderer.textureIDs.push_back(baseSlideDown.ID);
    Game::main.animationMap.emplace("baseSlideDown", &baseSlideDown);

    Animation2D baseSlide{ "assets/animations/lily/lily_slide.png", true,  1, 2, 1.0f, { 1, 1 }, true, GL_NEAREST };
    renderer.textureIDs.push_back(baseSlide.ID);
    Game::main.animationMap.emplace("baseSlide", &baseSlide);

    Animation2D baseCrouch{ "assets/animations/lily/lily_crouch.png", true, 2, 2, 0.5f, { 2, 2 }, true, GL_NEAREST };
    renderer.textureIDs.push_back(baseCrouch.ID);
    Game::main.animationMap.emplace("baseCrouch", &baseCrouch);

    Animation2D baseCrouchWalk{ "assets/animations/lily/lily_crouchWalk.png", true, 3, 4, 0.1f, { 1, 3, 3, 3 }, true, GL_NEAREST };
    renderer.textureIDs.push_back(baseCrouchWalk.ID);
    Game::main.animationMap.emplace("baseCrouchWalk", &baseCrouchWalk);

    Animation2D baseDeath{ "assets/animations/base/baseDying.png", true, 4, 4, 1.0f, { 2, 4, 4, 4 }, false, GL_NEAREST };
    renderer.textureIDs.push_back(baseDeath.ID);
    Game::main.animationMap.emplace("baseDeath", &baseDeath);

    Animation2D baseSlashOne{ "assets/animations/lily/lily_slashOne.png", true, 2, 3, 0.05f, { 2, 2, 2 }, false, GL_NEAREST };
    renderer.textureIDs.push_back(baseSlashOne.ID);
    Game::main.animationMap.emplace("baseSlashOne", &baseSlashOne);

    Animation2D baseSlashTwo{ "assets/animations/lily/lily_slashTwo.png", true, 2, 3, 0.05f, { 1, 2, 2 }, false, GL_NEAREST };
    renderer.textureIDs.push_back(baseSlashTwo.ID);
    Game::main.animationMap.emplace("baseSlashTwo", &baseSlashTwo);

    #pragma endregion

    #pragma region Offensive (as in Combat) Player Animations

    Texture2D lilyWreathedMap{ "assets/animations/lily/wreathed/lily_wreathed_map.png", true, GL_NEAREST };
    renderer.textureIDs.push_back(lilyWreathedMap.ID);
    Game::main.textureMap.emplace("lilyWreathedMap", &lilyWreathedMap);

    Animation2D wreathedIdle{ "assets/animations/lily/wreathed/wreathed_lily_idle.png", true, 2, 2, 0.5f, { 2, 2 }, true, GL_NEAREST };
    renderer.textureIDs.push_back(wreathedIdle.ID);
    Game::main.animationMap.emplace("wreathed_baseIdle", &wreathedIdle);

    Animation2D wreathedWalk{ "assets/animations/lily/wreathed/wreathed_lily_run.png", true, 3, 4, 0.05f, { 1, 3, 3, 3 }, true, GL_NEAREST };
    renderer.textureIDs.push_back(wreathedWalk.ID);
    Game::main.animationMap.emplace("wreathed_baseWalk", &wreathedWalk);

    Animation2D wreathedJumpUp{ "assets/animations/lily/wreathed/wreathed_lily_up.png", true, 2, 2, 0.1f, { 1, 2 }, true, GL_NEAREST };
    renderer.textureIDs.push_back(wreathedJumpUp.ID);
    Game::main.animationMap.emplace("wreathed_baseJumpUp", &wreathedJumpUp);

    Animation2D wreathedJumpDown{ "assets/animations/lily/wreathed/wreathed_lily_down.png", true,  2, 2, 0.5f, { 2, 2 }, true, GL_NEAREST };
    renderer.textureIDs.push_back(wreathedJumpDown.ID);
    Game::main.animationMap.emplace("wreathed_baseJumpDown", &wreathedJumpDown);

    Animation2D wreathedWallRun{ "assets/animations/lily/wreathed/wreathed_lily_wallRun.png", true,  3, 4, 0.1f, { 1, 3, 3, 3 }, true, GL_NEAREST };
    renderer.textureIDs.push_back(wreathedWallRun.ID);
    Game::main.animationMap.emplace("wreathed_baseWallRun", &wreathedWallRun);

    Animation2D wreathedSlideDown{ "assets/animations/lily/wreathed/wreathed_lily_slideDown.png", true,  1, 1, 5.0f, { 1 }, true, GL_NEAREST };
    renderer.textureIDs.push_back(wreathedSlideDown.ID);
    Game::main.animationMap.emplace("wreathed_baseSlideDown", &wreathedSlideDown);

    Animation2D wreathedSlide{ "assets/animations/lily/wreathed/wreathed_lily_slide.png", true,  1, 2, 1.0f, { 1, 1 }, true, GL_NEAREST };
    renderer.textureIDs.push_back(wreathedSlide.ID);
    Game::main.animationMap.emplace("wreathed_baseSlide", &wreathedSlide);

    Animation2D wreathedCrouch{ "assets/animations/lily/wreathed/wreathed_lily_crouch.png", true, 2, 2, 0.5f, { 2, 2 }, true, GL_NEAREST };
    renderer.textureIDs.push_back(wreathedCrouch.ID);
    Game::main.animationMap.emplace("wreathed_baseCrouch", &wreathedCrouch);

    Animation2D wreathedCrouchWalk{ "assets/animations/lily/wreathed/wreathed_lily_crouchWalk.png", true, 3, 4, 0.1f, { 1, 3, 3, 3 }, true, GL_NEAREST };
    renderer.textureIDs.push_back(wreathedCrouchWalk.ID);
    Game::main.animationMap.emplace("wreathed_baseCrouchWalk", &wreathedCrouchWalk);

    /*Animation2D baseDeath{ "assets/animations/base/baseDying.png", true, 4, 4, 1.0f, { 2, 4, 4, 4 }, false, GL_NEAREST };
    renderer.textureIDs.push_back(baseDeath.ID);*/
    Game::main.animationMap.emplace("wreathed_baseDeath", &baseDeath);

    Animation2D wreathedSlashOne{ "assets/animations/lily/wreathed/wreathed_lily_slashOne.png", true, 3, 3, 0.02f, { 2, 3, 3 }, false, GL_NEAREST };
    renderer.textureIDs.push_back(wreathedSlashOne.ID);
    Game::main.animationMap.emplace("wreathed_baseSlashOne", &wreathedSlashOne);

    Animation2D wreathedSlashTwo{ "assets/animations/lily/wreathed/wreathed_lily_slashTwo.png", true, 3, 3, 0.02f, { 2, 3, 3 }, false, GL_NEAREST };
    renderer.textureIDs.push_back(wreathedSlashTwo.ID);
    Game::main.animationMap.emplace("wreathed_baseSlashTwo", &wreathedSlashTwo);

    #pragma endregion
    //Animation2D swordBaseIdle{ "assets/animations/lily/lily_idle.png", true, 3, 2, 0.5f, { 3, 3 }, true, GL_NEAREST };
    //renderer.textureIDs.push_back(swordBaseIdle.ID);
    //Game::main.animationMap.emplace("sword_baseIdle", &swordBaseIdle);

    //Animation2D swordBaseWalk{ "assets/animations/lily/lily_run.png", true, 3, 4, 0.05f, { 3, 3, 3, 3 }, true, GL_NEAREST };
    //renderer.textureIDs.push_back(swordBaseWalk.ID);
    //Game::main.animationMap.emplace("sword_baseWalk", &swordBaseWalk);

    //Animation2D swordBaseJumpUp{ "assets/animations/lily/lily_up.png", true,  1, 1, 5.0f, { 1 }, true, GL_NEAREST };
    //renderer.textureIDs.push_back(swordBaseJumpUp.ID);
    //Game::main.animationMap.emplace("sword_baseJumpUp", &swordBaseJumpUp);

    //Animation2D swordBaseJumpDown{ "assets/animations/lily/lily_up.png", true,  1, 1, 5.0f, { 1 }, true, GL_NEAREST };
    //renderer.textureIDs.push_back(swordBaseJumpDown.ID);
    //Game::main.animationMap.emplace("sword_baseJumpDown", &swordBaseJumpDown);

    //Animation2D swordBaseStab{ "assets/animations/base/sword_baseStab.png", true, 2, 2, 0.05f, { 2, 2 }, false, GL_NEAREST };
    //renderer.textureIDs.push_back(swordBaseStab.ID);
    //Game::main.animationMap.emplace("sword_baseStab", &swordBaseStab);

    //Animation2D swordBaseAerialOne{ "assets/animations/base/sword_baseAerialOne.png", true, 2, 3, 0.05f, { 2, 2, 2 }, false, GL_NEAREST };
    //renderer.textureIDs.push_back(swordBaseAerialOne.ID);
    //Game::main.animationMap.emplace("sword_baseAerialOne", &swordBaseAerialOne);

    //Animation2D slashBaseAerialOne{ "assets/animations/base/slash_baseAerialOne.png", true, 2, 3, 0.05f, { 2, 2, 2 }, false, GL_NEAREST };
    //renderer.textureIDs.push_back(slashBaseAerialOne.ID);
    //Game::main.animationMap.emplace("slash_baseAerialOne", &slashBaseAerialOne);

    //#pragma endregion

    //#pragma region Test Character Animations

    //Animation2D testIdle{ "assets/animations/test/testIdle.png", true, 2, 2, 1.0f, { 2, 2 }, true, GL_NEAREST };
    //renderer.textureIDs.push_back(testIdle.ID);
    //Game::main.animationMap.emplace("testIdle", &testIdle);

    //Animation2D testWalk{ "assets/animations/test/testWalk.png", true, 3, 3, 0.1f, { 2, 3, 3 }, true, GL_NEAREST };
    //renderer.textureIDs.push_back(testWalk.ID);
    //Game::main.animationMap.emplace("testWalk", &testWalk);

    //Animation2D testJumpPrep{ "assets/animations/test/testJumpPrep.png", true, 1, 1, 5.0f, { 1 }, true, GL_NEAREST };
    //renderer.textureIDs.push_back(testJumpPrep.ID);
    //Game::main.animationMap.emplace("testJumpPrep", &testJumpPrep);

    //Animation2D testJumpUp{ "assets/animations/test/testJumpUp.png", true, 1, 1, 5.0f, { 1 }, true, GL_NEAREST };
    //renderer.textureIDs.push_back(testJumpUp.ID);
    //Game::main.animationMap.emplace("testJumpUp", &testJumpUp);

    //Animation2D testJumpDown{ "assets/animations/test/testJumpDown.png", true, 2, 2, 1.0f, { 2, 2 }, true, GL_NEAREST };
    //renderer.textureIDs.push_back(testJumpDown.ID);
    //Game::main.animationMap.emplace("testJumpDown", &testJumpDown);

    //Animation2D testDeath{ "assets/animations/test/testDying.png", true, 4, 4, 1.0f, { 1, 4, 4, 4 }, false, GL_NEAREST };
    //renderer.textureIDs.push_back(testDeath.ID);
    //Game::main.animationMap.emplace("testDeath", &testDeath);

    //#pragma endregion

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

    bool limitFPS = false;
    int fps = 60;
    const int ms = (int)(1000 * (1.0f / (fps * 2.0f)));
    auto start = std::chrono::steady_clock::now();

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

        auto now = std::chrono::steady_clock::now();
        auto diff = now - start;
        auto end = now + std::chrono::milliseconds(ms);

        // If a second has passed.
        if (diff >= std::chrono::seconds(1))
        {
            start = now;
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
        glClearColor(0.25f, 0.25f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        #pragma endregion

        #pragma region Update World State

        if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS && glfwGetTime() > slowLastChange + 0.5f)
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
            ParticleEngine::main.Update(deltaTime);
        }

        #pragma endregion;

        #pragma region Render
        // This is where we finally render and reset buffers.
        Game::main.renderer->sendToGL();
        Game::main.renderer->resetBuffers();

        if (limitFPS)
        {
            std::this_thread::sleep_until(end);
        }
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