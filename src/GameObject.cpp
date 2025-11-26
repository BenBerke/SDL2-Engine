#include "GameObject.h"
#include "Components/Transform.h"

GameObject::GameObject(Scene* parentScene) : scene(parentScene) {
    AddComponent<Transform>();
}
