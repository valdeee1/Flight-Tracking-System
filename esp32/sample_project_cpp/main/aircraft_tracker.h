#pragma once
#include <cstdint>
#include <string>
#include <array>
#include "ads_b_message.h"

class AircraftTracker : public ADS_B_Message
{
public:

    void addOrUpdateAircraft(const ADS_B_Message& message);
    void deleteAircraft(uint32_t icao);
    void getAircraft( const uint32_t icao);


private:
    typedef struct{

        uint32_t icao = 0;
        char callsign[9] = {0};

        uint32_t even_lat_cpr = 0;
        uint32_t even_lon_cpr = 0;
        uint32_t odd_lat_cpr = 0;
        uint32_t odd_lon_cpr = 0;

        bool has_even = false;
        bool has_odd = false;

        bool isActive = false;

        double latitude = 0;
        double longitude = 0;
    } Aircraft;

    static constexpr size_t MAX_AIRCRAFT = 64;

    std::array<Aircraft, MAX_AIRCRAFT> aircraft_list{};

};