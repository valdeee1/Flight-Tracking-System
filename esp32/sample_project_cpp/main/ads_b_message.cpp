#include "ads_b_message.h"
#include <cmath>
#include <algorithm>

#define M_PI 3.14159265358979323846

// Example 8d461f9b69a9936aef8f984e1b9a


ADS_B_Message::ADS_B_Message(const uint8_t raw_bytes[14]) 
{
    uint32_t message_crc = (static_cast<uint32_t>(raw_bytes[11]) << 16) |
                       (static_cast<uint32_t>(raw_bytes[12]) << 8)  |
                        static_cast<uint32_t>(raw_bytes[13]);
    if(calculateCRC(raw_bytes) != message_crc)
    {
        isValidMessage = false;
    }

    downlink_format = ((raw_bytes[0] >> 3) & 0b00011111);

    if(downlink_format == 17)
    {
        icao = (static_cast<uint32_t>(raw_bytes[1]) << 16) | (static_cast<uint32_t>(raw_bytes[2]) << 8) | static_cast<uint32_t>(raw_bytes[3]);
        capability = raw_bytes[0] & 0b00000111;
        type_code = (raw_bytes[4] >> 3) & 0b00011111; 

        if((type_code >= 9 && type_code <= 18) || (type_code >= 20 && type_code <= 22)) {

            has_position_data = true;
            is_odd = ((raw_bytes[6] >> 2) & 0b00000001) != 0;

            lat_cpr = ((static_cast<uint32_t>(raw_bytes[6]) & 0b00000011) << 15) |
            (static_cast<uint32_t>(raw_bytes[7]) << 7)                |
            ((static_cast<uint32_t>(raw_bytes[8]) >> 1) & 0b01111111);

            lon_cpr = ((static_cast<uint32_t>(raw_bytes[8]) & 0b00000001) << 16) |
            (static_cast<uint32_t>(raw_bytes[9]) << 8)                |
            static_cast<uint32_t>(raw_bytes[10]);

        } else {
            has_position_data = false;
        }
    } else {
        isValidMessage = false;
    }
}


double ADS_B_Message::calculateLatitude(uint32_t latCprOdd, uint32_t latCprEven, bool isOddNewer) 
{
    double dLat0 = 6.0; // Even latitude zone size
    double dLat1 = 360.0 / 59.0; // Odd latitude zone size

    // Normalizing CPR
    double lat_i_odd = static_cast<double>(latCprOdd) / 131072;
    double lat_i_even = static_cast<double>(latCprEven) / 131072;

    // Calculating latitude zone index
    double j = std::floor(59 * lat_i_even - 60 * lat_i_odd + 0.5);

    // Calculating latitude

    //Ensuring positive 
    auto pos_mod = [](double a, double n) {
        double r = std::fmod(a, n);
        return (r < 0.0) ? (r + n) : r;
    };
    double latEven = dLat0 * (pos_mod(j, 60) + lat_i_even);
    double latOdd = dLat1 * (pos_mod(j, 59) + lat_i_odd);

    // Support for southern hemisphere
    if (latEven >= 270.0) latEven -= 360.0;
    if (latOdd >= 270.0) latOdd -= 360.0;

    return isOddNewer ? latOdd : latEven;
}

double ADS_B_Message::calculateLongitude(uint32_t lonCprOdd, uint32_t lonCprEven, bool isOddNewer, double latitude) 
{
    // Calculate the number of latitude zones (NL)
    int nl;

    if (std::abs(latitude) >= 87.0)
    {
        nl = 1;
    }
    else if (std::abs(latitude) == 0.0)
    {
        nl = 59;
    }
    else {
        constexpr double NZ = 15.0;
        constexpr double DEG2RAD = M_PI / 180.0;

        double num = 1.0 - std::cos(M_PI / (2.0 * NZ));
        double den = std::pow(std::cos(DEG2RAD * std::abs(latitude)), 2);
        double val = 1.0 - (num / den);

        if (val < -1.0) val = -1.0;
        if (val > 1.0) val = 1.0;

        nl = static_cast<int>(std::floor((2.0 * M_PI) / std::acos(val)));
    }

    // Calculate the number of longitude zones (ni) based on NL
    int ni0 = std::max(nl, 1);
    int ni1 = std::max(nl - 1, 1);

    // Calculate the longitude zone sizes for even and odd messages
    double dLon0 = 360.0 / static_cast<double>(ni0);
    double dLon1 = 360.0 / static_cast<double>(ni1);

    // Normalization
    double lon_i_odd = static_cast<double>(lonCprOdd) / 131072.0;
    double lon_i_even = static_cast<double>(lonCprEven) / 131072.0;

    // Calculate longitude index m (always (nl - 1) and nl, independent of isOddNewer)
    double m = std::floor(lon_i_even * (nl - 1) - lon_i_odd * nl + 0.5);

    // Calculate longitude based on whether the odd or even message is newer
    auto pos_mod = [](double a, double n) {
        double r = std::fmod(a, n);
        return (r < 0.0) ? (r + n) : r;
    };

    double lon = 0.0;
    if (isOddNewer) {
        lon = dLon1 * (pos_mod(m, static_cast<double>(ni1)) + lon_i_odd);
    } else {
        lon = dLon0 * (pos_mod(m, static_cast<double>(ni0)) + lon_i_even);
    }

    // Support for longitude wrapping
    if (lon >= 180.0) {
        lon -= 360.0;
    }

    return lon;
}

uint32_t ADS_B_Message::calculateCRC(const uint8_t raw_bytes[14]) 
{
    uint32_t generator = 0xFFF409; 
    uint32_t crc = 0;

    for(int i = 0; i < 11; i++)
    {
        crc ^= (static_cast<uint32_t>(raw_bytes[i] << 16));

        for(int bit = 0; bit < 8; ++bit)
        {
            crc <<= 1;
            if (crc & 0x1000000)
            {
                crc ^= generator;
            }
        }
    }
    return crc & 0x00FFFFFF;
}
