#include "synchrono/SynFramework.h"

using namespace chrono;

namespace synchrono {
namespace agent {

SynFramework::SynFramework(const GPScoord& origin, std::shared_ptr<SynTerrain> terrain)
    : m_origin(origin), m_terrain(terrain) {
    // Store origin info in radians, from degree-based GPScoord
    m_lat_origin = origin.lat_rad();
    m_lon_origin = origin.lon_rad();
    m_cos_origin = std::cos(m_lat_origin);
}
SynFramework::~SynFramework() {}

std::shared_ptr<ChBezierCurve> SynFramework::CurveFromGPS(std::vector<GPScoord>& gps_points,
                                                          double vert_offset,
                                                          bool closed) {
    std::vector<ChVector<>> bezier_points;
    for (auto gps_point : gps_points)
        bezier_points.push_back(To3DCartesian(gps_point, vert_offset));

    bool is_already_closed = (gps_points.back() - gps_points.front()).Length() < 1e-6;
    if (closed && !is_already_closed)
        bezier_points.push_back(bezier_points.front());

    return chrono_types::make_shared<ChBezierCurve>(bezier_points);
}

ChVector<> SynFramework::To3DCartesian(const GPScoord& gps, double height) const {
    auto lat_rad = gps.lat_rad();
    auto lon_rad = gps.lon_rad();

    // x is East, y is North
    auto x = EARTH_RADIUS * (lon_rad - m_lon_origin) * m_cos_origin;
    auto y = EARTH_RADIUS * (lat_rad - m_lat_origin);

    auto z = m_terrain->GetHeight({x, y, 0}) + height;
    return {x, y, z};
}

}  // namespace agent
}  // namespace synchrono
