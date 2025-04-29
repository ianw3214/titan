#define NOMINMAX   
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <gl/glew.h>

#define SDL_MAIN_HANDLED 
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include "shader.h"
#include "fluid/simulation.h"

static float vertices[] = {
    -1.f, -1.f, 0.f,
    -1.f, 1.f, 0.f,
    1.f, 1.f, 0.f,
    -1.f, -1.f, 0.f,
    1.f, 1.f, 0.f,
    1.f, -1.f, 0.f
};

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
    // glEnable(GL_DEPTH_TEST); 
    glEnable(GL_STENCIL_TEST);
    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Wifreframe
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    Shader sphereShader("shaders/fluid/sphere.vert", "shaders/fluid/sphere.frag");
    sphereShader.SetFloat("screenWidth", 1280.f);
    sphereShader.SetFloat("screenHeight", 720.f);
    sphereShader.SetFloat("sphereSize", 0.1f);
    sphereShader.SetFloat("zoom", 50.f);

    Shader densityShader("shaders/fluid/sphere.vert", "shaders/fluid/sphere_density.frag");
    densityShader.SetFloat("screenWidth", 1280.f);
    densityShader.SetFloat("screenHeight", 720.f);
    densityShader.SetFloat("sphereSize", 1.2f);
    densityShader.SetFloat("zoom", 50.f);

    Shader pressureShader("shaders/fluid/pressure.vert", "shaders/fluid/pressure.frag");
    pressureShader.SetFloat("screenWidth", 1280.f);
    pressureShader.SetFloat("screenHeight", 720.f);
    pressureShader.SetFloat("zoom", 50.f);

    float deltaTime = 0.f;
    float lastFrame = 0.f;

    GLuint VAO;
    GLuint VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);
    glEnableVertexAttribArray(0);

    FluidSimulation simulation;
    simulation.SetBounds(12.f, 12.f);
    simulation.Initialize(1000);

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
            }
        }

        simulation.Update(deltaTime);

        /*
        for (size_t i = 0; i < simulation.GetPositions().size(); ++i)
        {
            const glm::vec2 pos = simulation.GetPositions()[i];
            const std::string name = std::string("particles[" + std::to_string(i) + ']');
            pressureShader.SetVec3(name, glm::vec3(pos.x, pos.y, 0.f));
        }
        */

        // Rendering here...
        glClearColor(0.01, 0.0f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );

        glBindVertexArray(VAO);
        // pressureShader.Use();
        // glDrawArrays(GL_TRIANGLES, 0, sizeof(vertices));    

        for (glm::vec2 position : simulation.GetPositions())
        {
            densityShader.Use();
            densityShader.SetVec3("spherePosition", glm::vec3(position.x, position.y, 0.f));
            glDrawArrays(GL_TRIANGLES, 0, sizeof(vertices));
        }

        for (glm::vec2 position : simulation.GetPositions())
        {
            sphereShader.Use();
            sphereShader.SetVec3("spherePosition", glm::vec3(position.x, position.y, 0.f));
            glDrawArrays(GL_TRIANGLES, 0, sizeof(vertices));
        }
        glBindVertexArray(0);

        SDL_GL_SwapWindow(window);
    }

    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}