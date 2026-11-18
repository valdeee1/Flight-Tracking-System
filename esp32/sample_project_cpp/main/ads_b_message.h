#pragma once
#include <string>
#include <cstdint>


class ADS_B_Message 
{
public:
    explicit ADS_B_Message(const std::string& raw_msg);

private:

    int calculateNL(double lat);
    double calculateLatitude(uint32_t latCprOdd, uint32_t latCprEven, bool isOddNewer);
    double calculateLongitude(uint32_t lonCprOdd, uint32_t lonCprEven, bool isOddNewer, double latitude);

    int capability;
    int icao;
    int type_code;


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
