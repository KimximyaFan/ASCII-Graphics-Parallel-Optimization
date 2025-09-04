#pragma once

#include "light/light.h"
#include <vector>
#include <memory>

class Light_Manager
{
public:
    Light_Manager() = default;
    ~Light_Manager() = default;

    void AddLight(const std::shared_ptr<Light>& light);
    void RemoveLight(const std::shared_ptr<Light>& light);
    void RemoveLightById(int id);
    void ClearLights();
    const std::vector<std::shared_ptr<Light>>& GetLights() const;
    void UpdateLights(float delta_time);

    void SetAmbient(const Vec3& c);
    const Vec3& GetAmbient() const;
private:
    std::vector<std::shared_ptr<Light>> lights;
    Vec3 ambient { 0.01f, 0.01f, 0.01f };
};