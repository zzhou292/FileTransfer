
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
// Authors: Asher Elmquist, Han Wang
// =============================================================================
//
// Chrono demonstration of applying texture
//
// =============================================================================

#include <cmath>
#include <cstdio>
#include <iomanip>

#include "chrono/assets/ChVisualization.h"
#include "chrono/physics/ChSystemNSC.h"
#include "chrono/physics/ChBodyEasy.h"

#include "chrono_sensor/ChCameraSensor.h"
#include "chrono_sensor/ChSensorManager.h"
#include "chrono_sensor/filters/ChFilterVisualize.h"
#include "chrono_sensor/filters/ChFilterSave.h"

using namespace chrono;
using namespace chrono::geometry;
using namespace chrono::sensor;

// -----------------------------------------------------------------------------
// Camera parameters
// -----------------------------------------------------------------------------

// Update rate in Hz
int update_rate = 30;

// Image width and height
unsigned int image_width = 1280;
unsigned int image_height = 720;

// Camera's horizontal field of view
float fov = CH_C_PI / 3.;

// Lag (in seconds) between sensing and when data becomes accessible
float lag = 0;

// Exposure (in seconds) of each image
float exposure_time = 0;

// -----------------------------------------------------------------------------
// Simulation parameters
// -----------------------------------------------------------------------------

// Texture files
const std::string normalmap = "sensor/textures/brick_normal.png";
const std::string kdmap = "sensor/textures/brick.png";

// Simulation step size
double step_size = 1e-3;

// Simulation end time
float end_time = 20.0f;

// Save camera images
bool save = true;

// Render camera images
bool vis = true;

// Output directory
const std::string out_dir = "SENSOR_OUTPUT/TEXTURE_DEMO/";

int main(int argc, char* argv[]) {
    GetLog() << "Copyright (c) 2020 projectchrono.org\nChrono version: " << CHRONO_VERSION << "\n\n";

    //--------------------
    // Create the system
    //--------------------
    ChSystemNSC mphysicalSystem;

    //----------------------------------------------------------------------------------
    // Create the surface and visual materials with textures to be applied to the bodies
    //----------------------------------------------------------------------------------
    auto phys_mat = chrono_types::make_shared<ChMaterialSurfaceNSC>();
    phys_mat->SetFriction(0.2f);

    // Create a visual material with an attached color
    auto vis_mat_onlycolor = std::make_shared<ChVisualMaterial>();
    vis_mat_onlycolor->SetKdTexture(GetChronoDataFile(kdmap));

    // Create a visual material with only normal map texture
    auto vis_mat_onlynormal = std::make_shared<ChVisualMaterial>();
    vis_mat_onlynormal->SetNormalMapTexture(GetChronoDataFile(normalmap));

    // Create a visual material with both normal map and color
    auto vis_mat_both = std::make_shared<ChVisualMaterial>();
    vis_mat_both->SetKdTexture(GetChronoDataFile(kdmap));
    vis_mat_both->SetNormalMapTexture(GetChronoDataFile(normalmap));

    //----------------------------------------
    // Create a floor and add it to the system
    //----------------------------------------
    auto floor = chrono_types::make_shared<ChBodyEasyBox>(100, 100, 1,  // x,y,z size
                                                          1000,         // density
                                                          true,         // collide enable?
                                                          true,         // visualization
                                                          phys_mat);    // physical material
    floor->SetPos({0, 0, -1.0});
    floor->SetRot(Q_from_AngZ(CH_C_PI / 2.0));
    floor->SetBodyFixed(true);
    auto floor_asset = floor->GetAssets()[0];
    mphysicalSystem.Add(floor);

    //----------------------------------
    // Create a box without a normal map
    //----------------------------------
    auto box = chrono_types::make_shared<ChBodyEasyBox>(.9, .9, .9,    // x,y,z size
                                                        1000,       // density
                                                        true,       // collide enable?
                                                        true,       // visualization
                                                        phys_mat);  // physical material
    box->SetPos({-0.9, 0, 0});
    box->SetBodyFixed(true);
    auto box_asset = box->GetAssets()[0];
    if (std::shared_ptr<ChVisualization> visual_asset = std::dynamic_pointer_cast<ChVisualization>(box_asset)) {
        visual_asset->material_list.push_back(vis_mat_onlycolor);
    }
    mphysicalSystem.Add(box);

    //-------------------------------------
    // Create another box with only normal map
    //-------------------------------------
    auto box1 = chrono_types::make_shared<ChBodyEasyBox>(1.0f, 1.0f, 1.0f,  // x,y,z size
                                                         1000,           // density
                                                         true,           // collide enable?
                                                         true,           // visualization
                                                         phys_mat);      // physical material
    box1->SetPos({0.2, 0, 0});
    box1->SetBodyFixed(true);
    auto box1_asset = box1->GetAssets()[0];
    if (std::shared_ptr<ChVisualization> visual_asset = std::dynamic_pointer_cast<ChVisualization>(box1_asset)) {
        visual_asset->material_list.push_back(vis_mat_onlynormal);
    }
    mphysicalSystem.Add(box1);

    //--------------------------------------------------
    // Create another box with both normal map and color
    //--------------------------------------------------
    auto box2 = chrono_types::make_shared<ChBodyEasyBox>(1.1, 1.1, 1.1,  // x,y,z size
                                                        1000,            // density
                                                        true,            // collide enable?
                                                        true,            // visualization
                                                        phys_mat);       // phyiscal material
    box2->SetPos({1.4, 0, 0});
    box2->SetBodyFixed(true);
    auto box2_asset = box2->GetAssets()[0];
    if (std::shared_ptr<ChVisualization> visual_asset = std::dynamic_pointer_cast<ChVisualization>(box2_asset)) {
        visual_asset->material_list.push_back(vis_mat_both);
    }
    mphysicalSystem.Add(box2);

    // -----------------------
    // Create a sensor manager
    // -----------------------
    auto manager = chrono_types::make_shared<ChSensorManager>(&mphysicalSystem);
    manager->scene->AddPointLight({-50, -50, 50}, {1, 1, 1}, 1000);

    // Get the point lights
    std::vector<PointLight>& lights = manager->scene->GetPointLights();

    // ------------------------------------------------
    // Create a camera and add it to the sensor manager
    // ------------------------------------------------
    chrono::ChFrame<double> offset_pose({-2, 0, 2}, Q_from_AngAxis(0, {1, 0, 0}));
    auto cam = chrono_types::make_shared<ChCameraSensor>(floor,         // body camera is attached to
                                                         update_rate,   // update rate in Hz
                                                         offset_pose,   // offset pose
                                                         image_width,   // image width
                                                         image_height,  // image height
                                                         fov,           // camera's horizontal field of view
                                                         lag,           // lag
                                                         exposure_time  // exposure time
    );
    cam->SetName("Top View Camera");

    if (vis)
        // Visualize the image
        cam->PushFilter(std::make_shared<ChFilterVisualize>(image_width, image_height));

    if (save)
        // Save the current image to a png file at the specified path
        cam->PushFilter(chrono_types::make_shared<ChFilterSave>(out_dir + "top_cam/"));

    // add sensor to the manager
    manager->AddSensor(cam);

    // -------------------------------------------------------
    // Create a second camera and add it to the sensor manager
    // -------------------------------------------------------

    chrono::ChFrame<double> offset_pose2({0, 0, 3}, Q_from_AngAxis(0, {1, 1, 1}));
    auto cam2 = chrono_types::make_shared<ChCameraSensor>(floor,          // body camera is attached to
                                                          update_rate,    // update rate in Hz
                                                          offset_pose2,   // offset pose
                                                          image_width,    // image width
                                                          image_height,   // image height
                                                          fov,            // camera's horizontal field of view
                                                          lag,            // lag
                                                          exposure_time,  // exposure time
                                                          3               // Super sample factor
    );
    cam2->SetName("Side View Camera");

    if (vis)
        // Visualize the image
        cam2->PushFilter(std::make_shared<ChFilterVisualize>(image_width, image_height));

    if (save)
        // Save the current image to a png file at the specified path
        cam2->PushFilter(chrono_types::make_shared<ChFilterSave>(out_dir + "side_cam/"));

    // add sensor to the manager
    manager->AddSensor(cam2);

    //----------------
    // Simulate system
    //----------------
    float orbit_radius = 5;
    float orbit_rate = .4;
    float ch_time = 0.0;

    while (ch_time < end_time) {
        // Rotate the cameras around the mesh at a fixed rate
        cam->SetOffsetPose(chrono::ChFrame<double>(
            {-orbit_radius * cos(ch_time * orbit_rate), -orbit_radius * sin(ch_time * orbit_rate), 3},
            Q_from_AngAxis(ch_time * orbit_rate, {0, 0, 1}) * Q_from_AngAxis(.5, {0, 1, 0})));

        cam2->SetOffsetPose(chrono::ChFrame<double>(
            {-orbit_radius * cos(ch_time * orbit_rate), -orbit_radius * sin(ch_time * orbit_rate), 1},
            Q_from_AngAxis(ch_time * orbit_rate, {0, 0, 1})));

        // Reposition the point lights
        // lights[0].pos = {-orbit_radius * cos(ch_time * orbit_rate), -orbit_radius * sin(ch_time * orbit_rate), 10};

        // Update sensor manager
        // Will render/save/filter automatically
        manager->Update();

        // Perform step of dynamics
        mphysicalSystem.DoStepDynamics(step_size);

        // Get the current time of the simulation
        ch_time = (float)mphysicalSystem.GetChTime();
    }
}
