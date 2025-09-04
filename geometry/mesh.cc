
#include "mesh.h"

void Mesh::ComputeLocalAABB()
{
    local_aabb = AABB();

    for (auto& v : vertices)
        local_aabb.Expand( Vec3( v.position.x, v.position.y, v.position.z ) );
}

const AABB& Mesh::GetLocalAABB()
{
    return local_aabb;
}