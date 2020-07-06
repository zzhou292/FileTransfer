// =============================================================================
// PROJECT CHRONO - http://projectchrono.org
//
// Copyright (c) 2014 projectchrono.org
// All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file at the top level of the distribution and at
// http://projectchrono.org/license-chrono.txt.
//
// =============================================================================
// Authors: Milad Rakhsha
// =============================================================================
//
//   Demo code about
//     - test force/torque accumulators
//
// =============================================================================

#include "chrono/assets/ChTexture.h"
#include "chrono/physics/ChBodyEasy.h"
#include "chrono/physics/ChSystemSMC.h"
#include "chrono/utils/ChUtilsCreators.h"
#include "chrono/utils/ChUtilsGenerators.h"
#include "chrono/utils/ChUtilsGeometry.h"
#include "chrono_irrlicht/ChIrrApp.h"

// Use the namespaces of Chrono
using namespace chrono;
using namespace chrono::irrlicht;

// Use the main namespaces of Irrlicht
using namespace irr;
using namespace irr::core;
using namespace irr::scene;
using namespace irr::video;
using namespace irr::io;
using namespace irr::gui;

void AddWall(std::shared_ptr<ChBody> body, const ChVector<>& dim, const ChVector<>& loc) {
    body->GetCollisionModel()->AddBox(dim.x(), dim.y(), dim.z(), loc);
    auto box = std::make_shared<ChBoxShape>();
    box->GetBoxGeometry().Size = dim;
    box->GetBoxGeometry().Pos = loc;
}

void build_system(ChSystemSMC& mphysicalSystem) {
    auto mysurfmaterial = std::make_shared<ChMaterialSurfaceSMC>();
    /// Set common material Properties
    mysurfmaterial->SetYoungModulus(1e8);
    mysurfmaterial->SetFriction(0.2f);
    mysurfmaterial->SetRestitution(0.05);
    mysurfmaterial->SetAdhesion(0);

    double bxDim = 2000;
    double byDim = 2000;
    double bzDim = 300;
    double fxDim = 1000;
    double fyDim = 1000;
    double fzDim = 200;
    /// Bottom wall
    double initSpace0 = 20;
    ChVector<> sizeBottom(bxDim / 2 + 3 * initSpace0, byDim / 2 + 3 * initSpace0, 2 * initSpace0);
    ChVector<> posBottom(0, 0, -2 * initSpace0);
    ChVector<> posTop(0, 0, bzDim + 2 * initSpace0);
    /// left and right Wall
    ChVector<> size_YZ(2 * initSpace0, byDim / 2 + 3 * initSpace0, bzDim / 2);
    ChVector<> pos_xp(bxDim / 2 + initSpace0, 0.0, bzDim / 2 + 1 * initSpace0);
    ChVector<> pos_xn(-bxDim / 2 - 3 * initSpace0, 0.0, bzDim / 2 + 1 * initSpace0);
    /// Front and back Wall
    ChVector<> size_XZ(bxDim / 2, 2 * initSpace0, bzDim / 2);
    ChVector<> pos_yp(0, byDim / 2 + initSpace0, bzDim / 2 + 1 * initSpace0);
    ChVector<> pos_yn(0, -byDim / 2 - 3 * initSpace0, bzDim / 2 + 1 * initSpace0);

    /// Create a container
    auto bin = std::make_shared<ChBody>(ChMaterialSurface::SMC);
    bin->SetPos(ChVector<>(0.0, 0.0, 0.0));
    bin->SetRot(ChQuaternion<>(1, 0, 0, 0));
    bin->SetIdentifier(-1);
    bin->SetBodyFixed(true);
    bin->GetCollisionModel()->ClearModel();
    bin->GetCollisionModel()->SetSafeMargin(initSpace0 / 2);
    bin->SetMaterialSurface(mysurfmaterial);
    /// MBD representation of the walls
    AddWall(bin, sizeBottom, posBottom);
    AddWall(bin, sizeBottom, posTop + ChVector<>(0.0, 0.0, 3 * initSpace0));
    AddWall(bin, size_YZ, pos_xp);
    AddWall(bin, size_YZ, pos_xn);
    AddWall(bin, size_XZ, pos_yp + ChVector<>(+1.5 * initSpace0, +1.5 * initSpace0, 0.0));
    AddWall(bin, size_XZ, pos_yn + ChVector<>(-0.5 * initSpace0, -0.5 * initSpace0, 0.0));
    bin->GetCollisionModel()->BuildModel();
    bin->SetCollide(true);
    mphysicalSystem.AddBody(bin);

    double h_length = 60;
    double h_height = 80;
    ChVector<> h_size(h_length, h_length, h_height);
    ChVector<double> body_pos(0, 0, 0);
    std::shared_ptr<ChBody> body = std::make_shared<ChBody>(ChMaterialSurface::SMC);
    body->SetPos(body_pos);
    body->SetRot(QUNIT);
    double volume = h_length * h_length * h_height;
    double density = 5000;
    double mass = density * volume;
    ChMatrix33<> gyration = utils::CalcBoxGyration(h_size);
    body->SetMass(mass);
    body->SetInertia(gyration * mass);
    body->SetCollide(true);
    body->SetBodyFixed(false);

    utils::AddBoxGeometry(body.get(), h_size);
    body->GetCollisionModel()->BuildModel();
    mphysicalSystem.AddBody(body);
    body->Empty_forces_accumulators();
    body->Accumulate_torque(ChVector<>(0., 0, 1e11), false);
    ChVector<> gravity(0);
    mphysicalSystem.Set_G_acc(gravity);
}

int main(int argc, char* argv[]) {
    GetLog() << "Copyright (c) 2017 projectchrono.org\nChrono version: " << CHRONO_VERSION << "\n\n";

    // Create a ChronoENGINE physical system
    ChSystemSMC mphysicalSystem;

    // Create the Irrlicht visualization (open the Irrlicht device,
    // bind a simple user interface, etc. etc.)
    ChIrrApp application(&mphysicalSystem, L"Bricks test", core::dimension2d<u32>(800, 600), false, true);

    // Easy shortcuts to add camera, lights, logo and sky in Irrlicht scene:
    ChIrrWizard::add_typical_Logo(application.GetDevice());
    ChIrrWizard::add_typical_Sky(application.GetDevice());
    ChIrrWizard::add_typical_Lights(application.GetDevice(), core::vector3df(70.f, 120.f, -90.f),
                                    core::vector3df(30.f, 80.f, 60.f), 290, 190);
    ChIrrWizard::add_typical_Camera(application.GetDevice(), core::vector3df(-500, 0., -500), core::vector3df(0, 0, 0));

    build_system(mphysicalSystem);
    application.AssetBindAll();
    application.AssetUpdateAll();
    mphysicalSystem.SetSolverType(ChSolver::Type::SOR);  // SOR_MULTITHREAD);
    mphysicalSystem.SetParallelThreadNumber(4);
    mphysicalSystem.SetMaxItersSolverSpeed(40);
    mphysicalSystem.SetSolverWarmStarting(false);
    mphysicalSystem.SetMaxPenetrationRecoverySpeed(1.0);
    application.SetStepManage(true);
    application.SetTimestep(0.01);

    while (application.GetDevice()->run()) {
        application.BeginScene(true, true, SColor(255, 140, 161, 192));

        ChIrrTools::drawGrid(application.GetVideoDriver(), 5, 5, 20, 20,
                             ChCoordsys<>(ChVector<>(0, 0.04, 0), Q_from_AngAxis(CH_C_PI / 2, VECT_X)),
                             video::SColor(50, 90, 90, 150), true);
        application.DrawAll();
        application.DoStep();
        application.EndScene();
    }

    return 0;
}
