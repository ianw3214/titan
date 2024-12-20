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

    float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };
    /*
    unsigned int indices[] = {
        0, 1, 3,
        1, 2, 3
    };
    */
    glm::vec3 cubePositions[] = {
        glm::vec3( 0.0f,  0.0f,  0.0f), 
        glm::vec3( 2.0f,  5.0f, -15.0f), 
        glm::vec3(-1.5f, -2.2f, -2.5f),  
        glm::vec3(-3.8f, -2.0f, -12.3f),  
        glm::vec3( 2.4f, -0.4f, -3.5f),  
        glm::vec3(-1.7f,  3.0f, -7.5f),  
        glm::vec3( 1.3f, -2.0f, -2.5f),  
        glm::vec3( 1.5f,  2.0f, -2.5f), 
        glm::vec3( 1.5f,  0.2f, -1.5f), 
        glm::vec3(-1.3f,  1.0f, -1.5f)  
    };
    GLuint VBO;
    glGenBuffers(1, &VBO);
    // GLuint EBO;
    // glGenBuffers(1, &EBO);

    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); 
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

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
    unsigned char* data = stbi_load("textures/default.png", &width, &height, &numChannels, 0);
    if (data == nullptr)
    {
        // TODO: Error here
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);

    // Texture 2
    GLuint texture2;
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    data = stbi_load("textures/knob.png", &width, &height, &numChannels, 0);
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

    Camera camera(glm::vec3(0.f, 0.f, 3.f));
    float deltaTime = 0.f;
    float lastFrame = 0.f;

    // glm::mat4 view = glm::mat4(1.0f);
    // view = glm::translate(view, glm::vec3(0.f, 0.f, -3.0f));
    glm::mat4 projection = glm::perspective(glm::radians(camera.mZoom), 1280.f / 720.f, 0.1f, 100.f);
    GLuint transformLocV = glGetUniformLocation(shader.mShaderID, "view");
    // glUniformMatrix4fv(transformLocV, 1, GL_FALSE, glm::value_ptr(view));
    GLuint transformLocP = glGetUniformLocation(shader.mShaderID, "projection");
    glUniformMatrix4fv(transformLocP, 1, GL_FALSE, glm::value_ptr(projection));

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
                glUniformMatrix4fv(transformLocP, 1, GL_FALSE, glm::value_ptr(projection));
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
        // glm::mat4 model = glm::mat4(1.0f);
        // model = glm::rotate(model, angle, glm::vec3(0.5f, 1.f, 0.f));
        GLuint transformLocM = glGetUniformLocation(shader.mShaderID, "model");
        // glUniformMatrix4fv(transformLocM, 1, GL_FALSE, glm::value_ptr(model));

        // Render the rectangle
        shader.Use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);
        glBindVertexArray(VAO);
        // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        // glDrawArrays(GL_TRIANGLES, 0, 36);

        glm::mat4 view = camera.GetViewMatrix();
        glUniformMatrix4fv(transformLocV, 1, GL_FALSE, glm::value_ptr(view));
        for (unsigned int i = 0; i < 10; ++i)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            float angle = static_cast<float>(i) * 20.f;
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.f, 0.3f, 0.5f));
            glUniformMatrix4fv(transformLocM, 1, GL_FALSE, glm::value_ptr(model));
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        SDL_GL_SwapWindow(window);
    }

    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}