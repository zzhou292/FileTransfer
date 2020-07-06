// =============================================================================
// PROJECT CHRONO - http://projectchrono.org
//
// Copyright (c) 2014 projectchrono.org
// All right reserved.
//
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file at the top level of the distribution and at
// http://projectchrono.org/license-chrono.txt.
//
// =============================================================================
// Author: Milad Rakhsha
// =============================================================================

// =============================================================================

// General Includes
#include <assert.h>
#include <limits.h>
#include <stdlib.h>
#include <ctime>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "chrono/ChConfig.h"
#include "chrono/core/ChFileutils.h"
#include "chrono/core/ChTransform.h"
#include "chrono/physics/ChSystemSMC.h"
#include "chrono/solver/ChSolverMINRES.h"
#include "chrono/utils/ChUtilsCreators.h"
#include "chrono/utils/ChUtilsGenerators.h"
#include "chrono/utils/ChUtilsGeometry.h"
#include "chrono/utils/ChUtilsInputOutput.h"
#include "chrono_mkl/ChSolverMKL.h"

// Chrono fea includes
#include "chrono_fea/ChBuilderBeam.h"
#include "chrono_fea/ChElementCableANCF.h"
#include "chrono_fea/ChLinkDirFrame.h"
#include "chrono_fea/ChVisualizationFEAmesh.h"

#include "chrono_fea/ChLinkPointFrame.h"
#include "chrono_fea/ChLinkPointFrameGeneral.h"
#include "chrono_fea/ChLinkPointPoint.h"
#include "chrono_fea/ChMesh.h"
#include "chrono_fea/ChNodeFEAxyzD.h"
#include "chrono_irrlicht/ChIrrApp.h"

// Chrono namespaces
using namespace chrono;
using namespace chrono::fea;
using namespace chrono::collision;
using namespace chrono::irrlicht;
using namespace irr;

void Create_MB_FE(ChSystemSMC& mphysicalSystemH, std::shared_ptr<fea::ChMesh> my_mesh);

int main(int argc, char* argv[]) {
    ChSystemSMC mphysicalSystem;
    ChIrrApp application(&mphysicalSystem, L"Test ANCF Cables", core::dimension2d<u32>(1600, 1200), false, true);
    application.AddTypicalLogo();
    application.AddTypicalSky();
    application.AddTypicalLights();
    application.AddTypicalCamera(core::vector3df(0.001f, -0.05f, 0.1f));

    auto my_mesh = std::make_shared<fea::ChMesh>();
    Create_MB_FE(mphysicalSystem, my_mesh);

    auto mvisualizebeamA = std::make_shared<ChVisualizationFEAmesh>(*(my_mesh.get()));
    mvisualizebeamA->SetFEMdataType(ChVisualizationFEAmesh::E_PLOT_NODE_DISP_NORM);
    mvisualizebeamA->SetColorscaleMinMax(0.0, 0.001);
    mvisualizebeamA->SetSmoothFaces(true);
    mvisualizebeamA->SetWireframe(false);
    my_mesh->AddAsset(mvisualizebeamA);

    application.AssetBindAll();
    application.AssetUpdateAll();
    mphysicalSystem.SetupInitial();

    int step_count = 0;
    double mTime = 0;

#ifdef CHRONO_MKL
    auto mkl_solver = std::make_shared<ChSolverMKL<>>();
    mkl_solver->SetSparsityPatternLock(true);
    mphysicalSystem.SetSolver(mkl_solver);
#else
    mphysicalSystem.SetSolverType(ChSolver::Type::MINRES);
    mphysicalSystem.SetSolverWarmStarting(true);
    mphysicalSystem.SetTolForce(1e-10);
    mphysicalSystem.SetMaxItersSolverSpeed(10000);
    mphysicalSystem.SetMaxItersSolverStab(10000);
#endif

    // Set up integrator
    //    mphysicalSystem.SetTimestepperType(ChTimestepper::Type::EULER_IMPLICIT);
    //    my_system.SetTimestepperType(ChTimestepper::Type::EULER_IMPLICIT_LINEARIZED);  // fast, less precise

    mphysicalSystem.SetTimestepperType(ChTimestepper::Type::HHT);
    auto mystepper = std::dynamic_pointer_cast<ChTimestepperHHT>(mphysicalSystem.GetTimestepper());
    mystepper->SetAlpha(-0.2);
    mystepper->SetMaxiters(100);
    mystepper->SetAbsTolerances(1e-10, 1e-10);
    mystepper->SetMode(ChTimestepperHHT::POSITION);
    mystepper->SetScaling(true);
    mystepper->SetVerbose(false);

    int stepEnd = 1000000;
    double dt = 0.01;
    int fps = 50;
    double time = 0;
    application.SetTimestep(dt);

    while (application.GetDevice()->run()) {
        application.BeginScene();
        application.DrawAll();
        application.DoStep();
        application.EndScene();
        std::cout << mphysicalSystem.GetChTime() << std::endl;
    }

    return 0;
}

void Create_MB_FE(ChSystemSMC& mphysicalSystem, std::shared_ptr<fea::ChMesh> my_mesh) {
    mphysicalSystem.Set_G_acc(ChVector<>(0.01, 0.0, 0.0));

    double initSpace0 = 0.0005;
    double Fiber_Diameter = initSpace0;
    double Area = 3.1415 * std::pow(Fiber_Diameter, 2) / 4;
    double E = 5e2;
    double nu = 0.3;
    double rho = 2000;
    /*================== Cable Elements =================*/
    auto msection_cable = std::make_shared<ChBeamSectionCable>();
    msection_cable->SetDiameter(Fiber_Diameter);
    msection_cable->SetYoungModulus(E);
    msection_cable->SetDensity(rho);
    msection_cable->SetBeamRaleyghDamping(0.02);

    double plate_lenght_x = 0.032;
    double plate_lenght_y = 0.032;
    double Height = 0.02;

    int N_x = 4;
    int N_y = 4;
    int numCableElems_Per_Fiber = 5;

    // For uniform grid
    double dx = plate_lenght_x / (N_x - 1);
    double dy = plate_lenght_y / (N_y - 1);

    for (int j = 0; j < N_y; j++) {
        for (int i = 0; i < N_x; i++) {
            ChBuilderBeamANCF builder;
            double loc_x = i * dx - plate_lenght_x / 2;
            double loc_y = j * dy - plate_lenght_y / 2;
            builder.BuildBeam(
                my_mesh,                          // the mesh where to put the created nodes and elements
                msection_cable,                   // the ChBeamSectionCable to use for the ChElementBeamANCF elements
                numCableElems_Per_Fiber,          // the number of ChElementBeamANCF to create
                ChVector<>(loc_x, loc_y, 0.0),    // the 'A' point in space (beginning of beam)
                ChVector<>(loc_x, loc_y, Height)  // the 'B' point in space (end of beam) _1D_elementsNodes_mesh,
            );
        }
    }

    /// Fix the nodes attached to the ground
    for (int i = 0; i < my_mesh->GetNnodes(); i++) {
        auto node = std::dynamic_pointer_cast<ChNodeFEAxyzD>(my_mesh->GetNode(i));
        if (node->GetPos().z() <= 0.0) {
            node->SetFixed(true);
        }
    }

    mphysicalSystem.Add(my_mesh);
}
