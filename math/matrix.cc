#include "matrix.h"

/*
    3x3 Matrix
*/

Vec3 Mat3x3::operator*(const Vec3& other) const
{
    return Vec3(
        m[0][0] * other.x + m[0][1] * other.y + m[0][2] * other.z,
        m[1][0] * other.x + m[1][1] * other.y + m[1][2] * other.z,
        m[2][0] * other.x + m[2][1] * other.y + m[2][2] * other.z
    );
}

Mat3x3& Mat3x3::InverseTranspose()
{
    float a00 = m[0][0], a01 = m[0][1], a02 = m[0][2];
    float a10 = m[1][0], a11 = m[1][1], a12 = m[1][2];
    float a20 = m[2][0], a21 = m[2][1], a22 = m[2][2];

    float det = a00*(a11*a22 - a12*a21)
                -a01*(a10*a22 - a12*a20)
                +a02*(a10*a21 - a11*a20);

    if (std::fabs(det) < 1e-6f)
    {
        m[0][0]=1; m[0][1]=0; m[0][2]=0;
        m[1][0]=0; m[1][1]=1; m[1][2]=0;
        m[2][0]=0; m[2][1]=0; m[2][2]=1;
        return *this;
    }

    float invDet = 1.0f / det;

    float c00 =  (a11*a22 - a12*a21) * invDet;
    float c01 = -(a10*a22 - a12*a20) * invDet;
    float c02 =  (a10*a21 - a11*a20) * invDet;

    float c10 = -(a01*a22 - a02*a21) * invDet;
    float c11 =  (a00*a22 - a02*a20) * invDet;
    float c12 = -(a00*a21 - a01*a20) * invDet;

    float c20 =  (a01*a12 - a02*a11) * invDet;
    float c21 = -(a00*a12 - a02*a10) * invDet;
    float c22 =  (a00*a11 - a01*a10) * invDet;

    m[0][0]=c00; m[0][1]=c01; m[0][2]=c02;
    m[1][0]=c10; m[1][1]=c11; m[1][2]=c12;
    m[2][0]=c20; m[2][1]=c21; m[2][2]=c22;

    return *this;
}

/*
    4x4 Matrix
*/

Mat4x4::Mat4x4(float value) 
{
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            m[i][j] = value;
}

Mat4x4::Mat4x4(const float data[16]) 
{
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            m[i][j] = data[i * 4 + j];
}

Mat4x4 Mat4x4::operator*(const Mat4x4& other) const 
{
    Mat4x4 result;

    for (int row = 0; row < 4; row++)
        for (int col = 0; col < 4; col++)
            for (int k = 0; k < 4; k++)
                result.m[row][col] += m[row][k] * other.m[k][col];

    return result;
}

Mat4x4& Mat4x4::operator*=(const Mat4x4& other) 
{
    Mat4x4 result;

    for (int row = 0; row < 4; row++)
        for (int col = 0; col < 4; col++)
            for (int k = 0; k < 4; k++)
                result.m[row][col] += m[row][k] * other.m[k][col];

    *this = result;
    return *this;
}

Vec4 Mat4x4::operator*(const Vec4& other) const 
{
    Vec4 result;

    result.x = m[0][0]*other.x + m[0][1]*other.y + m[0][2]*other.z + m[0][3]*other.w;
    result.y = m[1][0]*other.x + m[1][1]*other.y + m[1][2]*other.z + m[1][3]*other.w;
    result.z = m[2][0]*other.x + m[2][1]*other.y + m[2][2]*other.z + m[2][3]*other.w;
    result.w = m[3][0]*other.x + m[3][1]*other.y + m[3][2]*other.z + m[3][3]*other.w;

    return result;
}

Mat4x4 Mat4x4::operator+(const Mat4x4& other) const 
{
    Mat4x4 result;

    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            result.m[i][j] = m[i][j] + other.m[i][j];

    return result;
}

Mat4x4& Mat4x4::operator+=(const Mat4x4& other) 
{
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            m[i][j] += other.m[i][j];

    return *this;
}

Mat4x4 Mat4x4::operator-(const Mat4x4& other) const 
{
    Mat4x4 result;

    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            result.m[i][j] = m[i][j] - other.m[i][j];

    return result;
}

Mat4x4& Mat4x4::operator-=(const Mat4x4& other) 
{
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            m[i][j] -= other.m[i][j];

    return *this;
}

Mat4x4 Mat4x4::operator*(float scalar) const 
{
    Mat4x4 result;

    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            result.m[i][j] = m[i][j] * scalar;

    return result;
}

Mat4x4& Mat4x4::operator*=(float scalar) 
{
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            m[i][j] *= scalar;

    return *this;
}

Mat4x4 Mat4x4::operator/(float scalar) const 
{
    Mat4x4 result;

    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            result.m[i][j] = m[i][j] / scalar;

    return result;
}

Mat4x4& Mat4x4::operator/=(float scalar) 
{
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            m[i][j] /= scalar;

    return *this;
}

Mat4x4 Mat4x4::Identity()
{
    Mat4x4 mat;

    for (int i = 0; i < 4; ++i)
        mat.m[i][i] = 1.0f;

    return mat;
}

Mat4x4 Mat4x4::Zero()
{
    return Mat4x4();
}

/*
    1  0  0  x
    0  1  0  y
    0  0  1  z
    0  0  0  1
*/
Mat4x4 Mat4x4::Translation(const Vec3& t)
{
    Mat4x4 mat = Identity();
    mat.m[0][3] = t.x;
    mat.m[1][3] = t.y;
    mat.m[2][3] = t.z;
    return mat;
}

/*
    x  0  0  0
    0  y  0  0
    0  0  z  0
    0  0  0  1
*/
Mat4x4 Mat4x4::Scale(const Vec3& s)
{
    Mat4x4 mat = Identity();
    mat.m[0][0] = s.x;
    mat.m[1][1] = s.y;
    mat.m[2][2] = s.z;
    return mat;
}

/*
    1  0  0  0
    0  c -s  0
    0  s  c  0
    0  0  0  1
*/
Mat4x4 Mat4x4::RotationX(float angle)
{
    Mat4x4 mat = Identity();
    float c = std::cos(angle), s = std::sin(angle);
    mat.m[1][1] = c;
    mat.m[1][2] = -s;
    mat.m[2][1] = s;
    mat.m[2][2] = c;
    return mat;
}

/*
     c  0  s  0
     0  1  0  0
    -s  0  c  0
     0  0  0  1
*/
Mat4x4 Mat4x4::RotationY(float angle) {
    Mat4x4 mat = Identity();
    float c = std::cos(angle), s = std::sin(angle);
    mat.m[0][0] = c;
    mat.m[0][2] = s;
    mat.m[2][0] = -s;
    mat.m[2][2] = c;
    return mat;
}

/*
    c -s  0  0
    s  c  0  0
    0  0  1  0
    0  0  0  1
*/
Mat4x4 Mat4x4::RotationZ(float angle) {
    Mat4x4 mat = Identity();
    float c = std::cos(angle), s = std::sin(angle);
    mat.m[0][0] = c;
    mat.m[0][1] = -s;
    mat.m[1][0] = s;
    mat.m[1][1] = c;
    return mat;
}

/*
    cot(0.5*theta)/aspect               0            0               0
                        0  cot(0.5*theta)            0               0
                        0               0  (n+f)/(n-f)  (-2*n*f)/(n-f)
                        0               0           -1               0
*/
Mat4x4 Mat4x4::Perspective(float fov, float aspect, float near, float far)
{
    Mat4x4 mat;
    float cot_value = 1.0f / std::tan(fov/2);
    float inverse_near_minus_far = 1.0f / (near-far);

    mat.m[0][0] = cot_value / aspect;
    mat.m[1][1] = cot_value;
    mat.m[2][2] = (far + near) * inverse_near_minus_far;
    mat.m[2][3] = (-2 * far * near) * inverse_near_minus_far;
    mat.m[3][2] = -1.0f;

    return mat;
}

/*
    1. set 'camera position' and 'look at position'

    2. make vector n from that two position, this is z axis

    3. make vector u by cross up and n, this is x axis
    (up vector is usually <0, 1, 0> )

    4. make vector v by cross n and u, this is y axis

    5. u v n coordinate completed

    |  Ux  Uy  Uz  -dot(U, camera_pos) |
    |  Vx  Vy  Vz  -dot(V, camera_pos) |
    |  Nx  Ny  Nz  -dot(N, camera_pos) |
    |   0   0   0      1               |
*/
Mat4x4 Mat4x4::LookAt(const Vec3& camera_pos, const Vec3& look_at_pos, const Vec3& up)
{
    Vec3 n = Vec3::Normalize(camera_pos - look_at_pos);
    Vec3 u = Vec3::Normalize( Vec3::Cross(up, n) );
    Vec3 v = Vec3::Cross( n, u );

    Mat4x4 mat = Identity();

    mat.m[0][0] = u.x;  mat.m[0][1] = u.y;  mat.m[0][2] = u.z;
    mat.m[1][0] = v.x;  mat.m[1][1] = v.y;  mat.m[1][2] = v.z;
    mat.m[2][0] = n.x;  mat.m[2][1] = n.y;  mat.m[2][2] = n.z;

    mat.m[0][3] = -Vec3::Dot(u, camera_pos);
    mat.m[1][3] = -Vec3::Dot(v, camera_pos);
    mat.m[2][3] = -Vec3::Dot(n, camera_pos);

    mat.m[3][3] = 1;

    return mat;
}

/* 
    near means that clipping window is on near plane
    implementation should be changed if wanting the clipping window be off the near plane

    M_normpers =

    |  -2·n/(x_max-x_min)            0                 (x_max+x_min)/(x_max-x_min)             0    |
    |       0              -2·near/(y_max-y_min)     (y_max + y_min)/(y_max-y_min)             0    |
    |       0                        0                         (n+f)/(n-f)          (-2·n·f)/(n-f)  |
    |       0                        0                             -1                          0    |

*/
Mat4x4 Mat4x4::PerspectiveOffCenter( float x_near_min, float x_near_max,
                                     float y_near_min, float y_near_max,
                                     float z_near,     float z_far )
{
    Mat4x4 mat;

    float inverse_x_max_minus_x_min  = 1.0f / (x_near_max - x_near_min);
    float inverse_y_max_minus_y_min  = 1.0f / (y_near_max - y_near_min);
    float inverse_z_near_minus_z_far = 1.0f / (z_near     - z_far);

    mat.m[0][0] = -2.0f * z_near * inverse_x_max_minus_x_min;
    mat.m[0][2] = (x_near_max + x_near_min) * inverse_x_max_minus_x_min;

    mat.m[1][1] = -2.0f * z_near * inverse_y_max_minus_y_min;
    mat.m[1][2] = (y_near_max + y_near_min) * inverse_y_max_minus_y_min;

    mat.m[2][2] = (z_far + z_near) * inverse_z_near_minus_z_far;
    mat.m[2][3] = (-2.0f * z_near * z_far) * inverse_z_near_minus_z_far;

    mat.m[3][2] = -1.0f;

    return mat;
}

/*
    M_viewport = 

    |  (x_max - x_min)/2          0             0      (x_max + x_min)/2  |
    |         0           (y_max - y_min)/2     0      (y_max + y_min)/2  |
    |         0                   0            1/2             1/2        | 
    |         0                   0             0               1         |
*/

Mat4x4 Mat4x4::ViewportTransformation( float x_view_min, float x_view_max, 
                                       float y_view_min, float y_view_max )
{
    Mat4x4 mat;

    mat.m[0][0] = (x_view_max - x_view_min) * 0.5f;
    mat.m[0][3] = (x_view_max + x_view_min) * 0.5f;
    mat.m[1][1] = (y_view_max - y_view_min) * 0.5f;
    mat.m[1][3] = (y_view_max + y_view_min) * 0.5f;
    mat.m[2][2] = 0.5f;
    mat.m[2][3] = 0.5f;
    mat.m[3][3] = 1.0f;

    return mat;
}


Mat3x3 Mat4x4::TopLeft3x3() const
{
    Mat3x3 result;

    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            result.m[i][j] = m[i][j];

    return result;
}