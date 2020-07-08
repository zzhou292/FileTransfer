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
// Chrono-Sensor configuration header file
//
// Automatically created during CMake configuration.
//
// =============================================================================

#ifndef CH_CONFIG_SENSOR_H
#define CH_CONFIG_SENSOR_H

// Include main Chrono configuration  header
#include "chrono/ChConfig.h"

/// passing lists from cmake to c++ when using NVRTC for runtime compilation of RT Programs
#define CUDA_NVRTC_INCLUDE_LIST  \
  "/home/jason/Desktop/NVIDIA-OptiX-SDK-6.0.0-linux64/include", \
  "/home/jason/Desktop/NVIDIA-OptiX-SDK-6.0.0-linux64/include/optixu", \
  "/opt/cuda/include", \
  "/home/jason/Desktop/FileTransfer/chrono-dev-5985d8c5e92899005e8022fcf8ee6a959d758d63/src/chrono_sensor/rtkernels", \
  "/home/jason/Desktop/FileTransfer/chrono-dev-5985d8c5e92899005e8022fcf8ee6a959d758d63/src", \
  0,
#define CUDA_NVRTC_FLAG_LIST  \
  "-use_fast_math", \
  "-default-device", \
  "-rdc", \
  "true", \
  "-D__x86_64", \
  0,

#endif
