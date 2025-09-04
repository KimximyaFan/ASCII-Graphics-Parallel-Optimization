#pragma once

#include <vector>
#include <memory>
#include "entity.h"
#include "light_manager.h"
#include "camera.h"
#include "material/texture_manager.h"

class Scene
{
public:
    Scene();
    ~Scene() = default;

    /*
    AddEntity(const Entity& entity)
    같은 메서드에서 entities.push_back(entity); 하면 값 복사로 들어갑니다.
    */
    void AddEntity(const std::shared_ptr<Entity>& entity);
    void RemoveEntity(const std::shared_ptr<Entity>& entity);
    void ClearEntities();
    const std::vector<std::shared_ptr<Entity>>& GetEntities() const;
    const std::shared_ptr<Light_Manager>& GetLightManager() const;
    void SetCamera(const std::shared_ptr<Camera>& camera);
    const std::shared_ptr<Camera>& GetCamera() const;
    void Update(float delta_time);
    void SetTextureManager(const std::shared_ptr<Texture_Manager>& texture_manager);
    const std::shared_ptr<Texture_Manager>& GetTextureManager() const;

private:
    std::vector<std::shared_ptr<Entity>> entities;
    std::shared_ptr<Light_Manager> light_manager;
    std::shared_ptr<Camera> camera;
    std::shared_ptr<Texture_Manager> texture_manager;
};