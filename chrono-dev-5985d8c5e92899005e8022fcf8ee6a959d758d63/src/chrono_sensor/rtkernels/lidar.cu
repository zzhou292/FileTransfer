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

rtDeclareVariable(PerRayData_lidar, prd_lidar, rtPayload, );
rtDeclareVariable(optix::Ray, ray, rtCurrentRay, );
rtDeclareVariable(uint2, launch_index, rtLaunchIndex, );
rtDeclareVariable(float, scene_epsilon, , );
rtDeclareVariable(float, max_scene_distance, , );
rtDeclareVariable(rtObject, root_node, , );

// camera parameters
rtDeclareVariable(float3, c_pos, , );      // lidar position
rtDeclareVariable(float3, c_up, , );       // lidar up vector
rtDeclareVariable(float3, c_forward, , );  // lidar forward vector
rtDeclareVariable(float3, c_left, , );     // lidar left vector
rtDeclareVariable(float, c_hFOV, , );      // lidar horizontal field of view
rtDeclareVariable(float, c_vFOV, , );      // lidar vertical field of view

rtBuffer<float2, 2> output_buffer;  // byte version

// for beam divergence and multi-sample kernel
rtDeclareVariable(float, divergence_angle, , );  // lidar beam divergence
rtDeclareVariable(int, ray_samples, , );         // samples per beam

// This kernel is launched once for each pixel in the image
RT_PROGRAM void spherical() {
    size_t2 screen = output_buffer.size();

    // set the ray direction based on the proportion of image the pixel is located at
    float2 d = (make_float2(launch_index) + make_float2(0.5, 0.5)) / make_float2(screen) * 2.f - 1.f;  //[-1,1]

    // float ranges[ray_samples];
    // float intensities[ray_samples];

    // float for (int i = 0; i < ray_samples; i++) {
    float theta = d.x * c_hFOV / 2.0;
    float phi = d.y * c_vFOV / 2.0;
    float xy_proj = cos(phi);

    float z = sin(phi);
    float y = xy_proj * sin(theta);
    float x = xy_proj * cos(theta);

    // origin of the camera is  0,0,0 for now
    float3 ray_origin = c_pos;
    float3 ray_direction = normalize(c_forward * x + c_left * y + c_up * z);

    // create a ray based on the calculated parameters
    optix::Ray ray(ray_origin, ray_direction, LIDAR_RAY_TYPE, scene_epsilon, max_scene_distance);

    // set the ray pay load
    PerRayData_lidar prd_lidar = make_lidar_data(0, 1.f, 0);

    // launch the ray
    rtTrace(root_node, ray, prd_lidar,RT_RAY_FLAG_DISABLE_ANYHIT);

    // set the output buffer to be what is returned in the payload
    output_buffer[launch_index] = make_float2(prd_lidar.range, prd_lidar.intensity);
}

RT_PROGRAM void multi_sample() {
    float div_angle = divergence_angle;
    int sample_radius = ray_samples;

    size_t2 screen = output_buffer.size();
    int2 global_beam_dims = make_int2(screen.x / (sample_radius * 2 - 1), screen.y / (sample_radius * 2 - 1));
    int2 local_beam_dims = make_int2(sample_radius * 2 - 1, sample_radius * 2 - 1);

    // index of center of beam
    int beam_index_x = launch_index.x / (sample_radius * 2 - 1);
    int beam_index_y = launch_index.y / (sample_radius * 2 - 1);
    float2 beam_id_fraction =
        (make_float2(beam_index_x, beam_index_y) + make_float2(0.5, 0.5)) / make_float2(global_beam_dims) * 2.f -
        1.f;  //[-1,1]

    // theta and phi for beam center
    float beam_theta = beam_id_fraction.x * c_hFOV / 2.0;
    float beam_phi = beam_id_fraction.y * c_vFOV / 2.0;

    // index of local ray in beam
    int local_ray_index_x = launch_index.x % (sample_radius * 2 - 1);
    int local_ray_index_y = launch_index.y % (sample_radius * 2 - 1);
    float2 local_ray_id_fraction = (make_float2(local_ray_index_x, local_ray_index_y) + make_float2(0.5, 0.5)) /
                                       make_float2(local_beam_dims) * 2.f -
                                   1.f;  //[-1,1]

    // relative theta and phi for local ray in beam
    float local_ray_theta = local_ray_id_fraction.x * div_angle / 2.0;
    float local_ray_phi = local_ray_id_fraction.y * div_angle / 2.0;

    // carry on ray-tracing per ray
    float theta = beam_theta + local_ray_theta;
    float phi = beam_phi + local_ray_phi;

    float xy_proj = cos(phi);

    float z = sin(phi);
    float y = xy_proj * sin(theta);
    float x = xy_proj * cos(theta);

    // origin of the camera is  0,0,0 for now
    float3 ray_origin = c_pos;
    float3 ray_direction = normalize(c_forward * x + c_left * y + c_up * z);

    // create a ray based on the calculated parameters
    optix::Ray ray(ray_origin, ray_direction, LIDAR_RAY_TYPE, scene_epsilon, max_scene_distance);

    // set the ray pay load
    PerRayData_lidar prd_lidar = make_lidar_data(0, 1.f, 0);

    // launch the ray
    rtTrace(root_node, ray, prd_lidar,RT_RAY_FLAG_DISABLE_ANYHIT);

    // set the output buffer to be what is returned in the payload
    output_buffer[launch_index] = make_float2(prd_lidar.range, prd_lidar.intensity);
}
