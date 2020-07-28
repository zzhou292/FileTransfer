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

#pragma once

#include <string>
#include <vector>
#include <map>

#include "chrono/core/ChVector.h"
#include "chrono/core/ChMatrix33.h"
#include "chrono/geometry/ChTriangle.h"
#include "chrono/geometry/ChTriangleMeshConnected.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <iterator>
#include <string>
#include <algorithm>
#include <boost/algorithm/string.hpp>

namespace chrono {
namespace granular {

/// Decompose a given triangle mesh surface into a minimally-overlapping collection of spheres.
/// The mesh must be stored in OBJ format and must consist of only triangles.
/// Returns the number of spheres added as a result of the decomposition.
template <typename Real>
std::vector<ChVector<>> ImportSphericalDecomposition(
    std::string csvfilename,  ///< csv mesh file path
    Real sphere_radius        ///< Radius to use for all spheres in the decomposition
) {
    std::vector<ChVector<>> sphere_points;
    std::string csv_filename = csvfilename;
    
    std::string delimeter = ",";
    
    std::ifstream file(csv_filename);

    std::vector<std::vector<std::string> > dataList;
    std::string line = "";
    // Iterate through each line and split the content using delimeter
    // std::cout << getline(file,line)<<std::endl;
    while (getline(file, line))
    {
        std::vector<std::string> vec;
        std::cout<<line<<std::endl;
        boost::algorithm::split(vec, line, boost::is_any_of(delimeter));
        dataList.push_back(vec);
    }
    
    for(std::vector<std::string> vec : dataList)
    {
        std::cout<<"inloop"<<std::endl;
        
        for(std::string data : vec)
        {
            std::cout<<data << " , ";
        }
        std::cout<<std::endl;
    }

        // Close the File
    file.close();
    

    return sphere_points;
}
}  // namespace granular
}  // namespace chrono