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
// Container class for a lidar sensor
//
// =============================================================================

#include "chrono_sensor/ChLidarSensor.h"
#include "chrono_sensor/filters/ChFilterLidarReduce.h"
#include "chrono_sensor/filters/ChFilterOptixRender.h"

namespace chrono {
namespace sensor {

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
CH_SENSOR_API ChLidarSensor::ChLidarSensor(std::shared_ptr<chrono::ChBody> parent,
                                           float updateRate,
                                           chrono::ChFrame<double> offsetPose,
                                           unsigned int w,               // image width
                                           unsigned int h,               // image height
                                           float hFOV,                   // horizontal field of view
                                           float vFOV,                   // vertical field of view
                                           float lag,                    // lidar lag time
                                           float collection_time,        // collection time
                                           unsigned int sample_radius,   // radius of the beam samples
                                           float divergence_angle,       // divergence angle of the beam
                                           LidarReturnMode return_mode,  // return mode of the lidar
                                           LidarModelType lidar_model    // lidar model for generating data
                                           )
    : m_sample_radius(sample_radius),
      m_divergence_angle(divergence_angle),
      m_return_mode(return_mode),
      m_hFOV(hFOV),
      m_vFOV(vFOV),
      m_model_type(lidar_model),
      ChOptixSensor(parent,
                    updateRate,
                    offsetPose,
                    w * (2 * sample_radius - 1),
                    h * (2 * sample_radius - 1),
                    lag,
                    collection_time) {
    // set the program to match the model requested
    switch (lidar_model) {
        default:  // same as RAYCAST
            // select if we should use multisample ray casting method
            if (sample_radius > 1) {
                m_program_string = {"lidar", "multi_sample"};
                m_ray_launch_params.push_back(std::make_tuple<std::string, RTobjecttype, void*>(
                    "divergence_angle", RT_OBJECTTYPE_FLOAT, &m_divergence_angle));
                m_ray_launch_params.push_back(std::make_tuple<std::string, RTobjecttype, void*>(
                    "ray_samples", RT_OBJECTTYPE_INT, &m_sample_radius));
                m_filters.push_back(
                    chrono_types::make_shared<ChFilterLidarReduce>(return_mode, sample_radius, "lidar reduction"));

            } else {
                m_program_string = {"lidar", "spherical"};
            }

            m_buffer_format = RT_FORMAT_FLOAT2;

            break;
    }

    // list of parameters to pass to the ray generation program
    m_ray_launch_params.push_back(
        std::make_tuple<std::string, RTobjecttype, void*>("c_hFOV", RT_OBJECTTYPE_FLOAT, &m_hFOV));

    m_ray_launch_params.push_back(
        std::make_tuple<std::string, RTobjecttype, void*>("c_vFOV", RT_OBJECTTYPE_FLOAT, &m_vFOV));
}
// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
CH_SENSOR_API ChLidarSensor::~ChLidarSensor() {}

}  // namespace sensor
}  // namespace chrono
