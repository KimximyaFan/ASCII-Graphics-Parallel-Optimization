#pragma once

#include <memory>
#include "io/input_handler.h"
#include "math/vector.h"
#include "scene/camera.h"

class Camera_Controller
{
public:
    Camera_Controller(std::shared_ptr<Input_Handler> input, std::shared_ptr<Camera> camera);

    void SetSensitivity(float s);
    void SetSpeed(float walk);

    void Update(float dt);

private:
    std::shared_ptr<Input_Handler> input_handler;
    std::shared_ptr<Camera> camera;

    float sensitivity = 0.0025f;
    float walk_speed = 1.0f;

    float yaw = 0.0f;
    float pitch = 0.0f;
};