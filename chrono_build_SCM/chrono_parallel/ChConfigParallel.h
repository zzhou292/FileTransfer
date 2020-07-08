// =============================================================================
// PROJECT CHRONO - http://projectchrono.org
//
// Copyright (c) 2016 projectchrono.org
// All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file at the top level of the distribution and at
// http://projectchrono.org/license-chrono.txt.
//
// =============================================================================
// Authors: Radu Serban
// =============================================================================
//
// Chrono-Parallel configuration header file
//
// Automatically created during CMake configuration.
//
// =============================================================================

#ifndef CH_CONFIG_PARALLEL_H
#define CH_CONFIG_PARALLEL_H

// Include main Chrono configuration header
#include "chrono/ChConfig.h"

// If using double precision
//   #define CHRONO_PARALLEL_USE_DOUBLE
#define CHRONO_PARALLEL_USE_DOUBLE

// If using SIMD
//   #define CHRONO_PARALLEL_USE_SIMD
#define CHRONO_PARALLEL_USE_SIMD

// If using cuda in parallel module
//   #define CHRONO_PARALLEL_USE_CUDA
#undef CHRONO_PARALLEL_USE_CUDA


#endif
