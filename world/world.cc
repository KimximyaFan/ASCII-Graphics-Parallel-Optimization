#include "world.h"
#include "preprocess/test.h"

std::vector<std::shared_ptr<Entity>> World::GetWorldInfo()
{
    const float cellSize    = 1.0f;   // 셀 한 변
    const float wallYScale  = 2.0f;   // 벽 높이 3배
    const float floorYScale = 0.10f;  // 바닥 두께(납작하게)
    const float hs          = cellSize * 0.5f;
    const float eps         = 1e-4f * cellSize;

    std::vector<std::shared_ptr<Entity>> out;
    out.reserve(static_cast<size_t>(width * height) + 2);

    // 0) 바닥: 전체 맵 크기만큼 스케일, CHECKER 텍스처
    {
        auto floor = CreateCubeEntity_Flat24_Fixed(cellSize, 1.0f, 1.0f); // shared_ptr<Entity>

        // 스케일을 맵 크기에 맞춤 (X=width, Z=height / Y는 얇게)
        floor->transform.scale = Vec3(static_cast<float>(width),
                                      floorYScale,
                                      static_cast<float>(height));

        // 상면이 y=0 바로 아래에 오도록 (z-fighting 방지용 eps)
        const float halfH = cellSize * floorYScale * 0.5f;
        floor->transform.position = Vec3(width  * cellSize * 0.5f,
                                         -halfH - eps,
                                         height * cellSize * 0.5f);

        // 텍스처를 CHECKER로 변경
        if (!floor->parts.empty() && floor->parts[0].material) {
            floor->parts[0].material->texture_handle = Texture_Handle::CHECKER;
        }

        floor->ComputeLocalAABB();
        out.push_back(std::move(floor));
    }

    // 1) 시작 지점(메쉬 없음) — 벡터 맨 앞에 삽입 예정
    std::shared_ptr<Entity> startMarker; // 없으면 nullptr 유지

    // 2) 격자 순회: 벽/시작지점 처리
    for (int zy = 0; zy < height; ++zy)
    {
        for (int zx = 0; zx < width; ++zx)
        {
            const Object t = world_map[zy * width + zx];

            if (t == Object::W) // 벽
            {
                auto e = CreateCubeEntity_Flat24_Fixed(cellSize, 1.0f, 2.0f);

                // 세로만 3배
                e->transform.scale.y *= wallYScale;

                // 바닥(y=0)에 앉도록 중심 y를 절반 높이만큼 올림
                e->transform.position = Vec3(zx * cellSize + hs,
                                             (cellSize * wallYScale) * 0.5f,
                                             zy * cellSize + hs);

                e->ComputeLocalAABB();
                out.push_back(std::move(e));
            }
            else if (t == Object::s) // 시작 지점
            {
                if (!startMarker)
                {
                    startMarker = std::make_shared<Entity>(); // 빈 엔티티
                    startMarker->parts.clear();
                    startMarker->transform.position = Vec3(zx * cellSize + hs,
                                                           0.0f,
                                                           zy * cellSize + hs);
                }
            }
            // 그 외는 빈 공간 → 아무것도 생성 X
        }
    }

    // 3) 시작 지점을 맨 앞에 삽입
    if (startMarker) {
        out.insert(out.begin(), std::move(startMarker));
    }

    return out;
}

