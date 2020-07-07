#!/usr/bin/env bash
cd ../chrono/chrono-SCM/build;
cmake ../ -DEIGEN3_INCLUDE_DIR=/usr/local/eigen/3.3.7/eigen3 -DEigen3_DIR=/usr/local/eigen/3.3.7/cmake/ -DCMAKE_BUILD_TYPE=Release -DENABLE_MODULE_VEHICLE=TRUE -DENABLE_MODULE_SENSOR=TRUE -DOptiX_INCLUDE=/usr/local/optix/x86_64/6.0.0/include -DOptiX_INSTALL_DIR=/usr/local/optix/x86_64/6.0.0