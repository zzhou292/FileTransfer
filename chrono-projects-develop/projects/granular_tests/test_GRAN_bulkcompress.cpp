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
// Chrono::Granular simulation of granular material being compressed by a mass
// allowed to oscillate on top of the material.
// =============================================================================

#include <cmath>
#include <iostream>
#include <string>

#include "chrono/physics/ChBody.h"
#include "chrono/physics/ChSystemSMC.h"
#include "chrono/utils/ChUtilsSamplers.h"
#include "chrono_granular/ChGranularData.h"
#include "chrono_granular/api/ChApiGranularChrono.h"
#include "chrono_granular/physics/ChGranular.h"
#include "chrono_granular/physics/ChGranularTriMesh.h"
#include "chrono_granular/utils/ChGranularJsonParser.h"
#include "chrono_thirdparty/filesystem/path.h"

using namespace chrono;
using namespace chrono::granular;

/*
 * Lommen 2014:
 * Settle particles then drop box
 * 18,000 particles
 * Settled for 1 s
 * Poisson Ratio = 0.25
 * Coefficient of Restitution = 0.5
 * Static friction = 0.5
 * Rolling Friction = 0.04
 * Drop height (above material)= 0.2 m
 * Box mass = 50 kg
 * Periodic boundary condition in x and y
 * Time after release = 3 s
 * Varied Shear modulus
 *
 * Assumed:
 * Particle diameter = 4, 8, or 16 mm
 * Density 1500 kg/m3
 * Settled height ~0.25 m
 */

const double time_settle = 1;
double fill_top;

const double block_mass = 50000;  // 50kg
const double drop_height = 20;    // 0.2m

void ShowUsage(std::string name) {
    std::cout << "usage: " + name + " <json_file> <output_dir>" << std::endl;
}

void SetupGranSystem(ChGranularChronoTriMeshAPI& apiSMC_TriMesh, sim_param_holder& params, std::string out_dir) {
    ChSystemGranularSMC_trimesh& gran_sys = apiSMC_TriMesh.getGranSystemSMC_TriMesh();

    gran_sys.set_K_n_SPH2SPH(params.normalStiffS2S);
    gran_sys.set_K_n_SPH2WALL(params.normalStiffS2W);
    gran_sys.set_K_n_SPH2MESH(params.normalStiffS2M);
    gran_sys.set_K_t_SPH2SPH(params.tangentStiffS2S);
    gran_sys.set_K_t_SPH2WALL(params.tangentStiffS2W);
    gran_sys.set_K_t_SPH2MESH(params.tangentStiffS2M);
    gran_sys.set_Gamma_n_SPH2SPH(params.normalDampS2S);
    gran_sys.set_Gamma_n_SPH2WALL(params.normalDampS2W);
    gran_sys.set_Gamma_n_SPH2MESH(params.normalDampS2M);
    gran_sys.set_Gamma_t_SPH2SPH(params.tangentDampS2S);
    gran_sys.set_Gamma_t_SPH2WALL(params.tangentDampS2W);
    gran_sys.set_Gamma_t_SPH2MESH(params.tangentDampS2M);

    gran_sys.set_Cohesion_ratio(params.cohesion_ratio);
    gran_sys.set_Adhesion_ratio_S2M(params.adhesion_ratio_s2m);
    gran_sys.set_Adhesion_ratio_S2W(params.adhesion_ratio_s2w);
    gran_sys.set_gravitational_acceleration(params.grav_X, params.grav_Y, params.grav_Z);
    gran_sys.set_friction_mode(chrono::granular::GRAN_FRICTION_MODE::SINGLE_STEP);

    gran_sys.setOutputMode(GRAN_OUTPUT_MODE::CSV);
    filesystem::create_directory(filesystem::path(out_dir));

    gran_sys.set_timeIntegrator(GRAN_TIME_INTEGRATOR::FORWARD_EULER);
    gran_sys.set_fixed_stepSize(params.step_size);
    gran_sys.set_BD_Fixed(true);

    double epsilon = 0.2 * params.sphere_radius;
    double spacing = 2 * params.sphere_radius + epsilon;

    std::vector<ChVector<float>> body_points;

    // utils::HCPSampler<float> sampler(spacing);
    utils::PDSampler<float> sampler(2 * params.sphere_radius + epsilon);
    double fill_bottom = -params.box_Z / 2 + params.sphere_radius + epsilon;
    fill_top = params.box_Z / 2 - params.sphere_radius - epsilon;
    ChVector<> hdims(params.box_X / 2 - params.sphere_radius - epsilon,
                     params.box_Y / 2 - params.sphere_radius - epsilon, 0);
    for (double z = fill_bottom; z < fill_top; z += spacing) {
        ChVector<> center(0, 0, z);
        auto points = sampler.SampleBox(center, hdims);
        body_points.insert(body_points.end(), points.begin(), points.end());
    }

    std::cout << "Created " << body_points.size() << " spheres" << std::endl;

    apiSMC_TriMesh.setElemsPositions(body_points);

    // Mesh values
    std::vector<string> mesh_filenames;
    std::string mesh_filename = granular::GetDataFile("test_GRAN_bulkcompress/downward_square.obj");
    mesh_filenames.push_back(mesh_filename);

    std::vector<ChMatrix33<float>> mesh_rotscales;
    std::vector<float3> mesh_translations;

    ChMatrix33<float> scaling(ChVector<float>(params.box_X / 2, params.box_Y / 2, 1));
    mesh_rotscales.push_back(scaling);
    mesh_translations.push_back(make_float3(0, 0, 0));

    std::vector<float> mesh_masses;
    mesh_masses.push_back(block_mass);

    apiSMC_TriMesh.load_meshes(mesh_filenames, mesh_rotscales, mesh_translations, mesh_masses);
}

int main(int argc, char* argv[]) {
    granular::SetDataPath(std::string(PROJECTS_DATA_DIR) + "granular/");
    sim_param_holder params;

    if (argc != 3 || ParseJSON(argv[1], params) == false) {
        ShowUsage(argv[0]);
        return 1;
    }

    float iteration_step = params.step_size;
    ChGranularChronoTriMeshAPI apiSMC_TriMesh(params.sphere_radius, params.sphere_density,
                                              make_float3(params.box_X, params.box_Y, params.box_Z));

    std::string out_dir(argv[2]);
    SetupGranSystem(apiSMC_TriMesh, params, out_dir);
    ChSystemGranularSMC_trimesh& gran_sys = apiSMC_TriMesh.getGranSystemSMC_TriMesh();

    ChSystemSMC ch_sys;
    ch_sys.Set_G_acc(ChVector<>(params.grav_X, params.grav_Y, params.grav_Z));
    auto block = std::make_shared<ChBody>();
    block->SetBodyFixed(true);
    block->SetPos(ChVector<>(0, 0, params.box_Z));
    block->SetMass(block_mass);
    ch_sys.AddBody(block);

    unsigned int nSoupFamilies = gran_sys.getNumTriangleFamilies();
    std::cout << nSoupFamilies << " soup families" << std::endl;
    double* meshSoupLocOri = new double[7 * nSoupFamilies];
    float* meshVel = new float[6 * nSoupFamilies]();

    gran_sys.initialize();
    unsigned int currframe = 0;
    double out_fps = 100;
    float frame_step = 1.f / out_fps;  // Duration of a frame
    unsigned int out_steps = frame_step / iteration_step;
    std::cout << "out_steps " << out_steps << std::endl;

    unsigned int step = 0;
    bool box_released = false;
    gran_sys.disableMeshCollision();
    for (float t = 0; t < params.time_end; t += iteration_step, step++) {
        if (t >= time_settle && box_released == false) {
            gran_sys.enableMeshCollision();

            block->SetBodyFixed(false);
            double max_z = gran_sys.get_max_z();
            block->SetPos(ChVector<>(0, 0, max_z + params.sphere_radius + drop_height));

            box_released = true;
            std::cout << "Releasing box" << std::endl;
        }

        meshSoupLocOri[0] = 0;
        meshSoupLocOri[1] = 0;
        meshSoupLocOri[2] = block->GetPos().z();

        meshSoupLocOri[3] = 1;
        meshSoupLocOri[4] = 0;
        meshSoupLocOri[5] = 0;
        meshSoupLocOri[6] = 0;

        gran_sys.meshSoup_applyRigidBodyMotion(meshSoupLocOri, meshVel);
        if (step % out_steps == 0) {
            std::cout << "Rendering frame " << currframe << std::endl;
            char filename[100];
            sprintf(filename, "%s/step%06u", out_dir.c_str(), currframe++);
            gran_sys.writeFile(std::string(filename));
            gran_sys.write_meshes(std::string(filename));
            if (box_released) {
                std::cout << block->GetPos().z() << std::endl;
            }
        }

        float forces[6];
        gran_sys.collectGeneralizedForcesOnMeshSoup(forces);

        ch_sys.DoStepDynamics(iteration_step);
        gran_sys.advance_simulation(iteration_step);

        block->Empty_forces_accumulators();
        block->Accumulate_force(ChVector<>(0, 0, forces[2]), block->GetPos(), false);
    }

    delete[] meshSoupLocOri;

    return 0;
}