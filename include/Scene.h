#pragma once
#include <vector>
#include <memory>

#include "GameObject.h"

class Camera;

class Scene{
    private:
    std::vector<std::unique_ptr<GameObject>> objects;
    Camera* activeCamera = nullptr;

    public:
    Scene() = default;
    ~Scene() = default;

    template<typename T = GameObject, typename... Args>
        T& CreateObject(Args&&... args) {
        auto object = std::make_unique<T>(std::forward<Args>(args)...);
        object->scene = this;
        T& ref = *object;
        objects.push_back(std::move(object));
        return ref;
    }


    void RemoveObject(GameObject& obj){
        for(auto it = objects.begin(); it != objects.end();)
        {
            if(it->get() == &obj)  it = objects.erase(it);
            else ++it; 
        }
    }

    Camera& SetActiveCamera(Camera& camera){
        activeCamera = &camera;
        return camera;
    }

    const std::vector<std::unique_ptr<GameObject>>& GetObjects() const {
        return objects;
    }

    Camera* GetActiveCamera(){
        return activeCamera;
    }
    
    void Update(){
        for(auto& o : objects){
            if(!o->isActive) continue;
            o->Update();
        }
    }
};
