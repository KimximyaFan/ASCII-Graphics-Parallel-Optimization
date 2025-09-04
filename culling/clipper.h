
#pragma once

#include <array>
#include <vector>
#include "geometry/primitive.h"
#include "math/vector.h"
#include "math/matrix.h"


/*
    그래픽스 파이프라인에서는 투영 행렬(Projection Matrix) 곱셈 후, 
    아직 w로 나누기 전의 “클립 공간(Clip Space)” 상태에서 클리핑이 이뤄집니다.

    그 다음에 퍼스펙티브 디바이드(perspective divide, 
    𝑥/𝑤, 𝑦/𝑤, 𝑧/𝑤x/w,y/w,z/w)를 수행해 NDC(정규화 디바이스 좌표)로 맵핑한 뒤, 
    뷰포트 변환과 스크리소 검사(scissor test)·깊이 테스트(depth test)를 거칩니다.

    input -> mesh
    output -> clipped mesh 

    frustum_planes 를 쓰는 이유?
    -> 내외 판별은 h로만 해도 되지만, intersect vertex 를 구하려면 결국 plane이 필요함
*/

class Clipper 
{
public:
    Clipper () {};

    const std::array<Vec4, 6>& GetFrustumPlanes();

    Mesh ClipMesh(const Mesh& mesh) const;

    bool IsAABBVisible(const AABB& box) const;

    void ExtractFrustumPlanes(const Mat4x4& proj_view);

    Mesh BackFaceRemoval(const Mesh& in, const Mat4x4& V) const;

private:
    std::array<Vec4, 6> frustum_planes;

    static bool IsVertexInside(const Vertex& vertex, const Vec4& plane);

    static Vertex GetIntersectVertex(const Vec4& plane, const Vertex& a, const Vertex& b);

    static std::vector<Vertex> SutherlandHodgman(const std::vector<Vertex>& poly, const Vec4& plane);
};
