/*
    Copyright (c) 2017 Krzysztof Gabis
    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:
    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.
    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
    THE SOFTWARE.
*/

/* To update:
curl https://raw.githubusercontent.com/kgabis/cutils/master/mathutils.h > mathutils.h
curl https://raw.githubusercontent.com/kgabis/cutils/master/mathutils.c > mathutils.c
*/

#ifndef mathutils_h
#define mathutils_h

#include <stdbool.h>

typedef struct {
    float x, y;
} vec2_t;

typedef struct {
    float x, y, z;
} vec3_t;

typedef struct {
    float x, y, z, w;
} vec4_t;

typedef struct {
    float x, y, z, w;
} quat_t;

typedef struct {
    float m00, m01, m02, m03;
    float m10, m11, m12, m13;
    float m20, m21, m22, m23;
    float m30, m31, m32, m33;
} mat44_t;

//-----------------------------------------------------------------------------
// Various
//-----------------------------------------------------------------------------
float inv_sqrtf(float x);
bool float_eq(float a, float b);

//-----------------------------------------------------------------------------
// Vec2
//-----------------------------------------------------------------------------
vec2_t vec2_make(float x, float y);
vec2_t vec2_add(vec2_t a, vec2_t b);
vec2_t vec2_add3(vec2_t a, vec2_t b, vec2_t c);
vec2_t vec2_sub(vec2_t a, vec2_t b);
vec2_t vec2_mult(vec2_t a, vec2_t b);
vec2_t vec2_mults(vec2_t a, float s);
vec2_t vec2_div(vec2_t a, vec2_t b);
vec2_t vec2_divs(vec2_t a, float s);
float vec2_cross(vec2_t a, vec2_t b);
bool vec2_is_equal(vec2_t a, vec2_t b);
void vec2_print(vec2_t v);

//-----------------------------------------------------------------------------
// Vec3
//-----------------------------------------------------------------------------
vec3_t vec3_make(float x, float y, float z);
vec3_t vec3_add(vec3_t a, vec3_t b);
vec3_t vec3_add3(vec3_t a, vec3_t b, vec3_t c);
vec3_t vec3_sub(vec3_t a, vec3_t b);
vec3_t vec3_mult_mat44(vec3_t a, const mat44_t *m);
vec3_t vec3_mults(vec3_t v, float s);
vec3_t vec3_mult(vec3_t a, vec3_t b);
vec3_t vec3_normalize(vec3_t v);
vec3_t vec3_cross(vec3_t a, vec3_t b);
float  vec3_dot(vec3_t a, vec3_t b);
float  vec3_length(vec3_t v);
float vec3_length_sq(vec3_t v);
vec3_t vec3_negate(vec3_t v);

//-----------------------------------------------------------------------------
// Vec4
//-----------------------------------------------------------------------------
vec4_t vec4_make(float x, float y, float z, float w);
vec4_t vec4_from_vec3(vec3_t v);
vec3_t vec4_xyz(vec4_t v);
vec4_t vec4_mult_mat44(vec4_t v, const mat44_t *m);

//-----------------------------------------------------------------------------
// Quaternion
//-----------------------------------------------------------------------------
quat_t quat_make(float x, float y, float z, float w);
quat_t quat_axis(vec3_t ax, float angle);
quat_t quat_euler(float roll, float pitch, float yaw);
quat_t quat_lookat(vec3_t source, vec3_t dest);
bool   quat_is_normalized(quat_t q);
quat_t quat_normalize(quat_t q);
quat_t quat_mult(quat_t a, quat_t b);

//-----------------------------------------------------------------------------
// Matrix 4x4
//-----------------------------------------------------------------------------
void mat44_init(float m00, float m01, float m02, float m03,
                float m10, float m11, float m12, float m13,
                float m20, float m21, float m22, float m23,
                float m30, float m31, float m32, float m33,
                mat44_t *out_res);
void mat44_init_empty(mat44_t *out_res);
void mat44_identity(mat44_t *out_res);
float mat44_det(const mat44_t *mat);
void mat44_inv(const mat44_t *mat, bool *out_ok, mat44_t *out_res);
void mat44_mults(const mat44_t *mat, float s, mat44_t *out_res);
void mat44_scale(float x, float y, float z, mat44_t *out_res);
void mat44_rotate(quat_t q, mat44_t *out_res);
void mat44_translate(float x, float y, float z, mat44_t *out_res);
void mat44_transpose(const mat44_t *m, mat44_t *out_res);
void mat44_mult(const mat44_t *a, const mat44_t *b, mat44_t *out_res);
void mat44_frustum(float near, float far, float fov, float ratio, mat44_t *out_res);
void mat44_from_direction(vec3_t dir, mat44_t *out_res);

#endif /* mathutils_h */
