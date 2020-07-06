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
// OptiX rendering engine for processing jobs for sensing. Jobs are defined on
// each sensor as a graph. Recommended to use one engine per GPU to mitigate
// OptiX blocking launch calls
//
// =============================================================================

#ifndef CHOPTIXENGINE_H
#define CHOPTIXENGINE_H

#include "chrono_sensor/ChApiSensor.h"

#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>
#include <unordered_map>

#include "chrono_sensor/ChOptixSensor.h"
#include "chrono_sensor/scene/ChScene.h"

#include "chrono/assets/ChVisualMaterial.h"
#include "chrono/assets/ChBoxShape.h"
#include "chrono/assets/ChSphereShape.h"
#include "chrono/assets/ChCylinderShape.h"
#include "chrono/assets/ChTriangleMeshShape.h"

namespace chrono {
namespace sensor {

class CH_SENSOR_API ChOptixEngine {
    class DynamicMesh {
      public:
        DynamicMesh(std::shared_ptr<geometry::ChTriangleMeshConnected> mesh,
                    optix::GeometryGroup tri_group,
                    optix::Buffer vertex_buffer,
                    optix::Buffer normal_buffer) {
            m_mesh = mesh;
            m_vertex_buffer = vertex_buffer;
            m_normal_buffer = normal_buffer;
            m_tri_group = tri_group;

            int device_id = m_normal_buffer->getContext()->getEnabledDevices()[0];
            m_size = m_mesh->getCoordsVertices().size();
            m_normal_buffer_device_ptr = m_normal_buffer->getDevicePointer(device_id);
            m_vertex_buffer_device_ptr = m_vertex_buffer->getDevicePointer(device_id);
            m_host_normal_buffer = std::vector<float>(m_size * 3);
            m_host_vertex_buffer = std::vector<float>(m_size * 3);
            for (int i = 0; i < m_size; i++) {
                m_host_vertex_buffer[3 * i] = (float)mesh->getCoordsVertices()[i].x();
                m_host_vertex_buffer[3 * i + 1] = (float)mesh->getCoordsVertices()[i].y();
                m_host_vertex_buffer[3 * i + 2] = (float)mesh->getCoordsVertices()[i].z();

                m_host_normal_buffer[3 * i] = (float)mesh->getCoordsNormals()[i].x();
                m_host_normal_buffer[3 * i + 1] = (float)mesh->getCoordsNormals()[i].y();
                m_host_normal_buffer[3 * i + 2] = (float)mesh->getCoordsNormals()[i].z();
            }
        }

      public:
        std::shared_ptr<geometry::ChTriangleMeshConnected> getMesh() { return m_mesh; }
        optix::Buffer getNormalBuffer() { return m_normal_buffer; }
        optix::Buffer getVertexBuffer() { return m_vertex_buffer; }
        float* host_normalBuffer_ptr() { return m_host_normal_buffer.data(); }
        float* host_vertexBuffer_ptr() { return m_host_vertex_buffer.data(); }
        void* device_normalBuffer_ptr() { return m_normal_buffer_device_ptr; }
        void* device_vertexBuffer_ptr() { return m_vertex_buffer_device_ptr; }
        int getSize() { return m_size; }
        optix::GeometryGroup getTriGroup() { return m_tri_group; }

      private:
        optix::Buffer m_vertex_buffer;
        optix::Buffer m_normal_buffer;
        void* m_normal_buffer_device_ptr;
        void* m_vertex_buffer_device_ptr;
        int m_size;
        std::vector<float> m_host_normal_buffer;
        std::vector<float> m_host_vertex_buffer;
        optix::GeometryGroup m_tri_group;
        std::shared_ptr<geometry::ChTriangleMeshConnected> m_mesh;
    };

  public:
    // ChOptixEngine(ChSystem* sys, bool verbose = false);
    ChOptixEngine(ChSystem* sys, int device_id, int max_scene_reflections = 9, bool verbose = false);  /// constructor
    ~ChOptixEngine();                                                                                  /// destructor

    void AssignSensor(std::shared_ptr<ChOptixSensor> sensor);
    void UpdateSensors(std::shared_ptr<ChScene> scene);

    void ConstructScene();  /// tells the optix manager to construct the scene from scratch, translating all objects
                            /// from Chrono to Optix
    void AddInstancedStaticSceneMeshes(
        std::vector<ChFrame<>>& frames,
        std::shared_ptr<ChTriangleMeshShape>
            mesh);  /// adds a static triangle mesh to the scene that is external to Chrono. This is a way to add
                    /// complex environment that includes trees, etc

    // optix::Context GetOptixContext() { return m_context; }  // return the optix context for this group
    // optix::Group GetRootNode() { return m_root; }           // return the root node of this scene
    int GetDevice() { return m_deviceId; }  // can only look at the device number, cannot change it
    int GetNumSensor() { return (int)m_assignedSensor.size(); }
    optix::Context GetContext() { return m_context; }  /// gives the user the optix context
    std::vector<std::shared_ptr<ChOptixSensor>> GetSensor() {
        return m_assignedSensor;
    }  /// returns the list of sensors attached to the engine

  private:
    void Start();  /// start the render thread
    void Stop();   /// stop the render thread

    bool m_verbose;     /// whether the context should print errors and warnings
    void Initialize();  /// intialize function for the optix engine. This is what gets everything up and running
    void Process();     // function that processes sensor added to its queue

    void UpdateCameraTransforms();  /// updates all of the camera position and orientations
    void UpdateBodyTransforms();    /// updates all the transforms associated with the bodies
    void UpdateDynamicMeshes();     /// updates the dynamic meshes in the scene
    void UpdateSceneDescription(
        std::shared_ptr<ChScene> scene);  /// updates the scene characteristics such as lights, background, etc

    optix::Material GetDefaultMaterial();  // returns a default material, creating it if it does not yet exist
    optix::Material CreateMaterial(
        std::shared_ptr<ChVisualMaterial> chmat);               /// creates a new material based on a ChVisualMaterial
    optix::TextureSampler CreateTexture(std::string filename);  /// creates a texture from file
    optix::TextureSampler CreateTexture();                      /// creates a default texture

    optix::Geometry
    GetOptixBoxGeometry();  /// gets the box geometry and creates one if the single version doesn't yet exist
    optix::Geometry GetOptixSphereGeometry();    /// get the sphere geometry and creates it if it does not yet exist
    optix::Geometry GetOptixCylinderGeometry();  /// get the cylinder geometry and creates it if it does not yet exist

    void boxVisualization(std::shared_ptr<ChBoxShape> box_shape,
                          std::shared_ptr<ChVisualization> visual_asset,
                          optix::Group asset_group);
    void sphereVisualization(std::shared_ptr<ChSphereShape> sphere_shape,
                             std::shared_ptr<ChVisualization> visual_asset,
                             optix::Group asset_group);
    void cylinderVisualization(std::shared_ptr<ChCylinderShape> cylinder_shape,
                               std::shared_ptr<ChVisualization> visual_asset,
                               optix::Group asset_group);
    void staticTrimeshVisualization(std::shared_ptr<ChTriangleMeshShape> trimesh_shape,
                                    std::shared_ptr<ChVisualization> visual_asset,
                                    optix::Group asset_group);
    void dynamicTrimeshVisualization(std::shared_ptr<ChTriangleMeshShape> trimesh_shape,
                                     std::shared_ptr<ChVisualization> visual_asset,
                                     optix::Group asset_group);

    std::thread m_thread;                                  /// worker thread for performing render operations
    std::vector<std::shared_ptr<ChSensor>> m_renderQueue;  /// list of sensors for the engine to manage to process

    std::deque<Keyframe> m_keyframes;

    // mutex and condition variables
    std::mutex m_renderQueueMutex;            /// mutex for protecting the render queue
    std::condition_variable m_renderQueueCV;  /// condition variable for notifying the worker thread it should process
                                              /// the filters from the queue
    bool m_terminate = false;                 /// worker thread stop variable
    bool m_started = false;                   /// worker thread start variable

    // objects that should be instanced or reused
    optix::Geometry m_box_geometry;      /// box geometry that all boxes in the scenes will share
    optix::Acceleration m_box_accel;     /// acceleration object that all box geometries in the scene will share
    optix::Geometry m_sphere_geometry;   /// sphere geometry that all sphere in the scenes will share
    optix::Acceleration m_sphere_accel;  /// acceleration object that all sphere geometries in the scene will share
    optix::Geometry m_cyl_geometry;      /// sphere geometry that all sphere in the scenes will share
    optix::Acceleration m_cyl_accel;     /// acceleration object that all sphere geometries in the scene will share

    /// All RT Program should only be made once and then reused
    optix::Program m_camera_shader;    /// camera material shader
    optix::Program m_lidar_shader;     /// lidar material shader
    optix::Program m_shadow_shader;    /// shadow shader
    optix::Program m_camera_miss;      /// camera miss shader
    optix::Program m_lidar_miss;       /// lidar miss shader
    optix::Program m_box_int;          /// box intersection function
    optix::Program m_box_bounds;       /// box bounding function
    optix::Program m_sphere_int;       /// sphere intersection function
    optix::Program m_sphere_bounds;    /// sphere bounds function
    optix::Program m_cylinder_int;     /// cylinder intersection function
    optix::Program m_cylinder_bounds;  /// cylinder intersection function
    optix::Program m_mesh_att;         /// mesh attributes function

    // default material objects
    optix::TextureSampler m_empty_tex_sampler;  /// default texture sampler
    optix::Material m_default_material;         /// default material

    optix::Buffer m_light_buffer;  /// scene light buffer

    // information that belongs to the rendering concept of this engine
    optix::Context m_context;                                      /// the optix context we use for everything
    optix::Group m_root;                                           /// root node of the optix scene
    std::vector<std::shared_ptr<ChOptixSensor>> m_assignedSensor;  /// list of sensor this engine is responsible for
    ChSystem* m_system;                                            /// the chrono system that defines the scene
    std::vector<std::pair<std::shared_ptr<ChBody>, optix::Transform>>
        m_bodies;  /// matching bodies to transforms for quicker upgates of the scene
    std::vector<std::shared_ptr<DynamicMesh>> m_dynamicMeshes;  /// list of dynamic meshes for quick updating
    int m_deviceId;                                             /// ID of the GPU the context should be attached to
    int m_recursions;                                           /// number of allowable ray tracing recursions in optix
};

}  // namespace sensor
}  // namespace chrono

#endif
