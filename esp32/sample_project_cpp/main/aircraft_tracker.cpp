#include "aircraft_tracker.h"
#include "esp_log.h"
#include "esp_timer.h"

void AircraftTracker::addOrUpdateAircraft(const ADS_B_Message& message)
{
    if(!message.isValid()) {
        return;
    }

    Aircraft* target = nullptr;
    Aircraft* free_slot = nullptr;

    for (auto& aircraft : aircraft_list)
    {
        if (aircraft.isActive && aircraft.icao == message.getIcao())
        {
            target = &aircraft;
            break; 
        }
        if (!aircraft.isActive && free_slot == nullptr)
        {
            free_slot = &aircraft; 
        }
    }

    if (target == nullptr)
    {
        if (free_slot == nullptr)
        {
            return; 
        }
        *free_slot = Aircraft{}; 
        free_slot->icao = message.getIcao();
        free_slot->isActive = true;
        target = free_slot;
    }

    target->last_update_time_ms = esp_timer_get_time() / 1000;

    if (message.hasPositionData())
    {   
        if (message.getIsOdd())
        {
            target->odd_lat_cpr = message.getLatCpr();
            target->odd_lon_cpr = message.getLonCpr();
            target->has_odd = true;
        }
        else
        {
            target->even_lat_cpr = message.getLatCpr();
            target->even_lon_cpr = message.getLonCpr();
            target->has_even = true;
        }

        if (target->has_even && target->has_odd)
        {
            bool isOddNewer = message.getIsOdd();
            target->latitude = ADS_B_Message::calculateLatitude(
                target->odd_lat_cpr, target->even_lat_cpr, isOddNewer
            );
            target->longitude = ADS_B_Message::calculateLongitude(
                target->odd_lon_cpr, target->even_lon_cpr, isOddNewer, target->latitude
            );
        }
    }
}

void AircraftTracker::deleteAircraft(uint32_t icao)
{
    for (auto& aircraft : aircraft_list)
    {
        if (aircraft.isActive && aircraft.icao == icao)
        {
            aircraft = Aircraft{}; 
            break;
        }
    }
}

void AircraftTracker::deleteInActiveAircraft()
{
    uint32_t current_time_ms = esp_timer_get_time() / 1000;
    for (auto& aircraft : aircraft_list)
    {
        if (aircraft.isActive)
        {
            if (current_time_ms - aircraft.last_update_time_ms > timeout_ms)
            {
            aircraft = Aircraft{}; 
            }
        }
    }
}
