
#include "entity.h"

void Entity::Update(float delta)
{
    
}

void Entity::ComputeLocalAABB()
{
    local_aabb = AABB();

    for (auto& part : parts)
    {
        part.ComputeLocalAABB();
        local_aabb = AABB::Union(local_aabb, part.GetLocalAABB());
    }
}

const AABB& Entity::GetLocalAABB()
{
    return local_aabb;
}

Mat4x4 Entity::GetLocalToWorldMatrix() const
{
    return transform.GetMatrix();
}