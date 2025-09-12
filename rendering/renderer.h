#pragma once

#include <vector>
#include <memory> 
#include "geometry/primitive.h"
#include "math/matrix.h"
#include "material/color.h"
#include "geometry/rendering_struct.h"
#include "light_models/lighting_model.h"
#include "scene/scene.h"
#include "culling/clipper.h"
#include "material/texture.h"
#include "thread/thread_pool.h"

class Renderer
{
public:
    static const Color clear_color;
    static const float clear_depth;

    Renderer (int width, int height, int tile_w, int tile_h);
    Renderer (int width, int height, int tile_w, int tile_h, Thread_Pool* pool);

    void ClearBuffers ();

    void SetLightingModel (std::unique_ptr<Lighting_Model> lighting);

    const std::vector<Color>& GetFrameBuffer () const;

    const std::vector<float>& GetZBuffer () const;

    void Render(const Scene& scene);

private:
    int width, height, tile_w, tile_h, tile_grid_width, tile_grid_height;
    Mat4x4 viewport_matrix;
    std::vector<Color> frame_buffer;
    std::vector<float> z_buffer;
    Thread_Pool* thread_pool;
    std::unique_ptr<Lighting_Model> lighting_model;
    std::vector<Tile> tiles;
    std::vector<std::vector<Triangle_Reference>> tile_bins;
    std::vector<std::vector<Projected_Triangle>> triangles;
    std::vector<std::shared_ptr<Entity>> draw_list;

    Projected_Vertex ProjectVertex (const Vertex& v);

    inline float GetTriangleSpace (const Projected_Vertex& A, const Projected_Vertex& B, const Projected_Vertex& C);

    void RasterizeTriangle (const Vertex& v0, const Vertex& v1, const Vertex& v2, const Texture* texture);

    void DrawMesh (const std::vector<std::shared_ptr<Light>>& lights, 
                   const Vec3& camaera_pos,
                   const Vec3& ambient,
                   const Mesh& mesh,
                   const Clipper& clipper,
                   const Texture* texture,
                   Mat4x4 M, 
                   Mat4x4 V, 
                   Mat4x4 P);

    void MakeTiles(int tile_w, int tile_h);

    Bounding_Box GetTriangleBoundingBox (const Projected_Triangle& triangle);

    void TriangleIntoTileBin(const Projected_Triangle& triangle, int tid, int index);

    void AllocateTriangle();

    void DrawTile(const Tile& t);

    void DrawMesh_Parallel (const std::vector<std::shared_ptr<Light>>& lights, 
                   const Vec3& camaera_pos,
                   const Vec3& ambient,
                   const Mesh& mesh,
                   const Clipper& clipper,
                   const Texture* texture,
                   Mat4x4 M, 
                   Mat4x4 V, 
                   Mat4x4 P,
                   size_t tid );

    static void DrawMeshJob(void* ctx, size_t tid);
    static void DrawTileJob(void* ctx, size_t tid);

    void Render_Single(const Scene& scene);
    void Render_Parallel(const Scene& scene);
};
