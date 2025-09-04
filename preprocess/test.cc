#include "test.h"

std::shared_ptr<Entity> CreateCubeEntity_Flat24_Fixed(float size, float uv_scale_u, float uv_scale_v)
{
    float hs = size * 0.5f; // half size
    const Color faceColor = {0.5f, 0.5f, 0.5f, 0.0f}; // per-vertex color (optional tint)

    auto V4 = [&](float x, float y, float z) { return Vec4{x, y, z, 1.0f}; };
    auto N3 = [&](float x, float y, float z) { return Vec3{x, y, z}; };

    // 위치→UV 변환 (면 법선에 따라 투영 평면 선택)
    // 변경점: 6면 모두 UV 와인딩이 CCW가 되도록 ±X/±Y 플립 규칙 정렬
    auto UVof = [&](const Vec4& p, const Vec3& n) -> Vec2 {
        float u, v;
        if (fabsf(n.z) > 0.5f) {             // ±Z → (X,Y)
            u = (p.x/hs + 1)*0.5f;
            v = (p.y/hs + 1)*0.5f;
            if (n.z < 0) u = 1.0f - u;       // -Z만 u flip
        } else if (fabsf(n.x) > 0.5f) {      // ±X → (Z,Y)
            u = (p.z/hs + 1)*0.5f;
            v = (p.y/hs + 1)*0.5f;
            if (n.x > 0) u = 1.0f - u;       // **+X만 u flip** (기존과 반대)
        } else {                              // ±Y → (X,Z)
            u = (p.x/hs + 1)*0.5f;
            v = (p.z/hs + 1)*0.5f;
            if (n.y > 0) v = 1.0f - v;       // **+Y만 v flip** (기존과 반대)
        }
        // 경계에서 u,v==1.0이 딱 떨어져 wrap과 충돌하면 미세심 줄 수 있음
        const float one_minus_eps = std::nextafter(1.0f, 0.0f);
        u = std::clamp(u, 0.0f, one_minus_eps);
        v = std::clamp(v, 0.0f, one_minus_eps);
        return { u * uv_scale_u, v * uv_scale_v };
    };

    std::vector<Vertex>   vertices; vertices.reserve(24);
    std::vector<uint32_t> indices;  indices.reserve(36);

    // a,b,c,d: 바깥에서 볼 때 CCW, n: 면 법선
    // 변경점: alt_diag=true면 대각선을 1-3으로 사용(기본은 0-2)
    auto addFace = [&](const Vec4& a, const Vec4& b, const Vec4& c, const Vec4& d, const Vec3& n, bool alt_diag=false) {
        uint32_t base = static_cast<uint32_t>(vertices.size());
        const Vec2 ua = UVof(a,n);
        const Vec2 ub = UVof(b,n);
        const Vec2 uc = UVof(c,n);
        const Vec2 ud = UVof(d,n);

        vertices.push_back(Vertex{ a, n, ua, faceColor }); // 0
        vertices.push_back(Vertex{ b, n, ub, faceColor }); // 1
        vertices.push_back(Vertex{ c, n, uc, faceColor }); // 2
        vertices.push_back(Vertex{ d, n, ud, faceColor }); // 3

        if (!alt_diag) {
            // 기본: 대각선 0-2
            indices.push_back(base + 0);
            indices.push_back(base + 1);
            indices.push_back(base + 2);
            indices.push_back(base + 0);
            indices.push_back(base + 2);
            indices.push_back(base + 3);
        } else {
            // 대각선 1-3 (옆면 한쪽만 좌/우대각 통일용)
            indices.push_back(base + 0);
            indices.push_back(base + 1);
            indices.push_back(base + 3);
            indices.push_back(base + 1);
            indices.push_back(base + 2);
            indices.push_back(base + 3);
        }
    };

    // 8 모서리 좌표
    Vec4 v000 = V4(-hs, -hs, -hs);
    Vec4 v100 = V4( hs, -hs, -hs);
    Vec4 v110 = V4( hs,  hs, -hs);
    Vec4 v010 = V4(-hs,  hs, -hs);
    Vec4 v001 = V4(-hs, -hs,  hs);
    Vec4 v101 = V4( hs, -hs,  hs);
    Vec4 v111 = V4( hs,  hs,  hs);
    Vec4 v011 = V4(-hs,  hs,  hs);

    // 각 면 추가 (바깥에서 볼 때 CCW)
    // -Z (front)  normal = (0,0,-1)
    addFace(/*a*/ v010, /*b*/ v110, /*c*/ v100, /*d*/ v000, N3(0, 0, -1));
    // +Z (back)   normal = (0,0,+1)
    addFace(/*a*/ v001, /*b*/ v101, /*c*/ v111, /*d*/ v011, N3(0, 0, +1));
    // -X (left)   normal = (-1,0,0)
    addFace(/*a*/ v000, /*b*/ v001, /*c*/ v011, /*d*/ v010, N3(-1, 0, 0));
    // +X (right)  normal = (+1,0,0)
    // 변경점: 옆면 중 "한 면"의 대각선이 반대라면 여기 alt_diag=true로 통일
    addFace(/*a*/ v100, /*b*/ v110, /*c*/ v111, /*d*/ v101, N3(+1, 0, 0), /*alt_diag=*/true);
    // -Y (bottom) normal = (0,-1,0)
    addFace(/*a*/ v000, /*b*/ v100, /*c*/ v101, /*d*/ v001, N3(0, -1, 0));
    // +Y (top)    normal = (0,+1,0)
    addFace(/*a*/ v010, /*b*/ v011, /*c*/ v111, /*d*/ v110, N3(0, +1, 0));

    // Mesh 구성
    Mesh mesh;
    mesh.vertices = std::move(vertices);
    mesh.indices  = std::move(indices);

    // 머티리얼 (BRICK 텍스처 사용)
    auto mat = std::make_shared<Material>();
    mat->ambient_coef  = 1.0f;
    mat->diffuse_coef  = 1.0f;
    mat->specular_coef = 0.1f;
    mat->shininess     = 0.5f;
    mat->texture_handle = Texture_Handle::BRICK; // ← 여기!
    mesh.material = mat;

    // 엔티티
    auto entity = std::make_shared<Entity>();
    entity->parts.push_back(std::move(mesh));
    entity->transform.position = Vec3(0, 0.0f, 0);
    entity->transform.scale    = Vec3(1.0f, 1.0f, 1.0f);
    entity->ComputeLocalAABB();
    return entity;
}


std::shared_ptr<Entity> CreateCubeEntity_Flat24(float size = 1.0f)
{
    float hs = size * 0.5f; // half size
    const Color faceColor = {0.5f, 0.5f, 0.5f, 0.0f}; // per-vertex color (optional tint)
    const float uv_scale_u = 1.0f; // 필요 시 타일링 배수 조절
    const float uv_scale_v = 1.0f;

    auto V4 = [&](float x, float y, float z) { return Vec4{x, y, z, 1.0f}; };
    auto N3 = [&](float x, float y, float z) { return Vec3{x, y, z}; };

    auto UVof = [&](const Vec4& p, const Vec3& n) -> Vec2 {
    float u, v;
        if (fabsf(n.z) > 0.5f) {             // ±Z → (X,Y)
            u = (p.x/hs + 1)*0.5f;
            v = (p.y/hs + 1)*0.5f;
            if (n.z < 0) u = 1.0f - u;       // -Z 면 좌우 뒤집기(선택)
        } else if (fabsf(n.x) > 0.5f) {      // ±X → (Z,Y)
            u = (p.z/hs + 1)*0.5f;
            v = (p.y/hs + 1)*0.5f;
            if (n.x < 0) u = 1.0f - u;       // -X 면 좌우 뒤집기(선택)
        } else {                              // ±Y → (X,Z)
            u = (p.x/hs + 1)*0.5f;
            v = (p.z/hs + 1)*0.5f;
            if (n.y < 0) v = 1.0f - v;       // -Y 면 상하 뒤집기(선택)
        }
        // 경계에서 u,v==1.0이 딱 떨어져 wrap과 충돌하면 미세심 줄 수 있음
        const float one_minus_eps = std::nextafter(1.0f, 0.0f);
        u = std::clamp(u, 0.0f, one_minus_eps);
        v = std::clamp(v, 0.0f, one_minus_eps);
        return { u * uv_scale_u, v * uv_scale_v };
    };

    std::vector<Vertex>   vertices; vertices.reserve(24);
    std::vector<uint32_t> indices;  indices.reserve(36);

    // a,b,c,d: 바깥에서 볼 때 CCW, n: 면 법선
    auto addFace = [&](const Vec4& a, const Vec4& b, const Vec4& c, const Vec4& d, const Vec3& n) {
        uint32_t base = static_cast<uint32_t>(vertices.size());
        vertices.push_back(Vertex{ a, n, UVof(a,n), faceColor }); // 0
        vertices.push_back(Vertex{ b, n, UVof(b,n), faceColor }); // 1
        vertices.push_back(Vertex{ c, n, UVof(c,n), faceColor }); // 2
        vertices.push_back(Vertex{ d, n, UVof(d,n), faceColor }); // 3
        // 두 개의 삼각형
        indices.push_back(base + 0);
        indices.push_back(base + 1);
        indices.push_back(base + 2);
        indices.push_back(base + 0);
        indices.push_back(base + 2);
        indices.push_back(base + 3);
    };

    // 8 모서리 좌표
    Vec4 v000 = V4(-hs, -hs, -hs);
    Vec4 v100 = V4( hs, -hs, -hs);
    Vec4 v110 = V4( hs,  hs, -hs);
    Vec4 v010 = V4(-hs,  hs, -hs);
    Vec4 v001 = V4(-hs, -hs,  hs);
    Vec4 v101 = V4( hs, -hs,  hs);
    Vec4 v111 = V4( hs,  hs,  hs);
    Vec4 v011 = V4(-hs,  hs,  hs);

    // -Z (front)  normal = (0,0,-1)
    addFace(/*a*/ v010, /*b*/ v110, /*c*/ v100, /*d*/ v000, N3(0, 0, -1));
    // +Z (back)   normal = (0,0,+1)
    addFace(/*a*/ v001, /*b*/ v101, /*c*/ v111, /*d*/ v011, N3(0, 0, +1));
    // -X (left)   normal = (-1,0,0)
    addFace(/*a*/ v000, /*b*/ v001, /*c*/ v011, /*d*/ v010, N3(-1, 0, 0));
    // +X (right)  normal = (+1,0,0)
    addFace(/*a*/ v100, /*b*/ v110, /*c*/ v111, /*d*/ v101, N3(+1, 0, 0));
    // -Y (bottom) normal = (0,-1,0)
    addFace(/*a*/ v000, /*b*/ v100, /*c*/ v101, /*d*/ v001, N3(0, -1, 0));
    // +Y (top)    normal = (0,+1,0)
    addFace(/*a*/ v010, /*b*/ v011, /*c*/ v111, /*d*/ v110, N3(0, +1, 0));

    // Mesh 구성
    Mesh mesh;
    mesh.vertices = std::move(vertices);
    mesh.indices  = std::move(indices);

    // 머티리얼 (BRICK 텍스처 사용)
    auto mat = std::make_shared<Material>();
    mat->ambient_coef  = 1.0f;
    mat->diffuse_coef  = 1.0f;
    mat->specular_coef = 0.1f;
    mat->shininess     = 0.5f;
    mat->texture_handle = Texture_Handle::CHECKER; // ← 여기!
    mesh.material = mat;

    // 엔티티
    auto entity = std::make_shared<Entity>();
    entity->parts.push_back(std::move(mesh));
    entity->transform.position = Vec3(0, 0.0f, 0);
    entity->transform.scale    = Vec3(1.0f, 1.0f, 1.0f);
    entity->ComputeLocalAABB();
    return entity;
}


// 그대로 복붙해서 쓰세요. (필요시 include는 중복되어도 안전)
// UV 디버그 출력/컬러라이즈는 아래 두 상수로 온/오프합니다.


std::shared_ptr<Entity> CreateCubeEntity_Flat24_Debug(float size = 1.0f)
{
    // === 디버그 토글 ===
    constexpr bool DEBUG_UV_PRINT     = true; // 콘솔에 면별 UV 정보 출력
    constexpr bool DEBUG_UV_COLORIZE  = false; // vertex color를 (R=u, G=v)로 칠해 화면에서 시각화

    float hs = size * 0.5f; // half size
    const Color faceColor = {0.5f, 0.5f, 0.5f, 0.0f}; // per-vertex color (optional tint)
    const float uv_scale_u = 1.0f; // 필요 시 타일링 배수 조절
    const float uv_scale_v = 1.0f;

    auto V4 = [&](float x, float y, float z) { return Vec4{x, y, z, 1.0f}; };
    auto N3 = [&](float x, float y, float z) { return Vec3{x, y, z}; };

    // 위치→UV 변환 (면 법선에 따라 투영 평면 선택)
    auto UVof = [&](const Vec4& p, const Vec3& n) -> Vec2 {
        float u = 0.0f, v = 0.0f;
        if (std::fabs(n.z) > 0.5f) {             // ±Z → (X,Y)
            u = (p.x/hs + 1)*0.5f;
            v = (p.y/hs + 1)*0.5f;
            if (n.z < 0) u = 1.0f - u;           // -Z 면 좌우 뒤집기(선택)
        } else if (std::fabs(n.x) > 0.5f) {      // ±X → (Z,Y)
            u = (p.z/hs + 1)*0.5f;
            v = (p.y/hs + 1)*0.5f;
            if (n.x < 0) u = 1.0f - u;           // -X 면 좌우 뒤집기(선택)
        } else {                                  // ±Y → (X,Z)
            u = (p.x/hs + 1)*0.5f;
            v = (p.z/hs + 1)*0.5f;
            if (n.y < 0) v = 1.0f - v;           // -Y 면 상하 뒤집기(선택)
        }
        // 경계에서 u,v==1.0이 딱 떨어져 wrap과 충돌하면 미세심 줄 수 있음
        const float one_minus_eps = std::nextafter(1.0f, 0.0f);
        u = std::clamp(u, 0.0f, one_minus_eps);
        v = std::clamp(v, 0.0f, one_minus_eps);
        return { u * uv_scale_u, v * uv_scale_v };
    };

    // === UV 디버깅 헬퍼들 ===
    auto FaceTag = [&](const Vec3& n) -> const char* {
        if (n.z <= -0.5f) return "-Z";
        if (n.z >=  0.5f) return "+Z";
        if (n.x <= -0.5f) return "-X";
        if (n.x >=  0.5f) return "+X";
        if (n.y <= -0.5f) return "-Y";
        return "+Y";
    };
    auto area2_uv = [&](const Vec2& p, const Vec2& q, const Vec2& r) -> float {
        // 2D 삼각형 부호있는 면적*2 (CCW면 +)
        return (q.x - p.x)*(r.y - p.y) - (q.y - p.y)*(r.x - p.x);
    };
    auto uv_range = [&](const Vec2& a, const Vec2& b, const Vec2& c, const Vec2& d) {
        float umin = std::min(std::min(a.x, b.x), std::min(c.x, d.x));
        float umax = std::max(std::max(a.x, b.x), std::max(c.x, d.x));
        float vmin = std::min(std::min(a.y, b.y), std::min(c.y, d.y));
        float vmax = std::max(std::max(a.y, b.y), std::max(c.y, d.y));
        return std::tuple<float,float,float,float>(umin,umax,vmin,vmax);
    };

    std::vector<Vertex>   vertices; vertices.reserve(24);
    std::vector<uint32_t> indices;  indices.reserve(36);

    // a,b,c,d: 바깥에서 볼 때 CCW, n: 면 법선
    auto addFace = [&](const Vec4& a, const Vec4& b, const Vec4& c, const Vec4& d, const Vec3& n) {
        const Vec2 ua = UVof(a,n);
        const Vec2 ub = UVof(b,n);
        const Vec2 uc = UVof(c,n);
        const Vec2 ud = UVof(d,n);

        uint32_t base = static_cast<uint32_t>(vertices.size());

        if constexpr (DEBUG_UV_COLORIZE) {
            // UV를 색으로 착색: R=u, G=v (B=0)
            Color ca{ua.x, ua.y, 0.0f, 0.0f};
            Color cb{ub.x, ub.y, 0.0f, 0.0f};
            Color cc{uc.x, uc.y, 0.0f, 0.0f};
            Color cd{ud.x, ud.y, 0.0f, 0.0f};
            vertices.push_back(Vertex{ a, n, ua, ca }); // 0
            vertices.push_back(Vertex{ b, n, ub, cb }); // 1
            vertices.push_back(Vertex{ c, n, uc, cc }); // 2
            vertices.push_back(Vertex{ d, n, ud, cd }); // 3
        } else {
            vertices.push_back(Vertex{ a, n, ua, faceColor }); // 0
            vertices.push_back(Vertex{ b, n, ub, faceColor }); // 1
            vertices.push_back(Vertex{ c, n, uc, faceColor }); // 2
            vertices.push_back(Vertex{ d, n, ud, faceColor }); // 3
        }

        // 두 개의 삼각형
        indices.push_back(base + 0);
        indices.push_back(base + 1);
        indices.push_back(base + 2);
        indices.push_back(base + 0);
        indices.push_back(base + 2);
        indices.push_back(base + 3);

        if constexpr (DEBUG_UV_PRINT) {
            const float Auv = area2_uv(ua, ub, uc) + area2_uv(ua, uc, ud); // 사각형을 두 삼각형으로
            const bool ccw = (Auv > 0.0f);

            auto [umin, umax, vmin, vmax] = uv_range(ua, ub, uc, ud);
            const bool out_of_range =
                (umin < 0.0f) || (umax > std::nextafter(1.0f, 0.0f)) ||
                (vmin < 0.0f) || (vmax > std::nextafter(1.0f, 0.0f));

            std::printf("[UV] Face %s  n=(%+.0f,%+.0f,%+.0f)  UV-area=%+.6f (%s)\n",
                        FaceTag(n), n.x, n.y, n.z, Auv, ccw ? "CCW" : "CW");
            std::printf("     A P=(% .3f,% .3f,% .3f) → UV=(%.6f,%.6f)\n", a.x, a.y, a.z, ua.x, ua.y);
            std::printf("     B P=(% .3f,% .3f,% .3f) → UV=(%.6f,%.6f)\n", b.x, b.y, b.z, ub.x, ub.y);
            std::printf("     C P=(% .3f,% .3f,% .3f) → UV=(%.6f,%.6f)\n", c.x, c.y, c.z, uc.x, uc.y);
            std::printf("     D P=(% .3f,% .3f,% .3f) → UV=(%.6f,%.6f)\n", d.x, d.y, d.z, ud.x, ud.y);
            std::printf("     range: u=[%.6f, %.6f], v=[%.6f, %.6f]%s\n",
                        umin, umax, vmin, vmax, out_of_range ? "  **OUT OF [0,1)**" : "");
        }
    };

    // 8 모서리 좌표
    Vec4 v000 = V4(-hs, -hs, -hs);
    Vec4 v100 = V4( hs, -hs, -hs);
    Vec4 v110 = V4( hs,  hs, -hs);
    Vec4 v010 = V4(-hs,  hs, -hs);
    Vec4 v001 = V4(-hs, -hs,  hs);
    Vec4 v101 = V4( hs, -hs,  hs);
    Vec4 v111 = V4( hs,  hs,  hs);
    Vec4 v011 = V4(-hs,  hs,  hs);

    // 각 면 추가 (바깥에서 볼 때 CCW)
    // -Z (front)  normal = (0,0,-1)
    addFace(/*a*/ v010, /*b*/ v110, /*c*/ v100, /*d*/ v000, N3(0, 0, -1));
    // +Z (back)   normal = (0,0,+1)
    addFace(/*a*/ v001, /*b*/ v101, /*c*/ v111, /*d*/ v011, N3(0, 0, +1));
    // -X (left)   normal = (-1,0,0)
    addFace(/*a*/ v000, /*b*/ v001, /*c*/ v011, /*d*/ v010, N3(-1, 0, 0));
    // +X (right)  normal = (+1,0,0)
    addFace(/*a*/ v100, /*b*/ v110, /*c*/ v111, /*d*/ v101, N3(+1, 0, 0));
    // -Y (bottom) normal = (0,-1,0)
    addFace(/*a*/ v000, /*b*/ v100, /*c*/ v101, /*d*/ v001, N3(0, -1, 0));
    // +Y (top)    normal = (0,+1,0)
    addFace(/*a*/ v010, /*b*/ v011, /*c*/ v111, /*d*/ v110, N3(0, +1, 0));

    // Mesh 구성
    Mesh mesh;
    mesh.vertices = std::move(vertices);
    mesh.indices  = std::move(indices);

    // 머티리얼 (예: CHECKER 텍스처 사용)
    auto mat = std::make_shared<Material>();
    mat->ambient_coef   = 1.0f;
    mat->diffuse_coef   = 1.0f;
    mat->specular_coef  = 0.1f;
    mat->shininess      = 0.5f;
    mat->texture_handle = Texture_Handle::CHECKER;
    mesh.material = mat;

    // 엔티티
    auto entity = std::make_shared<Entity>();
    entity->parts.push_back(std::move(mesh));
    entity->transform.position = Vec3(0, 0.0f, 0);
    entity->transform.scale    = Vec3(1.0f, 1.0f, 1.0f);
    entity->ComputeLocalAABB();
    return entity;
}
