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
// Authors: Asher Elmquist
// =============================================================================
//
// RT kernels for mesh geometries
//
// =============================================================================

#include <optixu/optixu_math_namespace.h>

using namespace optix;

rtDeclareVariable(float3, shading_normal, attribute shading_normal, );
rtDeclareVariable(float2, texcoord, attribute texcoord, );

rtBuffer<int3> index_buffer;
rtBuffer<float3> vertex_buffer;
rtBuffer<float3> normal_buffer;
rtBuffer<float2> texcoord_buffer;

RT_PROGRAM void mesh_attributes() {
    const int3 vertex_idx = index_buffer[rtGetPrimitiveIndex()];
    const float2 bary_coord = rtGetTriangleBarycentrics();

    if (normal_buffer.size() > 0) {
        shading_normal =
            normalize(normal_buffer[vertex_idx.y] * bary_coord.x + normal_buffer[vertex_idx.z] * bary_coord.y +
                      normal_buffer[vertex_idx.x] * (1.0f - bary_coord.x - bary_coord.y));
    } else {
        shading_normal = normalize(cross(vertex_buffer[vertex_idx.y] - vertex_buffer[vertex_idx.x],
                                         vertex_buffer[vertex_idx.z] - vertex_buffer[vertex_idx.x]));
    }
    if (texcoord_buffer.size() == 0) {
        texcoord = make_float2(0.0f);
    } else {
        texcoord = texcoord_buffer[vertex_idx.y] * bary_coord.x + texcoord_buffer[vertex_idx.z] * bary_coord.y +
                   texcoord_buffer[vertex_idx.x] * (1.0f - bary_coord.x - bary_coord.y);
    }
}
