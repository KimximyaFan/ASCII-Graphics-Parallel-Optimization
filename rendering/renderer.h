#pragma once

#include <vector>
#include <memory> 
#include "geometry/primitive.h"
#include "math/matrix.h"
#include "material/color.h"
#include "geometry/projected_vertex.h"
#include "light_models/lighting_model.h"
#include "scene/scene.h"
#include "culling/clipper.h"
#include "material/texture.h"

class Renderer
{
public:
    static const Color clear_color;
    static const float clear_depth;

    Renderer (int width, int height);

    void ClearBuffers ();

    void SetLightingModel (std::unique_ptr<Lighting_Model> lighting);

    void DrawMesh (const std::vector<std::shared_ptr<Light>>& lights, 
                   const Vec3& camaera_pos,
                   const Vec3& ambient,
                   const Mesh& mesh,
                   const Clipper& clipper,
                   const Texture* texture,
                   Mat4x4 M, 
                   Mat4x4 V, 
                   Mat4x4 P);

    const std::vector<Color>& GetFrameBuffer () const;

    const std::vector<float>& GetZBuffer () const;

    void Render(const Scene& scene);

private:
    int width, height;
    Mat4x4 viewport_matrix;
    std::vector<Color> frame_buffer;
    std::vector<float> z_buffer;
    std::unique_ptr<Lighting_Model> lighting_model;

    Projected_Vertex ProjectVertex (const Vertex& v);

    inline float GetTriangleSpace (const Projected_Vertex& A, const Projected_Vertex& B, const Projected_Vertex& C);

    void RasterizeTriangle (const Vertex& v0, const Vertex& v1, const Vertex& v2, const Texture* texture);
};
