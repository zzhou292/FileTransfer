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
// Authors: Radu Serban, Aaron Young
// =============================================================================
//
// Global functions for accessing the Chrono::Interface model data.
//
// =============================================================================

#ifndef CH_INTERFACE_DATA_H
#define CH_INTERFACE_DATA_H

#include <string>
#include "synchrono_interface/SynApiInterface.h"

namespace synchrono {
namespace interface {

/// @addtogroup vehicle
/// @{

/// Set the path to the Chrono::Interface data directory (ATTENTION: not thread safe).
SYN_INTERFACE_API void SetDataInterfacePath(const std::string& path);

/// Get the current path to the Chrono::Interface data directory (thread safe).
SYN_INTERFACE_API const std::string& GetDataInterfacePath();

/// Get the complete path to the specified filename (thread safe).
/// The filename is assumed to be given relative to the Chrono::Interface model
/// data directory.
SYN_INTERFACE_API std::string GetDataInterfaceFile(const std::string& filename);

/// @} vehicle

}  // namespace interface
}  // end namespace chrono

#endif
