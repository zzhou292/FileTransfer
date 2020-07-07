#ifndef SYN_INTERSECTION_H
#define SYN_INTERSECTION_H

#include "synchrono/SynApi.h"
#include "synchrono/agent/SynFramework.h"

using namespace chrono;

namespace synchrono {
namespace agent {

class SYN_API Intersection {
  public:
    /// Defines a node
    /// Nodes are defined as an offset from the previous node
    struct Node {
        ChVector<> pos;  ///< point this node basically wraps

        /// Constructors
        Node() : pos({0, 0, 0}) {}                                  /// Sets pos to (0,0,0)
        Node(ChVector<> pos) : pos(pos) {}                          /// Set pos through constructor
        Node(double x, double y, double z = 0) : pos({x, y, z}) {}  /// Set pos to (x,y,z)

        /// Get pos
        ChVector<> GetPos() { return pos; }
    };

    /// Defines a lane
    /// Nodes start at stopping point and move back
    /// Nodes are defined as an offset from the previous node
    struct Lane {
        std::vector<Node> nodes;  ///< Nodes in this lane

        /// Possible constructor that sets the nodes list
        Lane(std::vector<Node> nodes) : nodes(nodes) {}

        /// Get node at specific index
        Node GetNode(int i) { return nodes[i]; }

        /// Set node list
        void SetNodes(std::vector<Node> nodes) { nodes = nodes; }

        /// Get node at the stopping point
        /// It is the first node in nodes
        Node GetStoppingNode() { return nodes.front(); }

        /// Get last node in the vector
        Node GetLastNode() { return nodes.back(); }

        /// Add a node to the nodes vector
        void AddNode(Node node) { nodes.push_back(node); }

        // /// Add multiple nodes at once to the nodes vector
        // void AddNodes(std::vector<Node> node) { nodes.push_back(node); }
    };

    /// Constructors
    Intersection() {}
    Intersection(std::vector<Lane> lanes) : lanes(lanes) {}

    /// Destructor
    ~Intersection() {}

    /// Add lane to lane vector
    void AddLanes(std::vector<Lane> lanes) { m_lanes.insert(m_lanes.end(), lanes.begin(), lanes.end()); }

    /// Add multiple lanes to lane vector at once
    void AddLane(Lane lane) { m_lanes.push_back(lane); }

    /// Set lane vector
    void SetLanes(std::vector<Lane> lanes) { m_lanes = lanes; }

    /// Get lanes
    std::vector<Lane> GetLanes() { return m_lanes; }

    /// Get lane at specified index
    Lane GetLane(int i) { return m_lanes[i]; }

  protected:
    std::vector<Lane> m_lanes;  ///< vector of lanes this Intersection has
}

/// Describes an intersection within the SynChrono framework
class SYN_API SynIntersection : public SynFramework {
  public:
    /// Construct a SynIntersection object with the specified origin and attached terrain
    SynIntersection(const GPScoord& origin, std::shared_ptr<SynTerrain> terrain) : SynFramework(origin, terrain) {}

    /// Destructor
    ~SynIntersection();

    /// Initialize the intersection
    /// Should set up lanes and traffic lights
    virtual void Initialize() = 0;

  private:
    std::shared_ptr<Intersection> m_intersection;  ///< intersection this class basically wraps
};

}  // namespace agent
}  // namespace synchrono

#endif
