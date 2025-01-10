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

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

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

    // Wifreframe
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    Shader shader("shaders/shader.vert", "shaders/shader.frag");
    Shader lightingShader("shaders/simple.vert", "shaders/lighting.frag");
    Shader lightSourceShader("shaders/simple.vert", "shaders/light.frag");

    float vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
    };
    GLuint VBO;
    glGenBuffers(1, &VBO);

    // Cube VAO
    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); 
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // Light VAO
    GLuint lightVAO;
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO); // Re-use the same VBO as the container cube
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Texture loading
    stbi_set_flip_vertically_on_load(true);  
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    int width, height, numChannels;
    unsigned char* data = stbi_load("textures/crate.png", &width, &height, &numChannels, 0);
    if (data == nullptr)
    {
        // TODO: Error here
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);

    // Specular map texture
    stbi_set_flip_vertically_on_load(true);  
    GLuint texture2;
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    data = stbi_load("textures/crate_specular.png", &width, &height, &numChannels, 0);
    if (data == nullptr)
    {
        // TODO: Error here
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);

    // Texture uniforms
    shader.Use();
    shader.SetInt("uTexture1", 0);
    shader.SetInt("uTexture2", 1);

    const glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
    Camera camera(glm::vec3(0.f, 0.f, 3.f));
    float deltaTime = 0.f;
    float lastFrame = 0.f;

    glm::mat4 projection = glm::perspective(glm::radians(camera.mZoom), 1280.f / 720.f, 0.1f, 100.f);
    shader.Use();
    GLuint transformLocV = glGetUniformLocation(shader.mShaderID, "view");
    GLuint transformLocP = glGetUniformLocation(shader.mShaderID, "projection");
    glUniformMatrix4fv(transformLocP, 1, GL_FALSE, glm::value_ptr(projection));
    lightingShader.Use();
    GLuint transformLocVLighting = glGetUniformLocation(lightingShader.mShaderID, "view");
    GLuint transformLocPLighting = glGetUniformLocation(lightingShader.mShaderID, "projection");
    glUniformMatrix4fv(transformLocPLighting, 1, GL_FALSE, glm::value_ptr(projection));
    lightSourceShader.Use();
    GLuint transformLocVSource = glGetUniformLocation(lightSourceShader.mShaderID, "view");
    GLuint transformLocPSource = glGetUniformLocation(lightSourceShader.mShaderID, "projection");
    glUniformMatrix4fv(transformLocPSource, 1, GL_FALSE, glm::value_ptr(projection));

    glm::vec3 pointLightPositions[] = {
        glm::vec3( 0.7f,  0.2f,  2.0f),
        glm::vec3( 2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f,  2.0f, -12.0f),
        glm::vec3( 0.0f,  0.0f, -3.0f)
    }; 

    // Set light parameters
    lightingShader.Use();
    lightingShader.SetInt("material.mDiffuse", 0);
    lightingShader.SetInt("material.mSpecular", 1);
    lightingShader.SetFloat("material.mShininess", 32.0f);

    lightingShader.SetVec3("directionalLight.mDirection", 0.f, -1.f, 0.f);
    lightingShader.SetVec3("directionalLight.mAmbient", 0.2f, 0.2f, 0.2f);
    lightingShader.SetVec3("directionalLight.mDiffuse", 0.5f, 0.5f, 0.5f);
    lightingShader.SetVec3("directionalLight.mSpecular", 1.f, 1.f, 1.f);
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
                shader.Use();
                glUniformMatrix4fv(transformLocP, 1, GL_FALSE, glm::value_ptr(projection));
                lightingShader.Use();
                glUniformMatrix4fv(transformLocPLighting, 1, GL_FALSE, glm::value_ptr(projection));
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
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Matrix transforms
        const float angle = static_cast<float>(SDL_GetTicks()) / 1000.f;
        GLuint transformLocM = glGetUniformLocation(shader.mShaderID, "model");
        GLuint transformLocMLighting = glGetUniformLocation(lightingShader.mShaderID, "model");
        GLuint transformLocMSource = glGetUniformLocation(lightSourceShader.mShaderID, "model");

        // Render the rectangle
        // shader.Use();
        lightingShader.Use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);
        glBindVertexArray(VAO);
        // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        // glDrawArrays(GL_TRIANGLES, 0, 36);

        glm::mat4 view = camera.GetViewMatrix();
        shader.Use();
        glUniformMatrix4fv(transformLocV, 1, GL_FALSE, glm::value_ptr(view));
        lightingShader.Use();
        glUniformMatrix4fv(transformLocVLighting, 1, GL_FALSE, glm::value_ptr(view));
        lightSourceShader.Use();
        glUniformMatrix4fv(transformLocVSource, 1, GL_FALSE, glm::value_ptr(view));

        // Draw cubes
        glm::mat4 model = glm::mat4(1.0f);
        // model = glm::translate(model, cubePositions[i]);
        // float angle = static_cast<float>(i) * 20.f;
        // model = glm::rotate(model, glm::radians(angle), glm::vec3(1.f, 0.3f, 0.5f));
        // glUniformMatrix4fv(transformLocM, 1, GL_FALSE, glm::value_ptr(model));
        lightingShader.Use();
        GLuint lightPosLoc = glGetUniformLocation(lightingShader.mShaderID, "lightPos");
        GLuint viewPosLoc = glGetUniformLocation(lightingShader.mShaderID, "viewPos");
        // glUniform3fv(lightPosLoc, 3, glm::value_ptr(lightPos));
        glUniform3f(lightPosLoc, lightPos.x, lightPos.y, lightPos.z);
        glUniform3f(viewPosLoc, camera.mPosition.x, camera.mPosition.y, camera.mPosition.z);
        lightingShader.SetVec3("spotLight.mPosition", camera.mPosition.x, camera.mPosition.y, camera.mPosition.z);
        lightingShader.SetVec3("spotLight.mDirection", camera.mFront.x, camera.mFront.y, camera.mFront.z);
        glUniformMatrix4fv(transformLocMLighting, 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // Draw lights
        lightSourceShader.Use();
        for (int i = 0; i < sizeof(pointLightPositions); ++i) 
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, pointLightPositions[i]);
            model = glm::scale(model, glm::vec3(0.2f));
            glUniformMatrix4fv(transformLocMSource, 1, GL_FALSE, glm::value_ptr(model));
            glBindVertexArray(lightVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        SDL_GL_SwapWindow(window);
    }

    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}