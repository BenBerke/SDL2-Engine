#include <glad/glad.h>
#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <filesystem>

#include "Renderer.h"
#include "shader.h"
#include "config.h"
#include "core/Matrix3x3.h"

#include "Components/SpriteRenderer.h"
#include "Components/Transform.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

static Shader* g_Shader = nullptr;

namespace Renderer{
struct ShapeData {
    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;
    int indexCount;
};

static std::unordered_map<ShapeType, ShapeData> g_Shapes;
static ShapeData g_TexturedQuad;

ShapeData CreateTriangleShape() {
    ShapeData shape;
    float s = (PIXELS_PER_UNIT * 2.0f) / SCREEN_WIDTH;
    float h = s * sqrtf(3.0f) / 2.0f;

    float vertices[] = {
        0.0f,    2.0f/3.0f * h, 0.0f,  1.0f, 1.0f, 1.0f,
       -s/2.0f, -1.0f/3.0f * h, 0.0f,  1.0f, 1.0f, 1.0f,
        s/2.0f, -1.0f/3.0f * h, 0.0f,  1.0f, 1.0f, 1.0f
    };
    unsigned int indices[] = { 0, 1, 2 };

    glGenVertexArrays(1, &shape.VAO);
    glGenBuffers(1, &shape.VBO);
    glGenBuffers(1, &shape.EBO);

    glBindVertexArray(shape.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, shape.VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shape.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    shape.indexCount = 3;
    return shape;
}

ShapeData CreateSquareShape() {
    ShapeData shape;
    float s = (PIXELS_PER_UNIT * 2.0f) / SCREEN_WIDTH;
    float aspect = (float)SCREEN_WIDTH / SCREEN_HEIGHT;
    float half = s / 2.0f;

    float vertices[] = {
        -half,  half * aspect, 0.0f,  1.0f, 1.0f, 1.0f,
         half,  half * aspect, 0.0f,  1.0f, 1.0f, 1.0f,
         half, -half * aspect, 0.0f,  1.0f, 1.0f, 1.0f,
        -half, -half * aspect, 0.0f,  1.0f, 1.0f, 1.0f
    };

    unsigned int indices[] = { 0, 1, 2, 2, 3, 0 };

    glGenVertexArrays(1, &shape.VAO);
    glGenBuffers(1, &shape.VBO);
    glGenBuffers(1, &shape.EBO);

    glBindVertexArray(shape.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, shape.VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shape.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    shape.indexCount = 6;
    return shape;
}

ShapeData CreateTexturedQuad() {
    ShapeData shape;

    float s = (PIXELS_PER_UNIT * 2.0f) / SCREEN_WIDTH;
    float half = s / 2.0f;

    float vertices[] = {
        // positions       // colors        // texcoords
        -half,  half, 0.0f,  1,1,1,   0.0f, 1.0f,
         half,  half, 0.0f,  1,1,1,   1.0f, 1.0f,
         half, -half, 0.0f,  1,1,1,   1.0f, 0.0f,
        -half, -half, 0.0f,  1,1,1,   0.0f, 0.0f
    };

    unsigned int indices[] = { 0, 1, 2, 2, 3, 0 };

    glGenVertexArrays(1, &shape.VAO);
    glGenBuffers(1, &shape.VBO);
    glGenBuffers(1, &shape.EBO);

    glBindVertexArray(shape.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, shape.VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shape.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // positions (3)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // colors (3)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texcoords (2)
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    shape.indexCount = 6;
    return shape;
}


bool InitWindow(SDL_Window*& window, SDL_GLContext& glContext)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
        return false;
    }

    std::cout << "Working directory: " << std::filesystem::current_path() << std::endl;

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

    g_Shader = new Shader("src/shaders/basic.vert", "src/shaders/basic.frag");
    g_Shader->use();

    g_Shapes[ShapeType::Triangle] = CreateTriangleShape();
    g_Shapes[ShapeType::Square]   = CreateSquareShape();
    g_TexturedQuad = CreateTexturedQuad();

    return true;
}

int GenerateTexture(const char* imagePath){
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    int width, height, nrChannels;
    unsigned char *data = stbi_load(imagePath, &width, &height, &nrChannels, 0);
    if(data){
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else std::cout << "Failed to load texture" << std::endl;
    stbi_image_free(data);
    return texture;
}

void DestroyWindow(SDL_Window* window, SDL_GLContext glContext)
{
    for (auto& [shapeType, shape] : g_Shapes) {
        glDeleteVertexArrays(1, &shape.VAO);
        glDeleteBuffers(1, &shape.VBO);
        glDeleteBuffers(1, &shape.EBO);
    }

    glDeleteVertexArrays(1, &g_TexturedQuad.VAO);
    glDeleteBuffers(1, &g_TexturedQuad.VBO);
    glDeleteBuffers(1, &g_TexturedQuad.EBO);

    delete g_Shader;

    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

struct Triangle{
    Vector2 position;
    float r, g, b;
};

static std::vector<Triangle> triangles;
void CreateTriangle(Vector2 pos, float r, float g, float b)
{
    Triangle t;
    t.position = pos;
    t.r = r;
    t.g = g;
    t.b = b;
    triangles.push_back(t);
}

static std::vector<SpriteRenderer*> sprites;

void Update(const Scene& scene){
    glClearColor(.25f, .25f, .25f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    sprites.clear();          

    const auto& objects = scene.GetObjects();
    for (const auto& obj : objects){
        if(!obj || !obj->isActive) continue;
        if(auto collider = obj->GetComponent<SpriteRenderer>()){
            sprites.push_back(collider);
        }
    }

    for (auto* sprite : sprites) {
        Transform* t = sprite->owner->GetComponent<Transform>();
        if (!t) continue;

        int loc = glGetUniformLocation(g_Shader->ID, "uTransform");
        float cosR = std::cos(t->rotation);
        float sinR = std::sin(t->rotation);
        float sx = t->scale.x, sy = t->scale.y;
        float x = (t->position.x * PIXELS_PER_UNIT) / (SCREEN_WIDTH  / 2.0f);
        float y = (t->position.y * PIXELS_PER_UNIT) / (SCREEN_HEIGHT / 2.0f);

        float transform[9] = {
            cosR * sx, -sinR * sy, 0.0f,
            sinR * sx,  cosR * sy, 0.0f,
            x, y, 1.0f
        };
        glUniformMatrix3fv(loc, 1, GL_FALSE, transform);

        if (sprite->HasTexture()) {
            glUniform1i(glGetUniformLocation(g_Shader->ID, "uHasTexture"), true);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, sprite->textureID);

            glBindVertexArray(g_TexturedQuad.VAO);
            glDrawElements(GL_TRIANGLES, g_TexturedQuad.indexCount, GL_UNSIGNED_INT, 0);
        } 
        else {
            glUniform1i(glGetUniformLocation(g_Shader->ID, "uHasTexture"), false);
            glUniform3f(glGetUniformLocation(g_Shader->ID, "overrideColor"),
            sprite->color.x, sprite->color.y, sprite->color.z);

            ShapeData& sdata = g_Shapes[sprite->shape];
            glBindVertexArray(sdata.VAO);
            glDrawElements(GL_TRIANGLES, sdata.indexCount, GL_UNSIGNED_INT, 0);
        }

    }

}

}//namespace renderer
