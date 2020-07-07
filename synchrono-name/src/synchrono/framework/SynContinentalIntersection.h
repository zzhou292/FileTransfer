#ifndef SYN_CONTINENTAL_INTERSECTION_H
#define SYN_CONTINENTAL_INTERSECTION_H

#include "synchrono/SynApi.h"
#include "synchrono/agent/SynFramework.h"

using namespace chrono;

namespace synchrono {
namespace agent {

/// Intersection to be used for demos that utilize the meshes created by Continental Mapping
/// Implemented from this:
/// https://transops.s3.amazonaws.com/uploaded_files/SPaT%20Webinar%20%233%20-%20NOCoE%20-Overview%20of%20MAP%20Messages.pdf
class SYN_API SynContinentalIntersection : public SynIntersection {
  public:
    enum IntersectionType {
        UNIV_AND_PARK  ///< University Ave. and N Park St intersection
    };

    struct UnivParkIntersection : public Intersection {
        UnivParkIntersection() {
            std::vector<Lane> UnivWestLanes = {
                Lane({{Node(20.0, -6.0, 1.0), Node(40.0, 0, 0)}}), Lane({{Node(20.0, -2.75, 1.0), Node(40.0, 0, 0)}}),
                Lane({{Node(20.0, 0.5, 1.0), Node(40.0, 0, 0)}}), Lane({{Node(20.0, 6.75, 1.0), Node(40.0, 0, 0)}})};

            std::vector<Lane> ParkNorthLanes = {Lane({{Node(1.5, -18, 1.0), Node(0, -20, 0)}}),
                                                Lane({{Node(4.0, -18, 1.0), Node(0, -20, 0)}})};

            std::vector<Lane> ParkSouthLanes = {Lane({{Node(-3.5, 20, 1.0), Node(0, 30, 0)}}),
                                                Lane({{Node(-8.75, 40, 1.0), Node(0, -20, 0)}})};

            AddLanes(UnivWestLanes);
            AddLanes(ParkNorthLanes);
            AddLanes(ParkSouthLanes);
        }
    };

    /// Construct a SynContinentalIntersection object with the specified origin, attached terrain and type
    SynContinentalIntersection(const GPScoord& origin, std::shared_ptr<SynTerrain> terrain, IntersectionType type)
        : SynFramework(origin, terrain), m_type(type), m_intersection(chrono_types::make_shared<UnivParkIntersection>()) {}

    /// Destructor
    ~SynContinentalIntersection();

    /// Initialize the intersection
    /// Sets up lanes and traffic lights
    virtual void Initialize();

    /// Get intersection type
    IntersectionType GetIntersectionType() { return m_type; }

  private:
    IntersectionType m_type;  ///< intersection type
};

}  // namespace agent
}  // namespace synchrono

#endif
