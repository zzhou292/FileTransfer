#ifndef SYN_SENSOR_MESSAGE_H
#define SYN_SENSOR_MESSAGE_H

#include "synchrono/flatbuffer/SynMessage.h"

namespace synchrono {

// a pixel in RGBA 8bpp format
struct PixelRGBA8 {
    uint8_t R;
    uint8_t G;
    uint8_t B;
    uint8_t A;
};

// a pixel in R8 (8-bit grayscale) format
struct PixelR8 {
    uint8_t R;
};

// depth and intensity data in generic format
struct PixelDI {
    float range;
    float intensity;
};

// point cloud and intensity data in generic format
struct PixelXYZI {
    float x;
    float y;
    float z;
    float intensity;
};

// IMU data in generic format
struct IMUData {
    double Accel[3];
    double Roll;
    double Pitch;
    double Yaw;
};

// GPS data in generic format
struct GPSData {
    double Latitude;
    double Longitude;
    double Altitude;
    double Time;
};

class SYN_API SynSensorMessage : public SynMessage {
  public:
    // Base class of 2D buffers
    struct SensorBuffer {
        SensorBuffer() : Width(0), Height(0), LaunchedCount(0) {}
        SensorBuffer(unsigned int w, unsigned int h) : Width(w), Height(h), LaunchedCount(0) {}
        SensorBuffer(unsigned int w, unsigned int h, void* b, int size_in_bytes)
            : Width(w), Height(h), LaunchedCount(0) {
            Buffer.resize(size_in_bytes);
            memcpy(Buffer.data(), b, size_in_bytes);
        }
        virtual ~SensorBuffer() {
            // delete Buffer;
        }  // virtual destructor so class is virtual so it can participate in dynamic_pointer_cast<>'s
        unsigned int Width;
        unsigned int Height;
        unsigned int LaunchedCount;  // number of times updates have been launched.
        std::vector<uint8_t> Buffer;
    };

    /// State struct that holds state information for a SynSensorMessage
    struct State : SynMessage::State {
        enum Type {
            NONE = 0,   ///< Default Type
            RGBA8 = 1,  ///< RGBA 8BPP Format
            R8 = 2,     ///< 8-bit Grayscale
            DI = 3,     ///< Depth and Intensity
            XYZI = 4,   ///< Point Cloud and Intensity
            IMU = 5,    ///< Accel (x,y,z), Roll, Pitch, Yaw
            GPS = 6     ///< Latitude, Longitude, Altitude, Time
        } type;

        std::shared_ptr<SensorBuffer> buffer;  ///< Handle to the SensorBuffer

        /// Default Constructor
        State(double time = 0, Type type = NONE, std::shared_ptr<SensorBuffer> buffer = nullptr)
            : SynMessage::State(time), type(type), buffer(buffer) {}

        /// Casts underlying Buffer to the specified type
        template <class T>
        T* GetBufferAsType() {
            return (T*)buffer->Buffer.data();
        }

        /// Get data type as SynFlatBuffers::Sensor::Type
        SynFlatBuffers::Sensor::Type GetTypeAsFlatBuffers() { return (SynFlatBuffers::Sensor::Type)type; }
    };

    /// Constructors
    SynSensorMessage(int id);
    SynSensorMessage(int id, std::shared_ptr<State> state);
    SynSensorMessage(int id, const SynFlatBuffers::Message* message);

    /// Generate state from FlatBuffers message
    virtual void GenerateState(const SynFlatBuffers::Message* message) override;

    /// Generate FlatBuffers message from this message's state
    virtual FlatBufferMessage GenerateMessage(flatbuffers::FlatBufferBuilder& builder) override;

  private:
    /// Helper methods for each sensor type
};

}  // namespace synchrono

#endif
