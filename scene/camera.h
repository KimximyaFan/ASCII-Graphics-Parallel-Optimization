#pragma once

#include "math/vector.h"
#include "math/matrix.h"

class Camera
{
public:
    Camera(const Vec3& position, 
           const Vec3& target, 
           const Vec3& up,
           float fov_y_deg,
           float aspect,
           float z_near,
           float z_far);

    /*
    
    Mat4x4 GetProjMatrix() const;
    Mat4x4 GetVPMatrix() const { return GetProjMatrix() * GetViewMatrix(); }
    */
    
    Vec3 GetPosition() const { return position; }
    Vec3 GetTarget() const { return target; }
    Vec3 GetUp() const { return up; }
    Vec3 GetViewDirection() const;
    
    Mat4x4 GetViewMatrix() const;
    Mat4x4 GetProjMatrix() const;
    
    void SetPosition(const Vec3& pos_vec) { position = pos_vec; }
    void SetTarget(const Vec3& tar_vec) { target = tar_vec; }
    void SetUp(const Vec3& up_vec) { up = up_vec; }
    void SetPerspective(float fov, float aspct, float z_near, float z_far);

private:
    Vec3 position;
    Vec3 target;
    Vec3 up;
    float fov_y_deg;
    float aspect;
    float z_near;
    float z_far;
};