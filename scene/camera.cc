#include "camera.h"

Camera::Camera(const Vec3& pos_vec,
               const Vec3& target_vec,
               const Vec3& up_vec,
               float fov,
               float aspct,
               float near,
               float far)
    : position(pos_vec),
      target(target_vec),
      up(up_vec),
      fov_y_deg(fov),
      aspect(aspct),
      z_near(near),
      z_far(far)
{}

Mat4x4 Camera::GetViewMatrix() const
{
    return Mat4x4::LookAt(position, target, up);
}

Mat4x4 Camera::GetProjMatrix() const
{
    float fov_y_rad = fov_y_deg * (3.1415926f / 180.0f);
    return Mat4x4::Perspective(fov_y_rad, aspect, z_near, z_far);
}

void Camera::SetPerspective(float fov, float aspct, float near, float far)
{
    fov_y_deg = fov;
    aspect = aspct;
    z_near = near;
    z_far = far;
}

Vec3 Camera::GetViewDirection() const
{
    return Vec3::Normalize(target - position);
}