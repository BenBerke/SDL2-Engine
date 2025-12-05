// SpriteRenderer.h
#pragma once
#include "Component.h"
#include "core/Vector.h"
#include <string>

namespace Renderer {
    int GenerateTexture(const char* imagePath);
}

enum class ShapeType {
    None,
    Triangle,
    Square,
    Circle
};

class SpriteRenderer : public Component {
public:
    unsigned int textureID = 0;
    bool hasTexture = false;
    ShapeType shape = ShapeType::Square;
    Vector3 color = {1.0f, 1.0f, 1.0f}; 

    std::string texturePath; 

    void SetTexture(const std::string& path) {
        texturePath = path;
        textureID = Renderer::GenerateTexture(path.c_str());
        hasTexture = (textureID != 0);
    }

    bool HasTexture() const { return hasTexture; }

    void SetColor(float r, float g, float b) { color = {r,g,b}; }
};
