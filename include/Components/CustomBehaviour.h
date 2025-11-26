#pragma once
#include "Component.h"
#include "Components/Collider.h"

class CustomBehaviour : public Component{
public:
virtual void Start();
virtual void Update();
virtual void OnCollisionEnter([[maybe_unused]] Collider* other) {}
virtual void OnCollisionStay([[maybe_unused]] Collider* other) {}
virtual void OnCollisionExit([[maybe_unused]] Collider* other) {}
virtual ~CustomBehaviour() = default;
};
