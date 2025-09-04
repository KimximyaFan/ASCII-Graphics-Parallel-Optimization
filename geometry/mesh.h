#pragma once

#include <vector>
#include <cstdint>
#include <memory>
#include "vertex.h"
#include "material/material.h"
#include "aabb.h"

/*
    std::vector<uint32_t>  indices;
    폴리곤을 각각 vertices의 인덱스 3개 로 본다
    인간 입장에서는 난해하지만
    gpu programming을 염두해둔다면 이런 설계가 효율적이다다
*/
class Mesh 
{
public:
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    std::shared_ptr<Material> material;

    void ComputeLocalAABB();
    const AABB& GetLocalAABB();
private:
    AABB local_aabb;
};