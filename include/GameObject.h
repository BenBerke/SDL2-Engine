#pragma once
#include <vector>
#include <memory>
#include <utility>
#include "Component.h"

class Scene;

class GameObject{
    private:
        std::vector<std::unique_ptr<Component>> components;
    public:
        bool isActive = true;
        Scene* scene = nullptr;

        GameObject(Scene* parentScene = nullptr);
        ~GameObject() = default;

        template<typename T, typename... Args>
        T& AddComponent(Args&&... args) {
            auto comp = std::make_unique<T>(std::forward<Args>(args)...);
            comp->OnAttach(this);
            T& ref = *comp;
            components.push_back(std::move(comp));
            return ref;
        }

        template<typename T>
        T* GetComponent() {
            for (auto& c : components) {
                if (auto casted = dynamic_cast<T*>(c.get())) return casted;
            }
            return nullptr;
        }

        template<typename T>
        std::vector<T*> GetComponents() {
            std::vector<T*> matches;
            for (auto& c : components) {
                if (auto casted = dynamic_cast<T*>(c.get())) matches.push_back(casted);
            }
            return matches;
        }

        void Update() {
        for (auto& c : components)
            c->Update();  
        }
};
