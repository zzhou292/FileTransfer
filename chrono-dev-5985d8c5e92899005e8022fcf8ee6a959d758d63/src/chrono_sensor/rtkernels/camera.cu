// =============================================================================
// PROJECT CHRONO - http://projectchrono.org
//
// Copyright (c) 2019 projectchrono.org
// All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file at the top level of the distribution and at
// http://projectchrono.org/license-chrono.txt.
//
// =============================================================================
// Authors: Asher Elmquist
// =============================================================================
//
// RT kernels for tracing and measureing depth for a typical pinhole camera
//
// =============================================================================

#include <math_constants.h>
#include <optixu/optixu_aabb.h>
#include "chrono_sensor/rtkernels/ray_utils.h"

using namespace optix;

rtDeclareVariable(PerRayData_camera, prd_camera, rtPayload, );
rtDeclareVariable(optix::Ray, ray, rtCurrentRay, );
rtDeclareVariable(uint2, launch_index, rtLaunchIndex, );
rtDeclareVariable(float, scene_epsilon, , );
rtDeclareVariable(float, max_scene_distance, , );
rtDeclareVariable(rtObject, root_node, , );
rtDeclareVariable(float3, default_color, , );
rtDeclareVariable(float, default_depth, , );
rtDeclareVariable(int, max_depth, , );

// camera parameters
rtDeclareVariable(float3, c_pos, , );      // camera position
rtDeclareVariable(float3, c_up, , );       // camera up vector
rtDeclareVariable(float3, c_forward, , );  // camera forward vector
rtDeclareVariable(float3, c_left, , );     // camera left vector
rtDeclareVariable(float, c_hFOV, , );      // camera horizontal field of view
// rtDeclareVariable(float, c_vFOV, , );      // camera vertical field of view

// environment map
rtTextureSampler<float4, 2> environment_map;
rtDeclareVariable(int, has_environment_map, , );

rtBuffer<uchar4, 2> output_buffer;  // byte version

// This kernel is launched once for each pixel in the image
RT_PROGRAM void pinhole_camera() {
    size_t2 screen = output_buffer.size();

    // set the ray direction based on the proportion of image the pixel is located at
    float2 d = (make_float2(launch_index) + make_float2(0.5, 0.5)) / make_float2(screen) * 2.f - 1.f;
    d.y *= (float)(screen.y) / (float)(screen.x);
    // origin of the camera is  0,0,0 for now
    float3 ray_origin = c_pos;
    float h_factor = c_hFOV / CUDART_PI_F * 2.0;
    // float v_factor = c_vFOV / CUDART_PI_F * 2.0;
    float3 ray_direction = normalize(c_forward - d.x * c_left * h_factor + d.y * c_up * h_factor);

    // create a ray based on the calculated parameters
    optix::Ray ray(ray_origin, ray_direction, CAMERA_RAY_TYPE, scene_epsilon, max_scene_distance);

    // set the ray pay load
    PerRayData_camera prd_camera = make_camera_data(make_float3(0), 1.f, 1);

    // launch the ray
    rtTrace(root_node, ray, prd_camera, RT_RAY_FLAG_DISABLE_ANYHIT);

    // set the output buffer to be what is returned in the payload
    output_buffer[launch_index] = make_color(prd_camera.color);
}

// This kernel is launched once for each pixel in the image
RT_PROGRAM void fov_lens_camera() {
    size_t2 screen = output_buffer.size();

    // set the ray direction based on the proportion of image the pixel is located at
    float2 d = (make_float2(launch_index) + make_float2(0.5, 0.5)) / make_float2(screen) * 2.f - 1.f;
    // rescale y direction to be proportional to x
    d.y *= (float)(screen.y) / (float)(screen.x);

    if (abs(d.x) > 1e-5 || abs(d.y) > 1e-5) {
        // float omega = c_hFOV * (tan(tan(c_hFOV / 2.0)) / tan(c_hFOV / 2.0));

        float r1 = sqrtf(d.x * d.x + d.y * d.y);

        // float r2 = tan(A * r1 * c_hFOV / 2.0) / (tan(c_hFOV / 2.0));
        float r2 = tan(r1 * tan(c_hFOV / 2.0)) / tan(c_hFOV / 2.0);
        // float r2 = r1 / cos(c_hFOV / 2);

        float scaled_extent = tan(tan(c_hFOV / 2.0)) / tan(c_hFOV / 2.0);

        // float y_max = 1.4142;

        d.x = d.x * (r2 / r1) / scaled_extent;
        d.y = d.y * (r2 / r1) / scaled_extent;
    }

    // origin of the camera is  0,0,0 for now
    float3 ray_origin = c_pos;
    float h_factor = c_hFOV / CUDART_PI_F * 2.0;
    // float v_factor = c_vFOV / CUDART_PI_F * 2.0;
    float3 ray_direction = normalize(c_forward - d.x * c_left * h_factor + d.y * c_up * h_factor);

    // create a ray based on the calculated parameters
    optix::Ray ray(ray_origin, ray_direction, CAMERA_RAY_TYPE, scene_epsilon, max_scene_distance);

    // set the ray pay load
    PerRayData_camera prd_camera = make_camera_data(make_float3(0), 1.f, 1);

    // launch the ray
    rtTrace(root_node, ray, prd_camera, RT_RAY_FLAG_DISABLE_ANYHIT);

    // set the output buffer to be what is returned in the payload
    output_buffer[launch_index] = make_color(prd_camera.color);
}

RT_PROGRAM void thinlens_camera() {
    size_t2 screen = output_buffer.size();

    // set the ray direction based on the proportion of image the pixel is located at
    float2 d = (make_float2(launch_index) + make_float2(0.5, 0.5)) / make_float2(screen) * 2.f - 1.f;
    d.y *= (float)(screen.y) / (float)(screen.x);
    // origin of the camera is  0,0,0 for now
    float3 ray_origin = c_pos;
    float h_factor = c_hFOV / CUDART_PI_F * 2.0;
    // float v_factor = c_vFOV / CUDART_PI_F * 2.0;
    float3 ray_direction = normalize(c_forward - d.x * c_left * h_factor + d.y * c_up * h_factor);

    // create a ray based on the calculated parameters
    optix::Ray ray(ray_origin, ray_direction, CAMERA_RAY_TYPE, scene_epsilon, max_scene_distance);

    // set the ray pay load
    PerRayData_camera prd_camera = make_camera_data(make_float3(0), 1.f, 1);

    // launch the ray
    rtTrace(root_node, ray, prd_camera, RT_RAY_FLAG_DISABLE_ANYHIT);

    // TODO: this should be defined in the camera sensor class
    float dir_sensor_len_ratio = .5f;  // ratio of direction incident on sensor to direction incident on lens
    float3 sensor_incident_vec = normalize(dir_sensor_len_ratio * ray_direction + c_forward);

    float3 distort_color = prd_camera.color * powf((dot(c_forward, sensor_incident_vec)), 4);

    // set the output buffer to be what is returned in the payload
    output_buffer[launch_index] = make_color(distort_color);
}
