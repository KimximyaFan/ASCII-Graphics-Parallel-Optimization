#include "camera_controller.h"
#include <algorithm>

Camera_Controller::Camera_Controller(std::shared_ptr<Input_Handler> input, std::shared_ptr<Camera> cam)
: input_handler(input), camera(cam)
{
    if (!camera)
        return;

    Vec3 view_dir = camera->GetViewDirection();

    yaw = std::atan2(view_dir.x, view_dir.z);
    pitch = std::clamp( std::asin(std::clamp(view_dir.y, -1.0f, 1.0f)), -1.55f, 1.55f);
}

void Camera_Controller::SetSensitivity(float s)
{
    sensitivity = s;
}

void Camera_Controller::SetSpeed(float spd)
{
    walk_speed = spd;
}

void Camera_Controller::Update(float dt)
{
    if (!input_handler || !camera) return;

    auto mouse_delta = input_handler->GetMouseDelta();
    yaw -= mouse_delta.dx * sensitivity;
    pitch -= mouse_delta.dy * sensitivity;
    pitch = std::clamp(pitch, -1.55f, 1.55f);

    const Vec3 forward
    {
        -std::sin(yaw) * std::cos(pitch),
        std::sin(pitch),
        -std::cos(yaw) * std::cos(pitch)
    };

    Vec3 worldUp = {0, 1.0f, 0};
    Vec3 right = Vec3::Normalize(Vec3::Cross(forward, worldUp));

    float fwd = 0.0f, strafe = 0.0f, levitate = 0.0f;
    
    if (input_handler->IsKeyDown(Key::W)) fwd += 1;
    if (input_handler->IsKeyDown(Key::S)) fwd -= 1;
    if (input_handler->IsKeyDown(Key::D)) strafe += 1;
    if (input_handler->IsKeyDown(Key::A)) strafe -= 1;
    if (input_handler->IsKeyDown(Key::Q)) levitate += 1;
    if (input_handler->IsKeyDown(Key::E)) levitate -= 1;
    if (input_handler->IsKeyDown(Key::LEFTSHIFT)) input_handler->PointerLockToggle();

    Vec3 delta = forward*fwd + right*strafe + worldUp*levitate;
    delta *= walk_speed * dt;
    
    if ( delta.Length() > 1e-6f )
        camera->SetPosition(camera->GetPosition() + delta);
        
    camera->SetTarget(camera->GetPosition() + forward);
}