#include <assert.h>
#include <limits.h>
#include <stdlib.h>
#include <ctime>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "chrono/ChConfig.h"
#include "chrono/physics/ChSystemSMC.h"
#include "chrono/solver/ChSolverMINRES.h"
#include "chrono_mkl/ChSolverMKL.h"

// Chrono fea includes
#include "chrono_fea/ChBuilderBeam.h"
#include "chrono_fea/ChElementCableANCF.h"
#include "chrono_fea/ChLinkDirFrame.h"
#include "chrono_fea/ChLinkPointFrame.h"
#include "chrono_fea/ChMesh.h"
#include "chrono_fea/ChNodeFEAxyzD.h"
#include "chrono_fea/ChVisualizationFEAmesh.h"
#include "chrono_irrlicht/ChIrrApp.h"

// Chrono namespaces
using namespace chrono;
using namespace chrono::fea;
using namespace chrono::collision;
using namespace chrono::irrlicht;
using namespace irr;

std::shared_ptr<ChNodeFEAxyzD> node_tip;
double E = 0.0, D = 0.0, L = 0.0, rho = 0.0, Fa = 0.0, Fw = 0.0;
int SIM_NUM = 0;
std::string delim = ",";

void Create_Model(ChSystemSMC& mphysicalSystemH, std::shared_ptr<fea::ChMesh> my_mesh);

void write(ChSystemSMC& mphysicalSystem, std::shared_ptr<fea::ChMesh> my_mesh, double Fx) {
    std::ofstream myout;
    std::string outname = std::to_string(SIM_NUM);
    myout.open(outname + ".csv", std::ofstream::app);
    myout << mphysicalSystem.GetChTime() << delim << Fx;

    for (int i = 0; i < my_mesh->GetNnodes(); i++) {
        ChVector<> pos = std::dynamic_pointer_cast<ChNodeFEAxyzD>(my_mesh->GetNode(i))->GetPos();
        myout << delim << pos.x() << delim << pos.z();
    }
    myout << std::endl;
};
int main(int argc, char* argv[]) {
    if (argc != 8) {
        printf(" wrong number of input params (sim_num, E D L rho Fa Fw) You entered %d\n", argc);
        return 0;
    }
    SIM_NUM = atoi(argv[1]);
    E = atof(argv[2]);
    D = atof(argv[3]);
    L = atof(argv[4]);
    rho = atof(argv[5]);
    Fa = atof(argv[6]);
    Fw = atof(argv[7]);

    printf("num=%d, E=%f,D=%f,L=%f,rho=%f,Fa=%f,Fw=%f\n", SIM_NUM, E, D, L, rho, Fa, Fw);

    ChSystemSMC mphysicalSystem;
    auto my_mesh = std::make_shared<fea::ChMesh>();
    Create_Model(mphysicalSystem, my_mesh);

#ifdef CHRONO_IRRLICHT
    ChIrrApp application(&mphysicalSystem, L"Test ANCF Cables", core::dimension2d<u32>(1600, 1200), false, true);
    application.AddTypicalLogo();
    application.AddTypicalSky();
    application.AddTypicalLights();
    application.AddTypicalCamera(core::vector3df(0.001f, -0.05f, 0.1f));

    auto mvisualizebeamA = std::make_shared<ChVisualizationFEAmesh>(*(my_mesh.get()));
    mvisualizebeamA->SetFEMdataType(ChVisualizationFEAmesh::E_PLOT_NODE_DISP_NORM);
    mvisualizebeamA->SetColorscaleMinMax(0.0, 0.001);
    mvisualizebeamA->SetSmoothFaces(true);
    mvisualizebeamA->SetWireframe(false);
    my_mesh->AddAsset(mvisualizebeamA);
    application.AssetBindAll();
    application.AssetUpdateAll();

#endif

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
    //    my_system.SetTimestepperType(ChTimestepper::Type::EULER_IMPLICIT_LINEARIZED);

    mphysicalSystem.SetTimestepperType(ChTimestepper::Type::HHT);
    auto mystepper = std::dynamic_pointer_cast<ChTimestepperHHT>(mphysicalSystem.GetTimestepper());
    mystepper->SetAlpha(-0.1);
    mystepper->SetMaxiters(100);
    mystepper->SetAbsTolerances(1e-10, 1e-10);
    mystepper->SetMode(ChTimestepperHHT::POSITION);
    mystepper->SetScaling(true);
    mystepper->SetVerbose(false);

    double dt = 0.001;
    long int step = 0;
    int fps = 100;
    double endTime = 10;
    int freq = (int)(1.0 / (double)fps / dt);
    std::ofstream myout;
    std::string outname = std::to_string(SIM_NUM);
    myout.open(outname + ".csv");
    myout << "time" << delim << "Fx";

    for (int i = 0; i < my_mesh->GetNnodes(); i++) {
        ChVector<> pos = std::dynamic_pointer_cast<ChNodeFEAxyzD>(my_mesh->GetNode(i))->GetPos();
        myout << delim << "x" << i << delim << "z" << i;
    }
    myout << std::endl;

#ifdef CHRONO_IRRLICHT
    application.SetTimestep(dt);
    while (application.GetDevice()->run()) {
        application.BeginScene();
        application.DrawAll();
        double Fx = Fa * sin(Fw * mphysicalSystem.GetChTime());
        if (step % freq == 0) {
            write(mphysicalSystem, my_mesh, Fx);
        }
        node_tip->SetForce(ChVector<>(Fx, 0.0, 0.0));
        application.DoStep();
        application.EndScene();
        step++;
        if (mphysicalSystem.GetChTime() > endTime)
            break;
    }
#else
    while (mphysicalSystem.GetChTime() < endTime) {
        double Fx = Fa * sin(Fw * mphysicalSystem.GetChTime());
        if (step % freq == 0) {
            write(mphysicalSystem, my_mesh, Fx);
        }
        node_tip->SetForce(ChVector<>(Fx, 0.0, 0.0));
        mphysicalSystem.DoStepDynamics(dt);
        step++;

        if (mphysicalSystem.GetChTime() > endTime)
            break;
    }
#endif
    myout.close();
    return 0;
}

void Create_Model(ChSystemSMC& mphysicalSystem, std::shared_ptr<fea::ChMesh> my_mesh) {
    mphysicalSystem.Set_G_acc(ChVector<>(0.00, 0.0, 0.0));
    auto ground = std::make_shared<ChBody>();
    ground->SetIdentifier(-1);
    ground->SetBodyFixed(true);

    double Area = 3.1415 * std::pow(D, 2) / 4;
    double nu = 0.3;
    /*================== Cable Elements =================*/
    auto msection_cable = std::make_shared<ChBeamSectionCable>();
    msection_cable->SetDiameter(D);
    msection_cable->SetYoungModulus(E);
    msection_cable->SetDensity(rho);
    msection_cable->SetBeamRaleyghDamping(0.2);
    msection_cable->SetArea(Area);

    double plate_lenght_x = 0.1;
    double plate_lenght_y = 0.1;
    double Height = L;

    //    int N_x = 2;
    //    int N_y = 2;
    int numCableElems = 20;

    // For uniform grid
    //    double dx = plate_lenght_x / (N_x - 1);
    //    double dy = plate_lenght_y / (N_y - 1);
    ChBuilderBeamANCF builder;
    //
    //    for (int j = 0; j < N_y; j++) {
    //        for (int i = 0; i < N_x; i++) {
    double loc_x = 0.0;
    double loc_y = 0.0;
    builder.BuildBeam(my_mesh,         // the mesh where to put the created nodes and elements
                      msection_cable,  // the ChBeamSectionCable to use for the ChElementBeamANCF elements
                      numCableElems,   // the number of ChElementBeamANCF to create
                      ChVector<>(loc_x, loc_y, 0.0),    // the 'A' point in space (beginning of beam)
                      ChVector<>(loc_x, loc_y, Height)  // the 'B' point in space (end of beam) _1D_elementsNodes_mesh,
    );

    node_tip = builder.GetLastBeamNodes().back();

    //        }
    //    }

    // Fix the nodes attached to the ground
    for (int i = 0; i < my_mesh->GetNnodes(); i++) {
        auto node = std::dynamic_pointer_cast<ChNodeFEAxyzD>(my_mesh->GetNode(i));
        if (node->GetPos().z() <= 0.01) {
            node->SetFixed(true);

            //            auto pos_const = std::make_shared<ChLinkPointFrame>();
            //            pos_const->Initialize(node, ground);
            //            mphysicalSystem.Add(pos_const);
            //
            //            auto dir_const = std::make_shared<ChLinkDirFrame>();
            //            dir_const->Initialize(node, ground);
            //            dir_const->SetDirectionInAbsoluteCoords(node->D);
            //            mphysicalSystem.Add(dir_const);
        }
    }

    mphysicalSystem.Add(my_mesh);
}
