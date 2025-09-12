#include "scene/scene.h"
#include "scene/camera.h"
#include "scene/entity.h"
#include "scene/light_manager.h"
#include "math/vector.h"
#include "light/directional_light.h"
#include "light_models/blinn_phong.h"
#include "fps_counter/fps_counter.h"
#include "io/input_handler.h"
#include "io/output_handler.h"
#include "rendering/renderer.h"
#include "preprocess/test.h"
#include "preprocess/texture_register.h"
#include "world/camera_controller.h"
#include "world/world.h"
#include "world/test_world.h"
#include "thread/thread_pool.h"

int main(int argc, char* argv[])
{
    const int width  = 120;
    const int height = 30;
    const int tile_w = 10;
    const int tile_h = 5;

    const int number_of_thread = 2;
    const int q_capacity = 1024;

    Scene scene;

    auto input_handler = std::make_shared<Input_Handler>(nullptr, true);

    auto texture_manager_ptr = std::make_shared<Texture_Manager>();
    TextureRegisterPreprocess(*texture_manager_ptr);
    scene.SetTextureManager(texture_manager_ptr);

    Vec3 camera_pos = Vec3(5.5f, 5.5f, 5.5f);
    Vec3 camera_target = Vec3(0.0f, 0.0f, 0.0f);
    Vec3 camera_up = Vec3(0.0f, 1.0, 0.0f);
    float fov = 45.0f;
    float cell_aspect = 0.5f;
    float aspect = (float)width / (float)height * cell_aspect;
    float near_plane = -0.1f;
    float far_plane = -50.0f;
 
    auto camera = std::make_shared<Camera>(
        camera_pos,
        camera_target,
        camera_up,
        fov,
        aspect,
        near_plane,
        far_plane 
    );

    auto camera_controller = std::make_shared<Camera_Controller>(input_handler, camera);
    
    scene.SetCamera(camera);

    scene.GetLightManager()->SetAmbient(Vec3(0.30f, 0.30f, 0.30f));
  
    auto key_light = std::make_shared<Directional_Light>(Vec3(50,  100, 50), 1.0f);

    scene.GetLightManager()->AddLight(key_light);

    /*
    std::shared_ptr<Entity> entity;
    
    if (true) 
        entity = CreateCubeEntity_Flat24_Fixed(5.0f);
    else
        entity = CreateCubeEntity_Fla t24_Debug(5.0f);

    scene.AddEntity(entity);
    */
     
    std::unique_ptr<World> world = std::make_unique<Test_World>();
    world->SetWorld();
    auto world_info = world->GetWorldInfo();

    for (const std::shared_ptr<Entity>& entity : world_info )
    {
        scene.AddEntity(entity);
    }

    camera->SetPosition(world_info[0]->transform.position);
    
    Thread_Pool thread_pool(number_of_thread, q_capacity);

    Renderer renderer(width, height, tile_w, tile_h, thread_pool);
    renderer.SetLightingModel(std::make_unique<Blinn_Phong>());

    Output_Handler output_handler(width, height);

    Fps_Counter fps_counter;
    fps_counter.Start();  

    auto lastTime = std::chrono::high_resolution_clock::now();

    while ( true )
    {
        auto now   = std::chrono::high_resolution_clock::now();
        float dt   = std::chrono::duration<float>(now - lastTime).count();
        lastTime   = now;

        input_handler->Poll();
        if (input_handler->IsKeyDown(Key::SPACE)) break;
        camera_controller->Update(dt);

        renderer.Render(scene);
        output_handler.PrintBuffer(renderer.GetFrameBuffer(), fps_counter.Get_Fps());
    }

    thread_pool.Stop();
}