#pragma once

class GameObject;

class Component{
    public:
    Component() = default;
    virtual ~Component() {}

    GameObject* owner = nullptr;

    virtual void Start() {}

    virtual void Update() {}
};