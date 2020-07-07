#ifndef SYN_VIS_H
#define SYN_VIS_H

#include "synchrono/SynApi.h"

namespace synchrono {
namespace visualization {

class SYN_API SynVis {
  public:
    /// Visualization types
    enum VisType {
        SENSOR_VIS,   ///> sensor visualization
        IRRLICHT_VIS  ///> irrlicht visualization
    };

    /// Constructs a visualization manager
    SynVis(SynVis::VisType type) : m_type(type), m_should_initialize(true) {}

    /// Destructor
    ~SynVis() {}

    /// Whether or not visualizers need initialization
    bool ShouldInitialize() { return m_should_initialize; }

    /// Advance the state of this visualizer.
    virtual void Update(double step) = 0;

    /// Initialize this visualizer.
    virtual void Initialize() = 0;

    /// Get visualizer type
    SynVis::VisType GetType() { return m_type; }

  protected:
    SynVis::VisType m_type;  ///< vis type

    bool m_should_initialize;  ///< does visualizers need initialization
};

}  // namespace visualization
}  // namespace synchrono

#endif  // SYN_VIS_H
