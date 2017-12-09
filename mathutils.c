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

#include "mathutils.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

#define MATHUTILS_EPS 0.00001

//-----------------------------------------------------------------------------
// Various
//-----------------------------------------------------------------------------

float inv_sqrtf(float x) {
    float xhalf = 0.5f * x;
    int i = *(int*)&x;              // get bits for floating value
    i = 0x5f375a86 - (i >> 1);      // gives initial guess y0
    x = *(float*)&i;                // convert bits back to float
    x = x * (1.5f - xhalf * x * x); // Newton step, repeating increases accuracy
    return x;
}

bool float_eq(float a, float b) {
    return fabsf(a - b) < MATHUTILS_EPS;
}

//-----------------------------------------------------------------------------
// Vec2
//-----------------------------------------------------------------------------

vec2_t vec2_make(float x, float y) {
    return (vec2_t){.x = x, .y = y};
}

vec2_t vec2_add(vec2_t a, vec2_t b) {
    return (vec2_t){.x = a.x + b.x, .y = a.y + b.y};
}

vec2_t vec2_add3(vec2_t a, vec2_t b, vec2_t c) {
    return (vec2_t){.x = a.x + b.x + c.x, .y = a.y + b.y + c.y};
}

vec2_t vec2_sub(vec2_t a, vec2_t b) {
    return (vec2_t){.x = a.x - b.x, .y = a.y - b.y};
}

vec2_t vec2_mult(vec2_t a, vec2_t b) {
    return (vec2_t){.x = a.x * b.x, .y = a.y * b.y};
}

vec2_t vec2_mults(vec2_t a, float s) {
    return (vec2_t){.x = a.x * s, .y = a.y * s};
}

vec2_t vec2_div(vec2_t a, vec2_t b) {
    return (vec2_t){.x = a.x / b.x, .y = a.y / b.y};
}

vec2_t vec2_divs(vec2_t a, float s) {
    return (vec2_t){.x = a.x / s, .y = a.y / s};
}

float vec2_cross(vec2_t a, vec2_t b) {
    return (a.x*b.y)-(a.y*b.x);
}

bool vec2_is_equal(vec2_t a, vec2_t b) {
    return fabs(a.x - b.x) < MATHUTILS_EPS && fabs(a.y - b.y) < MATHUTILS_EPS;
}

//-----------------------------------------------------------------------------
// Vec2
//-----------------------------------------------------------------------------

vec3_t vec3_make(float x, float y, float z) {
    return (vec3_t){.x = x, .y = y, .z = z};
}

vec3_t vec3_add(vec3_t a, vec3_t b) {
    return (vec3_t){.x = a.x + b.x, .y = a.y + b.y, .z = a.z + b.z};
}


vec3_t vec3_add3(vec3_t a, vec3_t b, vec3_t c) {
    return (vec3_t){
        .x = a.x + b.x + c.x,
        .y = a.y + b.y + c.y,
        .z = a.z + b.z + c.z
    };
}

vec3_t vec3_sub(vec3_t a, vec3_t b) {
    return (vec3_t){.x = a.x - b.x, .y = a.y - b.y, .z = a.z - b.z};
}

vec3_t vec3_mults(vec3_t v, float s) {
    return (vec3_t){.x = v.x*s, .y = v.y*s, .z = v.z*s};
}

vec3_t vec3_mult(vec3_t a, vec3_t b) {
    return (vec3_t){.x = a.x * b.x, .y = a.y * b.y, .z = a.z * b.z};
}

vec3_t vec3_mult_mat44(vec3_t a, const mat44_t *m) {
    float x = m->m00*a.x + m->m10*a.y + m->m20*a.z + m->m30;
    float y = m->m01*a.x + m->m11*a.y + m->m21*a.z + m->m31;
    float z = m->m02*a.x + m->m12*a.y + m->m22*a.z + m->m32;
    return vec3_make(x, y, z);
}

vec3_t vec3_normalize(vec3_t v) {
    float inv_len = inv_sqrtf((v.x*v.x) + (v.y*v.y) + (v.z*v.z));
    return vec3_mults(v, inv_len);
}

vec3_t vec3_cross(vec3_t a, vec3_t b) {
    vec3_t v;
    v.x = (a.y * b.z) - (b.y * a.z);
    v.y = (a.z * b.x) - (b.z * a.x);
    v.z = (a.x * b.y) - (b.x * a.y);
    return v;
}

float  vec3_dot(vec3_t a, vec3_t b) {
    return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}

float  vec3_length(vec3_t v) {
    return sqrtf((v.x*v.x) + (v.y*v.y) + (v.z*v.z));
}

float vec3_length_sq(vec3_t v) {
    return (v.x*v.x) + (v.y*v.y) + (v.z*v.z);
}

vec3_t vec3_negate(vec3_t v) {
    return (vec3_t){.x = -v.x, .y = -v.y, .z = -v.z};
}

//-----------------------------------------------------------------------------
// Vec4
//-----------------------------------------------------------------------------

vec4_t vec4_make(float x, float y, float z, float w) {
    return (vec4_t){.x = x, .y = y, .z = z, .w = w};
}

vec4_t vec4_from_vec3(vec3_t v) {
    return vec4_make(v.x, v.y, v.z, 0.0f);
}

vec3_t vec4_xyz(vec4_t v) {
    return vec3_make(v.x, v.y, v.z);
}

vec4_t vec4_mult_mat44(vec4_t v, const mat44_t *m) {
    float x = m->m00 * v.x + m->m10 * v.y + m->m20 * v.z + m->m30 * v.w;
    float y = m->m01 * v.x + m->m11 * v.y + m->m21 * v.z + m->m31 * v.w;
    float z = m->m02 * v.x + m->m12 * v.y + m->m22 * v.z + m->m32 * v.w;
    float w = m->m03 * v.x + m->m13 * v.y + m->m23 * v.z + m->m33 * v.w;
    return (vec4_t){.x = x, .y = y, .z = z, .w = w};
}

//-----------------------------------------------------------------------------
// Quat
//-----------------------------------------------------------------------------

quat_t quat_make(float x, float y, float z, float w) {
    return (quat_t){.x = x, .y = y, .z = z, .w = w};
}

quat_t quat_axis(vec3_t ax, float angle) {
    float len = sqrtf(ax.x*ax.x + ax.y*ax.y + ax.z*ax.z);
    float sinangle = sinf(0.5f * angle)/len;
    float cosangle = cosf(0.5f * angle);
    return quat_make(ax.x*sinangle, ax.y*sinangle, ax.z*sinangle, cosangle);
}

quat_t quat_euler(float roll, float pitch, float yaw) {
    float p = pitch * 0.5f;
    float y = yaw * 0.5f;
    float r = roll * 0.5f;

    float sinp = sin(p);
    float siny = sin(y);
    float sinr = sin(r);

    float cosp = cos(p);
    float cosy = cos(y);
    float cosr = cos(r);

    quat_t q;
    q.x = sinr * cosp * cosy - cosr * sinp * siny;
    q.y = cosr * sinp * cosy + sinr * cosp * siny;
    q.z = cosr * cosp * siny - sinr * sinp * cosy;
    q.w = cosr * cosp * cosy + sinr * sinp * siny;
    return q;
}

bool quat_is_normalized(quat_t q) {
    return fabs((q.x*q.x + q.y*q.y + q.z*q.z + q.w*q.w) - 1.0f) < 0.00001;
}

quat_t quat_normalize(quat_t q) {
    float inv_len = 1.0f / sqrt(q.x*q.x + q.y*q.y + q.z*q.z + q.w*q.w);
    return quat_make(q.x*inv_len, q.y*inv_len, q.z*inv_len, q.w*inv_len);
}

quat_t quat_mult(quat_t a, quat_t b) {
    quat_t q1 = a, q2 = b;
    quat_t q;
    q.x =  q1.x * q2.w + q1.y * q2.z - q1.z * q2.y + q1.w * q2.x;
    q.y = -q1.x * q2.z + q1.y * q2.w + q1.z * q2.x + q1.w * q2.y;
    q.z =  q1.x * q2.y - q1.y * q2.x + q1.z * q2.w + q1.w * q2.z;
    q.w = -q1.x * q2.x - q1.y * q2.y - q1.z * q2.z + q1.w * q2.w;
    return q;
}

//-----------------------------------------------------------------------------
// Mat44
//-----------------------------------------------------------------------------

void mat44_init(float m00, float m01, float m02, float m03,
                float m10, float m11, float m12, float m13,
                float m20, float m21, float m22, float m23,
                float m30, float m31, float m32, float m33,
                mat44_t *r) {
    r->m00 = m00; r->m01 = m01; r->m02 = m02; r->m03 = m03;
    r->m10 = m10; r->m11 = m11; r->m12 = m12; r->m13 = m13;
    r->m20 = m20; r->m21 = m21; r->m22 = m22; r->m23 = m23;
    r->m30 = m30; r->m31 = m31; r->m32 = m32; r->m33 = m33;
}

void mat44_init_empty(mat44_t *r) {
    memset(r, 0, sizeof(*r));
}

void mat44_identity(mat44_t *r) {
    mat44_init_empty(r);
    r->m00 = 1.0f;
    r->m11 = 1.0f;
    r->m22 = 1.0f;
    r->m33 = 1.0f;
}

float mat44_det(const mat44_t *m) {
    float r;
    r = m->m03*m->m12*m->m21*m->m30 - m->m02*m->m13*m->m21*m->m30 - m->m03*m->m11*m->m22*m->m30 + m->m01*m->m13*m->m22*m->m30 +
        m->m02*m->m11*m->m23*m->m30 - m->m01*m->m12*m->m23*m->m30 - m->m03*m->m12*m->m20*m->m31 + m->m02*m->m13*m->m20*m->m31 +
        m->m03*m->m10*m->m22*m->m31 - m->m00*m->m13*m->m22*m->m31 - m->m02*m->m10*m->m23*m->m31 + m->m00*m->m12*m->m23*m->m31 +
        m->m03*m->m11*m->m20*m->m32 - m->m01*m->m13*m->m20*m->m32 - m->m03*m->m10*m->m21*m->m32 + m->m00*m->m13*m->m21*m->m32 +
        m->m01*m->m10*m->m23*m->m32 - m->m00*m->m11*m->m23*m->m32 - m->m02*m->m11*m->m20*m->m33 + m->m01*m->m12*m->m20*m->m33 +
        m->m02*m->m10*m->m21*m->m33 - m->m00*m->m12*m->m21*m->m33 - m->m01*m->m10*m->m22*m->m33 + m->m00*m->m11*m->m22*m->m33;
    return r;
}

void mat44_inv(const mat44_t *m, bool *out_ok, mat44_t *r) {
    float det = mat44_det(m);
    if (float_eq(det, 0)) {
        if (out_ok != NULL) {
            *out_ok = false;
        }
        return;
    }
    r->m00 = m->m12*m->m23*m->m31-m->m13*m->m22*m->m31+m->m13*m->m21*m->m32-m->m11*m->m23*m->m32-m->m12*m->m21*m->m33+m->m11*m->m22*m->m33;
    r->m01 = m->m03*m->m22*m->m31-m->m02*m->m23*m->m31-m->m03*m->m21*m->m32+m->m01*m->m23*m->m32+m->m02*m->m21*m->m33-m->m01*m->m22*m->m33;
    r->m02 = m->m02*m->m13*m->m31-m->m03*m->m12*m->m31+m->m03*m->m11*m->m32-m->m01*m->m13*m->m32-m->m02*m->m11*m->m33+m->m01*m->m12*m->m33;
    r->m03 = m->m03*m->m12*m->m21-m->m02*m->m13*m->m21-m->m03*m->m11*m->m22+m->m01*m->m13*m->m22+m->m02*m->m11*m->m23-m->m01*m->m12*m->m23;
    r->m10 = m->m13*m->m22*m->m30-m->m12*m->m23*m->m30-m->m13*m->m20*m->m32+m->m10*m->m23*m->m32+m->m12*m->m20*m->m33-m->m10*m->m22*m->m33;
    r->m11 = m->m02*m->m23*m->m30-m->m03*m->m22*m->m30+m->m03*m->m20*m->m32-m->m00*m->m23*m->m32-m->m02*m->m20*m->m33+m->m00*m->m22*m->m33;
    r->m12 = m->m03*m->m12*m->m30-m->m02*m->m13*m->m30-m->m03*m->m10*m->m32+m->m00*m->m13*m->m32+m->m02*m->m10*m->m33-m->m00*m->m12*m->m33;
    r->m13 = m->m02*m->m13*m->m20-m->m03*m->m12*m->m20+m->m03*m->m10*m->m22-m->m00*m->m13*m->m22-m->m02*m->m10*m->m23+m->m00*m->m12*m->m23;
    r->m20 = m->m11*m->m23*m->m30-m->m13*m->m21*m->m30+m->m13*m->m20*m->m31-m->m10*m->m23*m->m31-m->m11*m->m20*m->m33+m->m10*m->m21*m->m33;
    r->m21 = m->m03*m->m21*m->m30-m->m01*m->m23*m->m30-m->m03*m->m20*m->m31+m->m00*m->m23*m->m31+m->m01*m->m20*m->m33-m->m00*m->m21*m->m33;
    r->m22 = m->m01*m->m13*m->m30-m->m03*m->m11*m->m30+m->m03*m->m10*m->m31-m->m00*m->m13*m->m31-m->m01*m->m10*m->m33+m->m00*m->m11*m->m33;
    r->m23 = m->m03*m->m11*m->m20-m->m01*m->m13*m->m20-m->m03*m->m10*m->m21+m->m00*m->m13*m->m21+m->m01*m->m10*m->m23-m->m00*m->m11*m->m23;
    r->m30 = m->m12*m->m21*m->m30-m->m11*m->m22*m->m30-m->m12*m->m20*m->m31+m->m10*m->m22*m->m31+m->m11*m->m20*m->m32-m->m10*m->m21*m->m32;
    r->m31 = m->m01*m->m22*m->m30-m->m02*m->m21*m->m30+m->m02*m->m20*m->m31-m->m00*m->m22*m->m31-m->m01*m->m20*m->m32+m->m00*m->m21*m->m32;
    r->m32 = m->m02*m->m11*m->m30-m->m01*m->m12*m->m30-m->m02*m->m10*m->m31+m->m00*m->m12*m->m31+m->m01*m->m10*m->m32-m->m00*m->m11*m->m32;
    r->m33 = m->m01*m->m12*m->m20-m->m02*m->m11*m->m20+m->m02*m->m10*m->m21-m->m00*m->m12*m->m21-m->m01*m->m10*m->m22+m->m00*m->m11*m->m22;
    if (out_ok != NULL) {
        *out_ok = true;
    }
    mat44_mults(r, 1.0/det, r);
}

void mat44_mults(const mat44_t *m, float s, mat44_t *r) {
    r->m00 = m->m00*s; r->m01 = m->m01*s; r->m02 = m->m02*s; r->m03 = m->m03*s;
    r->m10 = m->m10*s; r->m11 = m->m11*s; r->m12 = m->m12*s; r->m13 = m->m13*s;
    r->m20 = m->m20*s; r->m21 = m->m21*s; r->m22 = m->m22*s; r->m23 = m->m23*s;
    r->m30 = m->m30*s; r->m31 = m->m31*s; r->m32 = m->m32*s; r->m33 = m->m33*s;
}

void mat44_scale(float x, float y, float z, mat44_t *r) {
    mat44_init_empty(r);
    r->m00 = x;
    r->m11 = y;
    r->m22 = z;
    r->m33 = 1.0f;
}

void mat44_rotate(quat_t q, mat44_t *r) {
    if (!quat_is_normalized(q)) {
        q = quat_normalize(q);
    }

    float x2 = q.x + q.x, y2 = q.y + q.y, z2 = q.z + q.z;
    float xx = q.x * x2,  xy = q.x * y2,  xz = q.x * z2;
    float yy = q.y * y2,  yz = q.y * z2,  zz = q.z * z2;
    float wx = q.w * x2,  wy = q.w * y2,  wz = q.w * z2;

    r->m00 = 1.0f-(yy+zz); r->m01 = xy + wz;      r->m02 = xz - wy;      r->m03 = 0;
    r->m10 = xy - wz;      r->m11 = 1.0f-(xx+zz); r->m12 = yz + wx;      r->m13 = 0;
    r->m20 = xz + wy;      r->m21 = yz - wx;      r->m22 = 1.0f-(xx+yy); r->m23 = 0;
    r->m30 = 0;            r->m31 = 0;            r->m32 = 0;            r->m33 = 1.0f;
}

void mat44_translate(float x, float y, float z, mat44_t *r) {
    mat44_init_empty(r);
    r->m00 = 1.0f;
    r->m11 = 1.0f;
    r->m22 = 1.0f;
    r->m30 = x;
    r->m31 = y;
    r->m32 = z;
    r->m33 = 1.0f;
}

void mat44_transpose(const mat44_t *m, mat44_t *r) {
    r->m00 = m->m00; r->m01 = m->m10; r->m02 = m->m20; r->m03 = m->m30;
    r->m10 = m->m01; r->m11 = m->m11; r->m12 = m->m21; r->m13 = m->m31;
    r->m20 = m->m02; r->m21 = m->m12; r->m22 = m->m22; r->m23 = m->m32;
    r->m30 = m->m03; r->m31 = m->m13; r->m32 = m->m23; r->m33 = m->m33;
}

void mat44_mult(const mat44_t *a, const mat44_t *b, mat44_t *r) {
    r->m00 = (a->m00 * b->m00 + a->m01 * b->m10 + a->m02 * b->m20 + a->m03 * b->m30);
    r->m01 = (a->m00 * b->m01 + a->m01 * b->m11 + a->m02 * b->m21 + a->m03 * b->m31);
    r->m02 = (a->m00 * b->m02 + a->m01 * b->m12 + a->m02 * b->m22 + a->m03 * b->m32);
    r->m03 = (a->m00 * b->m03 + a->m01 * b->m13 + a->m02 * b->m23 + a->m03 * b->m33);

    r->m10 = (a->m10 * b->m00 + a->m11 * b->m10 + a->m12 * b->m20 + a->m13 * b->m30);
    r->m11 = (a->m10 * b->m01 + a->m11 * b->m11 + a->m12 * b->m21 + a->m13 * b->m31);
    r->m12 = (a->m10 * b->m02 + a->m11 * b->m12 + a->m12 * b->m22 + a->m13 * b->m32);
    r->m13 = (a->m10 * b->m03 + a->m11 * b->m13 + a->m12 * b->m23 + a->m13 * b->m33);

    r->m20 = (a->m20 * b->m00 + a->m21 * b->m10 + a->m22 * b->m20 + a->m23 * b->m30);
    r->m21 = (a->m20 * b->m01 + a->m21 * b->m11 + a->m22 * b->m21 + a->m23 * b->m31);
    r->m22 = (a->m20 * b->m02 + a->m21 * b->m12 + a->m22 * b->m22 + a->m23 * b->m32);
    r->m23 = (a->m20 * b->m03 + a->m21 * b->m13 + a->m22 * b->m23 + a->m23 * b->m33);

    r->m30 = (a->m30 * b->m00 + a->m31 * b->m10 + a->m32 * b->m20 + a->m33 * b->m30);
    r->m31 = (a->m30 * b->m01 + a->m31 * b->m11 + a->m32 * b->m21 + a->m33 * b->m31);
    r->m32 = (a->m30 * b->m02 + a->m31 * b->m12 + a->m32 * b->m22 + a->m33 * b->m32);
    r->m33 = (a->m30 * b->m03 + a->m31 * b->m13 + a->m32 * b->m23 + a->m33 * b->m33);
}

void mat44_frustum(float near, float far, float fov, float ratio, mat44_t *r) {
    float w = 1.0f / tan(fov * M_PI / 360.0f); // todo: check if order is correct
    float h = w * ratio;
    mat44_init_empty(r);
    r->m00 = w;
    r->m11 = h;
    r->m22 = far / (far - near);
    r->m23 = 1.0f;
    r->m32 = (-near * far) / (far - near);
}

void mat44_from_direction(vec3_t dir, mat44_t *r) {
    vec3_t up = vec3_make(0, 1, 0);

    dir = vec3_normalize(dir);

    vec3_t xaxis = vec3_cross(up, dir);
    xaxis = vec3_normalize(xaxis);

    vec3_t yaxis = vec3_cross(dir, xaxis);
    yaxis = vec3_normalize(yaxis);

    mat44_identity(r);
    r->m00 = xaxis.x;
    r->m01 = xaxis.y;
    r->m02 = xaxis.z;

    r->m10 = yaxis.x;
    r->m11 = yaxis.y;
    r->m12 = yaxis.z;

    r->m20 = dir.x;
    r->m21 = dir.y;
    r->m22 = dir.z;
}
