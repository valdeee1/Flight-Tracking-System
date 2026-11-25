#pragma once
#include <string>
#include <cstdint>


class ADS_B_Message 
{
public:
    explicit ADS_B_Message(const uint8_t raw_bytes[14]);

    static double calculateLatitude(uint32_t latCprOdd, uint32_t latCprEven, bool isOddNewer);
    static double calculateLongitude(uint32_t lonCprOdd, uint32_t lonCprEven, bool isOddNewer, double latitude);
    uint32_t getIcao() const { return icao; }
    uint8_t getTypeCode() const { return type_code; }
    bool getIsOdd() const { return is_odd; }
    uint32_t getLatCpr() const { return lat_cpr; }
    uint32_t getLonCpr() const { return lon_cpr; }
    bool hasPositionData() const { return has_position_data; }
    int getDownlinkFormat() const { return downlink_format; }

private:
    uint8_t capability = 0;
    uint32_t icao = 0;
    uint8_t type_code = 0; 
    bool is_odd;
    uint32_t lat_cpr;
    uint32_t lon_cpr;
    bool has_position_data;
    uint8_t downlink_format;

    enum class Capability : uint8_t {
        LEVEL_1 = 0x00,
        RESERVED_1 = 0x01,
        RESERVED_2 = 0x02,
        RESERVED_3 = 0x03,
        LEVEL_2_EITHER = 0x04, // Ilmassa tai maassa
        LEVEL_2_GROUND = 0x05, // Vain maassa
        LEVEL_2_AIRBORNE = 0x06, // Vain ilmassa
        RESERVED_7 = 0x07
    };

    enum class TypeCode : uint8_t {
        IDENTIFICATION = 1,
        SURFACE_POSITION = 5,
        AIRBORNE_POSITION_BARO = 9,
        AIRBORNE_VELOCITY = 19,
        AIRBORNE_POSITION_GNSS = 20,
        STATUS_EMERGENCY = 28
    };
};
