#include "SynContinentalIntersection.h"

namespace synchrono {
namespace agent {

// Initialize the intersection
// Sets up lanes and traffic lights
SynContinentalIntersection::Initialize() {
  // Super messy and a lot needs to change about the pipeline of traffic_light creation
  double l = 20;
  double h = 1.0;
  double w = 2.5;
  double b = 2.5;
  double x = 1.5;
  double y = -18;
  AddLane(ChVector<>(x, y, h), ChVector<>(x, y - l, h), w);
  AddLane(ChVector<>(x + w + b, y, h), ChVector<>(x + w + b, y - l, h), w);

  x = 20;
  y = -6;
  b = .75;
  AddLane(ChVector<>(x, y, h), ChVector<>(x + l, y, h), w);
  AddLane(ChVector<>(x, y + w + b, h), ChVector<>(x + l, y + w + b, h), w);
  AddLane(ChVector<>(x, y + (w + b) * 2, h), ChVector<>(x + l, y + (w + b) * 2, h), w);
  b = 1.75;
  AddLane(ChVector<>(x, y + (w + b) * 3, h), ChVector<>(x + l, y + (w + b) * 3, h), w);

  x = -3.5;
  y = 40;
  b = 2.75;
  AddLane(ChVector<>(x, y - l, h), ChVector<>(x, y + 0.5 * l, h), w);
  AddLane(ChVector<>(x - (w + b), y, h), ChVector<>(x - (w + b), y - l, h), w);

  if (m_rank == 1) {
      SetColor(SynSPATMessage::GREEN, {0, 1});
  } else if (m_rank == 2) {
      SetColor(SynSPATMessage::GREEN, {0, 1, 2, 3});
  } else if (m_rank == 3) {
      SetColor(SynSPATMessage::GREEN, {0});
  }
}

}  // namespace agent
}  // namespace synchrono
