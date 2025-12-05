#pragma once
#include <cmath>
#include "Component.h"
#include "core/Vector.h"
#include "core/Matrix3x3.h"

class Transform : public Component {
private:
    Vector2 screenPosition{0.0f, 0.0f};

public:
    Vector2 position;
    Vector2 scale;
    float rotation;

    Transform(Vector2 p = Vector2{0, 0}, float r = 0)
        : screenPosition(p), position(p), scale{1.0f, 1.0f}, rotation(r) {}

    virtual ~Transform();

    void Update() override;

    Vector2 GetScreenPosition() const { return screenPosition; }

    void Translate(Vector2 pos) { position = pos; }

    Matrix3x3 GetMatrix() const {
        float cosR = std::cos(rotation);
        float sinR = std::sin(rotation);

        Matrix3x3 mat;
        mat.m[0][0] = cosR * scale.x;
        mat.m[0][1] = -sinR * scale.y;
        mat.m[0][2] = position.x;
        mat.m[1][0] = sinR * scale.x;
        mat.m[1][1] = cosR * scale.y;
        mat.m[1][2] = position.y;
        mat.m[2][0] = 0.0f;
        mat.m[2][1] = 0.0f;
        mat.m[2][2] = 1.0f;
        return mat;
    }
};

