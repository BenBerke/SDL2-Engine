#pragma once
#include "Component.h"
#include "core/Vector2.h"
#include "config.h"
#include "Scene.h"

#include "Camera.h"

class Transform : public Component{
private:
    Vector2 screenPosition{0.0f, 0.0f};
public:
    Vector2 position;
    Vector2 scale;
    float rotation;

    Transform(Vector2 p = Vector2{0, 0}) : screenPosition(p), position(p), scale{1.0f, 1.0f}, rotation(0.0f) {}

    void Update() override {
        Camera* activeCamera = owner->scene->GetActiveCamera();
        Transform* cameraTransform = activeCamera ? activeCamera->owner->GetComponent<Transform>() : nullptr;
        const float camX = cameraTransform ? cameraTransform->position.x : 0.0f;
        const float camY = cameraTransform ? cameraTransform->position.y : 0.0f;

        screenPosition = Vector2{
            SCREEN_WIDTH_CENTER + (position.x - camX) * PIXELS_PER_UNIT,
            SCREEN_HEIGHT_CENTRE - (position.y - camY) * PIXELS_PER_UNIT
        };
    }

    Vector2 GetScreenPosition() const {
        return screenPosition;
    }

    void Translate(Vector2 pos){
        position = pos;
    }
};
