#define NOMINMAX   
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <gl/glew.h>

#define SDL_MAIN_HANDLED 
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "camera.h"
#include "shader.h"
#include "model.h"
#include "shapes/cube.h"

#include "lighting/directionalLight.h"

// =====================================================
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    PSTR lpCmdLine, int nCmdShow)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        return -1;
    }

    SDL_SetRelativeMouseMode(SDL_TRUE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_WindowFlags windowFlags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_Window* window = SDL_CreateWindow("Titan renderer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, windowFlags);
    if (window == nullptr)
    {
        return -1;
    }

    // SDL_SetWindowGrab(window, SDL_TRUE);
    SDL_GLContext glContext = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, glContext);
    SDL_GL_SetSwapInterval(1);

    GLenum glewResult = glewInit();
    if (glewResult != GLEW_OK)
    {
        return -1;
    }

    glViewport(0, 0, 1280, 720);  
    glEnable(GL_DEPTH_TEST); 
    glEnable(GL_STENCIL_TEST);
    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // glDepthFunc(GL_ALWAYS); 

    // Wifreframe
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    Shader singleColourShader("shaders/simple.vert", "shaders/singleColour.frag");

    Shader lightingShader("shaders/simple.vert", "shaders/lighting.frag");
    // Model backpack("assets/backpack/backpack.obj");
    Cube cube(glm::vec3(10.f, 0.f, 0.f));
    Cube cube2(glm::vec3(100.f, 0.f, 0.f));
    Cube cube3(glm::vec3(0.f, 0.f, 0.f));
    Cube cube4(glm::vec3(-1.f, -1.f, -1.f));
    cube.SetTexture("textures/crate.png");
    cube2.SetTexture("textures/crate.png");
    cube3.SetTexture("textures/window.png");
    cube4.SetTexture("textures/grass.png");

    const glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
    Camera camera(glm::vec3(0.f, 0.f, 3.f));
    float deltaTime = 0.f;
    float lastFrame = 0.f;

    glm::mat4 projection = glm::perspective(glm::radians(camera.mZoom), 1280.f / 720.f, 0.1f, 100.f);
    lightingShader.Use();
    GLuint transformLocMLighting = glGetUniformLocation(lightingShader.mShaderID, "model");
    GLuint transformLocVLighting = glGetUniformLocation(lightingShader.mShaderID, "view");
    GLuint transformLocPLighting = glGetUniformLocation(lightingShader.mShaderID, "projection");
    glUniformMatrix4fv(transformLocPLighting, 1, GL_FALSE, glm::value_ptr(projection));
    
    singleColourShader.Use();
    GLuint transformLocMSimple = glGetUniformLocation(singleColourShader.mShaderID, "model");
    GLuint transformLocVSimple = glGetUniformLocation(singleColourShader.mShaderID, "view");
    GLuint transformLocPSimple = glGetUniformLocation(singleColourShader.mShaderID, "projection");
    glUniformMatrix4fv(transformLocPSimple, 1, GL_FALSE, glm::value_ptr(projection));
    

    DirectionalLight directionalLight(glm::vec3(0.f, -1.f, 0.f));

    glm::vec3 pointLightPositions[] = {
        glm::vec3( 0.7f,  0.2f,  2.0f),
        glm::vec3( 2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f,  2.0f, -12.0f),
        glm::vec3( 0.0f,  0.0f, -3.0f)
    }; 

    // Set light parameters
    lightingShader.Use();
    lightingShader.SetFloat("material.mShininess", 32.0f);

    directionalLight.SetShaderUniforms(lightingShader);
    for (int i = 0; i < sizeof(pointLightPositions); ++i)
    {
        glm::vec3 lightPos = pointLightPositions[i];
        lightingShader.SetVec3("pointLights[" + std::to_string(i) + "].mPosition", lightPos.x, lightPos.y, lightPos.z);
        lightingShader.SetFloat("pointLights[" + std::to_string(i) + "].mConstant", 1.0f);
        lightingShader.SetFloat("pointLights[" + std::to_string(i) + "].mLinear", 0.22f);
        lightingShader.SetFloat("pointLights[" + std::to_string(i) + "].mQuadratic", 0.2f);
        lightingShader.SetVec3("pointLights[" + std::to_string(i) + "].mAmbient", 0.f, 0.f, 0.f);
        lightingShader.SetVec3("pointLights[" + std::to_string(i) + "].mDiffuse", 0.5f, 0.5f, 0.5f);
        lightingShader.SetVec3("pointLights[" + std::to_string(i) + "].mSpecular", 1.f, 1.f, 1.f);
    }
    lightingShader.SetVec3("spotLight.mPosition", camera.mPosition.x, camera.mPosition.y, camera.mPosition.z);
    lightingShader.SetVec3("spotLight.mDirection", camera.mFront.x, camera.mFront.y, camera.mFront.z);
    lightingShader.SetFloat("spotLight.mCutoff", glm::cos(glm::radians(8.f)));
    lightingShader.SetFloat("spotLight.mOuterCutoff", glm::cos(glm::radians(12.5f)));
    lightingShader.SetVec3("spotLight.mAmbient", 0.f, 0.f, 0.f);
    lightingShader.SetVec3("spotLight.mDiffuse", 0.5f, 0.5f, 0.5f);
    lightingShader.SetVec3("spotLight.mSpecular", 1.f, 1.f, 1.f);

    bool forwardInput = false;
    bool leftInput = false;
    bool backInput = false;
    bool rightInput = false;

    bool running = true;
    while (running)
    {
        float currentFrame = static_cast<float>(SDL_GetTicks()) / 1000.f;
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        SDL_Event event;
        while(SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                running = false;
            }
            if (event.type == SDL_WINDOWEVENT)
            {
                if (event.window.type == SDL_WINDOWEVENT_RESIZED)
                {
                    int width = event.window.data1;
                    int height = event.window.data2;
                    glViewport(0, 0, width, height);
                }
            }
            if (event.type == SDL_KEYDOWN)
            {
                if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
                {
                    running = false;
                }
                if (event.key.keysym.scancode == SDL_SCANCODE_W)
                {
                    forwardInput = true;
                }
                if (event.key.keysym.scancode == SDL_SCANCODE_S)
                {
                    backInput = true;
                }
                if (event.key.keysym.scancode == SDL_SCANCODE_A)
                {
                    leftInput = true;
                }
                if (event.key.keysym.scancode == SDL_SCANCODE_D)
                {
                    rightInput = true;
                }
            }
            if (event.type == SDL_KEYUP)
            {
                if (event.key.keysym.scancode == SDL_SCANCODE_W)
                {
                    forwardInput = false;
                }
                if (event.key.keysym.scancode == SDL_SCANCODE_S)
                {
                    backInput = false;
                }
                if (event.key.keysym.scancode == SDL_SCANCODE_A)
                {
                    leftInput = false;
                }
                if (event.key.keysym.scancode == SDL_SCANCODE_D)
                {
                    rightInput = false;
                }
            }
            if (event.type == SDL_MOUSEMOTION)
            {
                float xOffset = static_cast<float>(event.motion.xrel);
                float yOffset = static_cast<float>(event.motion.yrel);
                camera.ProcessMouseMovement(xOffset, yOffset);
            }
            if (event.type == SDL_MOUSEWHEEL)
            {
                camera.ProcessMouseScroll(static_cast<float>(event.wheel.y));
                projection = glm::perspective(glm::radians(camera.mZoom), 1280.f / 720.f, 0.1f, 100.f);
                lightingShader.Use();
                glUniformMatrix4fv(transformLocPLighting, 1, GL_FALSE, glm::value_ptr(projection));
                singleColourShader.Use();
                glUniformMatrix4fv(transformLocPSimple, 1, GL_FALSE, glm::value_ptr(projection));
            }
        }

        if (forwardInput)
        {
            camera.Move(CameraMovement::FORWARD, deltaTime);
        }
        if (backInput)
        {
            camera.Move(CameraMovement::BACKWARD, deltaTime);
        }
        if (leftInput)
        {
            camera.Move(CameraMovement::LEFT, deltaTime);
        }
        if (rightInput)
        {
            camera.Move(CameraMovement::RIGHT, deltaTime);
        }

        // Rendering here...
        glClearColor(0.2, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );

        // Matrix transforms
        const float angle = static_cast<float>(SDL_GetTicks()) / 1000.f;
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = camera.GetViewMatrix();

        lightingShader.Use();
        lightingShader.SetVec3("lightPos", lightPos.x, lightPos.y, lightPos.z);
        lightingShader.SetVec3("viewPos", camera.mPosition.x, camera.mPosition.y, camera.mPosition.z);
        lightingShader.SetVec3("spotLight.mPosition", camera.mPosition.x, camera.mPosition.y, camera.mPosition.z);
        lightingShader.SetVec3("spotLight.mDirection", camera.mFront.x, camera.mFront.y, camera.mFront.z);
        glUniformMatrix4fv(transformLocMLighting, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(transformLocVLighting, 1, GL_FALSE, glm::value_ptr(view));
        // backpack.Draw(lightingShader);

        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilMask(0xFF);
        cube.Draw(lightingShader);
        cube2.Draw(lightingShader);

        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilMask(0x00);
        glDisable(GL_DEPTH_TEST);
        singleColourShader.Use();
        glUniformMatrix4fv(transformLocMSimple, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(transformLocVSimple, 1, GL_FALSE, glm::value_ptr(view));
        cube.Draw(singleColourShader, 1.2f);
        cube2.Draw(singleColourShader, 1.2f);

        glStencilMask(0xFF);
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glEnable(GL_DEPTH_TEST);

        // This needs to be properly sorted in the general case
        cube4.Draw(lightingShader);
        cube3.Draw(lightingShader);

        SDL_GL_SwapWindow(window);
    }

    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}