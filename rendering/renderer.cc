
#include "renderer.h"
#include "culling/clipper.h"
#include <algorithm>
#include <iostream>

const Color Renderer::clear_color = {0, 0, 0, 0};
const float Renderer::clear_depth = 1.0f;

Renderer::Renderer (int w, int h)
    : width(w), height(h),
    frame_buffer(w*h),
    z_buffer(w*h) 
    { viewport_matrix = Mat4x4::ViewportTransformation(0, w, 0, h); }

const std::vector<Color>& Renderer::GetFrameBuffer () const
{
    return frame_buffer;
}

const std::vector<float>& Renderer::GetZBuffer () const
{
    return z_buffer;
}

void Renderer::ClearBuffers ()
{
    std::fill(frame_buffer.begin(), frame_buffer.end(), clear_color);
    std::fill(z_buffer.begin(), z_buffer.end(), clear_depth);
}

void Renderer::SetLightingModel(std::unique_ptr<Lighting_Model> lighting)
{
    lighting_model = std::move(lighting);
}


Projected_Vertex Renderer::ProjectVertex (const Vertex& v)
{
    
    // Clip Space -> NDC
    float invW = 1.0f / v.position.w;
    float ndcX = v.position.x * invW;
    float ndcY = v.position.y * invW;
    float ndcZ = v.position.z * invW;
    
    Projected_Vertex out;
    out.invW = invW;
    out.x = (ndcX*0.5f + 0.5f) * width;
    // 배열의 y축은 위에서 아래로 가기때문에, y를 뒤집어줘야함, height - y 
    out.y = (1.0f - (ndcY*0.5f + 0.5f)) * height;
    out.z = ndcZ*0.5f + 0.5f;

    out.r_over_w = v.color.r * invW;
    out.g_over_w = v.color.g * invW;
    out.b_over_w = v.color.b * invW;
    out.u_over_w = v.uv.x * invW;
    out.v_over_w = v.uv.y * invW;

    return out;
}

inline float Renderer::GetTriangleSpace (const Projected_Vertex& A,
                                        const Projected_Vertex& B,
                                        const Projected_Vertex& C)
{
    return (B.y-A.y)*(C.x-A.x) - (B.x-A.x)*(C.y-A.y);
}

/*
    Barycentric Interpolation
    
    easy to implement

    if all weights are positive, current point is in triangle
*/

void Renderer::RasterizeTriangle (const Vertex& v0, const Vertex& v1, const Vertex& v2, const Texture* texture)
{
    Projected_Vertex A = ProjectVertex(v0);
    Projected_Vertex B = ProjectVertex(v1);
    Projected_Vertex C = ProjectVertex(v2);
    Projected_Vertex P;

    int x_min = std::max(0, (int)std::floor(std::min({A.x, B.x, C.x})));
    int x_max = std::min(width-1, (int)std::ceil(std::max({A.x, B.x, C.x})));
    int y_min = std::max(0, (int)std::floor(std::min({A.y, B.y, C.y})));
    int y_max = std::min(height-1, (int)std::ceil(std::max({A.y, B.y, C.y})));

    float area = GetTriangleSpace(A, B, C);

    if (fabs(area) < 1e-6f) return;

    float orient = area > 0.0f ? 1.0f : -1.0f;
    area *= orient;

    float inv_area = 1.0f/area;

    for (int y=y_min; y<=y_max; ++y)
    {
        P.y = y+0.5f;

        for (int x=x_min; x<=x_max; ++x)
        {
            P.x = x+0.5f;

            float w0 = orient * GetTriangleSpace(B, C, P) * inv_area;
            float w1 = orient * GetTriangleSpace(C, A, P) * inv_area;
            float w2 = orient * GetTriangleSpace(A, B, P) * inv_area;

            if ( w0 < 0.0f || w1 < 0.0f || w2 < 0.0f)
                continue;

            float z = w0*A.z + w1*B.z + w2*C.z;

            int index = y*width + x;
            
            if ( z >= z_buffer[index] ) continue;

            float denom = w0*A.invW + w1*B.invW + w2*C.invW;

            if (!std::isfinite(denom) || std::fabs(denom) < 1e-12f) continue;
            
            float u = (w0*A.u_over_w + w1*B.u_over_w + w2*C.u_over_w) / denom;
            float v = (w0*A.v_over_w + w1*B.v_over_w + w2*C.v_over_w) / denom;

            Color tex = texture ? texture->Sample(u, v) : Color{1,1,1,1};
            
            Color col;
            col.r = (w0*A.r_over_w + w1*B.r_over_w + w2*C.r_over_w) / denom;
            col.g = (w0*A.g_over_w + w1*B.g_over_w + w2*C.g_over_w) / denom;
            col.b = (w0*A.b_over_w + w1*B.b_over_w + w2*C.b_over_w) / denom;
            col.a = 1.0f;

            Color out = col * tex;
            
            z_buffer[index] = z;
            frame_buffer[index] = out;
        }
    }
}


/*

조명·컬링·노멀 같은 “뷰 공간 로직”은 MV를, 
클립·래스터 같은 “투영 이후 로직”은 MVP를 씁니다. 
필요한 처리에 따라 둘 다 갖고 있는 게 가장 유연

M = MC to WC
V = WC to VC
P = VC to projection 
*/

void Renderer::DrawMesh (const std::vector<std::shared_ptr<Light>>& lights, 
                         const Vec3& camera_pos,
                         const Vec3& ambient,
                         const Mesh& mesh,
                         const Clipper& clipper,
                         const Texture* texture, 
                         Mat4x4 M, 
                         Mat4x4 V, 
                         Mat4x4 P
                        )
{
    Mat4x4 PV = P * V;
    Mat3x3 InverseTranspose_M = M.TopLeft3x3().InverseTranspose(); 
    Mesh out_mesh = mesh;

    // MC to WC
    for (size_t i=0; i<out_mesh.vertices.size(); ++i)
    {
        out_mesh.vertices[i].position = M * out_mesh.vertices[i].position;
        out_mesh.vertices[i].normal = Vec3::Normalize( InverseTranspose_M * out_mesh.vertices[i].normal );
    }

    out_mesh = clipper.BackFaceRemoval(out_mesh, V);

    //out_mesh = clipper.BackFaceRemoval(out_mesh, view_direction);
    //out_mesh = clipper.BackFaceRemoval2(out_mesh, V);

    // Illumniation
    for (size_t i=0; i<out_mesh.vertices.size(); ++i)
    {
        out_mesh.vertices[i].color = lighting_model->Shade(
            out_mesh.vertices[i].position.ToVec3(),
            out_mesh.vertices[i].normal,
            camera_pos - out_mesh.vertices[i].position.ToVec3(),
            *mesh.material,
            lights,
            out_mesh.vertices[i].color,
            ambient
        );
    }

    // Clipping
    Mesh transformed_mesh = clipper.ClipMesh(out_mesh);

    //Projection
    for (auto& v : transformed_mesh.vertices)
        v.position = PV * v.position;
 
    for (size_t i=0; i+2<transformed_mesh.indices.size(); i+=3) 
    {
        RasterizeTriangle(transformed_mesh.vertices[transformed_mesh.indices[i+0]], 
                          transformed_mesh.vertices[transformed_mesh.indices[i+1]], 
                          transformed_mesh.vertices[transformed_mesh.indices[i+2]],
                          texture);
    }
}

void Renderer::Render(const Scene& scene)
{
    ClearBuffers();

    std::vector<std::shared_ptr<Light>> lights = scene.GetLightManager()->GetLights();

    Vec3 ambient = scene.GetLightManager()->GetAmbient();

    std::shared_ptr<Camera> camera = scene.GetCamera();

    Mat4x4 V = camera->GetViewMatrix();

    Mat4x4 P = camera->GetProjMatrix();

    Vec3 view_direction = camera->GetViewDirection();

    Vec3 camera_pos = camera->GetPosition();

    Clipper clipper;
    clipper.ExtractFrustumPlanes(P*V);

    // AABB Culling
    for ( auto& e : scene.GetEntities() )
    {
        AABB world_aabb = e->GetLocalAABB().MatrixConversion(e->GetLocalToWorldMatrix());

        if ( clipper.IsAABBVisible(world_aabb) == false )
            continue;

        for ( auto& mesh : e->parts )
        {
            DrawMesh(lights, camera_pos, ambient, mesh, clipper, scene.GetTextureManager()->GetTexture(mesh.material->texture_handle), e->GetLocalToWorldMatrix(), V, P);
        }
    }
}



/*
    Parallel
*/

Projected_Triangle Renderer::DrawMesh_Parallel (const std::vector<std::shared_ptr<Light>>& lights, 
                         const Vec3& camera_pos,
                         const Vec3& ambient,
                         const Mesh& mesh,
                         const Clipper& clipper,
                         const Texture* texture, 
                         Mat4x4 M, 
                         Mat4x4 V, 
                         Mat4x4 P
                        )
{
    Mat4x4 PV = P * V;
    Mat3x3 InverseTranspose_M = M.TopLeft3x3().InverseTranspose(); 
    Mesh out_mesh = mesh;

    // MC to WC
    for (size_t i=0; i<out_mesh.vertices.size(); ++i)
    {
        out_mesh.vertices[i].position = M * out_mesh.vertices[i].position;
        out_mesh.vertices[i].normal = Vec3::Normalize( InverseTranspose_M * out_mesh.vertices[i].normal );
    }

    out_mesh = clipper.BackFaceRemoval(out_mesh, V);

    // Illumniation
    for (size_t i=0; i<out_mesh.vertices.size(); ++i)
    {
        out_mesh.vertices[i].color = lighting_model->Shade(
            out_mesh.vertices[i].position.ToVec3(),
            out_mesh.vertices[i].normal,
            camera_pos - out_mesh.vertices[i].position.ToVec3(),
            *mesh.material,
            lights,
            out_mesh.vertices[i].color,
            ambient
        );
    }

    // Clipping
    Mesh transformed_mesh = clipper.ClipMesh(out_mesh);

    //Projection
    for (auto& v : transformed_mesh.vertices)
        v.position = PV * v.position;
 
    for (size_t i=0; i+2<transformed_mesh.indices.size(); i+=3) 
    {
        RasterizeTriangle(transformed_mesh.vertices[transformed_mesh.indices[i+0]], 
                          transformed_mesh.vertices[transformed_mesh.indices[i+1]], 
                          transformed_mesh.vertices[transformed_mesh.indices[i+2]],
                          texture);
    }
}

void Renderer::Render_Parallel(const Scene& scene, int thread_count)
{
    ClearBuffers();

    std::vector<std::shared_ptr<Light>> lights = scene.GetLightManager()->GetLights();

    Vec3 ambient = scene.GetLightManager()->GetAmbient();

    std::shared_ptr<Camera> camera = scene.GetCamera();

    Mat4x4 V = camera->GetViewMatrix();

    Mat4x4 P = camera->GetProjMatrix();

    Vec3 view_direction = camera->GetViewDirection();

    Vec3 camera_pos = camera->GetPosition();

    Clipper clipper;
    clipper.ExtractFrustumPlanes(P*V);

    // AABB Culling
    for ( auto& e : scene.GetEntities() )
    {
        AABB world_aabb = e->GetLocalAABB().MatrixConversion(e->GetLocalToWorldMatrix());

        if ( clipper.IsAABBVisible(world_aabb) == false )
            continue;

        for ( auto& mesh : e->parts )
        {
            DrawMesh(lights, camera_pos, ambient, mesh, clipper, scene.GetTextureManager()->GetTexture(mesh.material->texture_handle), e->GetLocalToWorldMatrix(), V, P);
        }
    }
}