#pragma once

#include <vector>
#include "scene/entity.h"
#include <memory>

enum Object
{
    z,
    s,
    W
};

class World
{
public:
    virtual ~World() = default;

    virtual void SetWorld() = 0;
    std::vector<std::shared_ptr<Entity>> GetWorldInfo();
protected:
    int width, height;
    std::vector<Object> world_map;
};
