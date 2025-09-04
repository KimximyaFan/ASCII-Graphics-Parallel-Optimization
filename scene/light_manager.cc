
#include "light_manager.h"
#include <algorithm>

void Light_Manager::AddLight(const std::shared_ptr<Light>& light)
{
    lights.push_back(light);
}

void Light_Manager::RemoveLight(const std::shared_ptr<Light>& light)
{   
    auto iter = std::find(lights.begin(), lights.end(), light);

    if ( iter != lights.end() )
        lights.erase(iter);
}

void Light_Manager::RemoveLightById(int id)
{
    for (size_t i = 0; i < lights.size(); ++i)
    {
        if (lights[i]->GetId() == id)
        {
            lights.erase(lights.begin() + i);
            break;
        }
    }
}

void Light_Manager::ClearLights()
{
    lights.clear();
}

const std::vector<std::shared_ptr<Light>>& Light_Manager::GetLights() const
{
    return lights;
}

void Light_Manager::UpdateLights(float delta_time)
{
    /*
    임시
    for (auto& light : lights_) {
        light->Update(deltaTime);
    }
    */
}

void Light_Manager::SetAmbient(const Vec3& c)
{
    ambient = c;
}

const Vec3& Light_Manager::GetAmbient() const
{
    return ambient;
}