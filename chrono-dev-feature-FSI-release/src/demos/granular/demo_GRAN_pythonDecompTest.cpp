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
// Authors: Nic Olsen
// =============================================================================
// Chrono::Granular demo demonstrating some useful utilities for terrain
// initialization. The PDLayerSampler generates material in layers according to
// the Poisson Disk sampler. The terrain is read in from a triangle OBJ mesh
// file and decomposed into spheres which are set as fixed for the simulation.
// Note that the sphere size should be small enough to capture each triangle.
// =============================================================================

#include <iostream>
#include <vector>
#include <string>

#include "chrono/core/ChGlobal.h"
#include "chrono/utils/ChUtilsSamplers.h"

#include "chrono_thirdparty/filesystem/path.h"

#include "chrono_granular/api/ChApiGranularChrono.h"
#include "chrono_granular/physics/ChGranular.h"
#include "chrono_granular/utils/ChGranularJsonParser.h"
#include "chrono_granular/utils/ChGranularSphereDecomp.h"
#include "chrono_granular/utils/ChGranularSphereImport.h"

using namespace chrono;
using namespace chrono::granular;

void ShowUsage(std::string name) {
    std::cout << "usage: " + name + " <json_file>" << std::endl;
}

int main(int argc, char* argv[]) {
    sim_param_holder params;
    if (argc != 2 || ParseJSON(argv[1], params, true) == false) {
        ShowUsage(argv[0]);
        return 1;
    }

    ChSystemGranularSMC gran_sys(params.sphere_radius, params.sphere_density,
                                 make_float3(params.box_X, params.box_Y, params.box_Z));

    ChGranularSMC_API apiSMC;
    apiSMC.setGranSystem(&gran_sys);

   //  Add spherically-decomposed underlying terrain.
    std::string csvfilename("/home/jason/Desktop/FileTransfer/chrono-dev-feature-FSI-release/data/granular/demo_GRAN_fixedterrain/sph_decomp_jz.csv");

    ChVector<float> scaling(params.box_X / 2, params.box_Y / 2, params.box_Z);

    ChVector<float> offset(0, 0, -params.box_Z / 2);

    std::cout<<"--------------!!!!!!!--------"<<std::endl;
    //float radius = 0.25;
        std::vector<ChVector<double>> terrain_points =
        ImportSphericalDecomposition<double>(csvfilename, params.sphere_radius);
    std::cout<<"------------????????????????--------"<<std::endl;
    
}