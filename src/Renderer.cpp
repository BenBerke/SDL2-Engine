#include <glad/glad.h>
#include <SDL2/SDL.h>
#include <iostream>
#include <vector>

#include "Renderer.h"
#include "shader.h"
#include "config.h"

#include "core/Matrix3x3.h"

#include "Components/SpriteRenderer.h"
#include "Components/Transform.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

static Shader* g_Shader = nullptr;
static unsigned int g_VAO = 0, g_VBO = 0, g_EBO = 0;

bool InitWindow(SDL_Window*& window, SDL_GLContext& glContext)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
        return false;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    window = SDL_CreateWindow(
        "OpenGL Renderer",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH, SCREEN_HEIGHT,
        SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI
    );
    if (!window) {
        std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << std::endl;
        return false;
    }

    glContext = SDL_GL_CreateContext(window);
    if (!glContext) {
        std::cerr << "SDL_GL_CreateContext failed: " << SDL_GetError() << std::endl;
        return false;
    }

    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        return false;
    }

    SDL_GL_SetSwapInterval(1);
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    // === SHADER SETUP ===
    g_Shader = new Shader("src/shaders/basic.vert", "src/shaders/basic.frag");
    g_Shader->use();

    // === TRIANGLE VERTICES ===
    float vertices[] = {
        // positions        // colors
         0.0f,  0.5f, 0.0f,  1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,
         0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f
    };

    // === INDICES (EBO) ===
    unsigned int indices[] = {
        0, 1, 2  // draw a single triangle
    };


    glGenVertexArrays(1, &g_VAO);
    glGenBuffers(1, &g_VBO);
    glBindVertexArray(g_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, g_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &g_EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
    return true;
}

void DestroyWindow(SDL_Window* window, SDL_GLContext glContext)
{
    glDeleteVertexArrays(1, &g_VAO);
    glDeleteBuffers(1, &g_VBO);
    glDeleteBuffers(1, &g_EBO);
    delete g_Shader;

    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

static std::vector<SpriteRenderer*> sprites;

void Update(const Scene& scene){
    const auto& objects = scene.GetObjects();
    for (const auto& obj : objects){
        if(!obj || !obj->isActive) continue;
        if(auto collider = obj->GetComponent<SpriteRenderer>()){
            sprites.push_back(collider);
        }
    }

    for(SpriteRenderer* sr : sprites){
        Transform* t = sr->owner->GetComponent<Transform>();
        if (!t) continue;

        Matrix3x3 model = t->GetMatrix();
        int loc = glGetUniformLocation(g_Shader->ID, "uTransform");
        glUniformMatrix3fv(loc, 1, GL_FALSE, &model.m[0][0]);

        glBindVertexArray(g_VAO);
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
    }
}
