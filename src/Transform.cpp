#include "pch.h"

#include "Components/Transform.h"

#include "Components/Camera.h"
#include "GameObject.h"
#include "Scene.h"
#include "config.h"

Transform::~Transform() = default;

void Transform::Update() {
    Camera* activeCamera = owner && owner->scene ? owner->scene->GetActiveCamera() : nullptr;
    Transform* cameraTransform = activeCamera ? activeCamera->owner->GetComponent<Transform>() : nullptr;
    const float camX = cameraTransform ? cameraTransform->position.x : 0.0f;
    const float camY = cameraTransform ? cameraTransform->position.y : 0.0f;

    screenPosition = Vector2{
        SCREEN_WIDTH_CENTER + (position.x - camX) * PIXELS_PER_UNIT,
        SCREEN_HEIGHT_CENTRE - (position.y - camY) * PIXELS_PER_UNIT
    };
}
