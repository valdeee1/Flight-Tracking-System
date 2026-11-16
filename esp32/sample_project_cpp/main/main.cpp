#include <cstdio>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include <string>
#include <math.h>


void vParsingTask(void *pvParameters);
void vSPITask(void *pvParameters); 

// Example 8d461f9b69a9936aef8f984e1b9a


class ADS_B_Message {
public:
    ADS_B_Message(string raw_msg) {

    }

private:
    int capability;
    int icao;
    int type_code;

    typedef enum {
        ADSB_CA_LEVEL_1 = 0x00,
        ADSB_CA_RESERVED_1 = 0x01,
        ADSB_CA_RESERVED_2 = 0x02,
        ADSB_CA_RESERVED_3 = 0x03,
        ADSB_CA_LEVEL_2_EITHER = 0x04, // Ilmassa tai maassa
        ADSB_CA_LEVEL_2_GROUND = 0x05, // Vain maassa
        ADSB_CA_LEVEL_2_AIRBORNE = 0x06, // Vain ilmassa
        ADSB_CA_RESERVED_7 = 0x07
    } adsb_capability_t;

    typedef enum {
        ADSB_TC_IDENT_MIN = 1,
        ADSB_TC_IDENT_MAX = 4,
        ADSB_TC_SURFACE_POS_MIN = 5,
        ADSB_TC_SURFACE_POS_MAX = 8,
        ADSB_TC_AIRBORNE_POS_BARO_MIN = 9,
        ADSB_TC_AIRBORNE_POS_BARO_MAX = 18,
        ADSB_TC_AIRBORNE_VELOCITY = 19,
        ADSB_TC_AIRBORNE_POS_GNSS_MIN = 20,
        ADSB_TC_AIRBORNE_POS_GNSS_MAX = 22,
        ADSB_TC_STATUS_EMERGENCY = 28
    } adsb_type_code_t;
}



extern "C" void app_main(void)
{
    xQueueHandle_t message_buffer = xQueueCreate(10, sizeof(char*)); // Luodaan jono, johon mahtuu 10 kokonaislukua
    
    xTaskCreatePinnedToCore(
        vParsingTask,
        "Parsing_Task",
        2048,
        (void*)ads_b_message,
        5,
        NULL,
        1
    );
    xTaskCreatePinnedToCore(
        vSPITask,       // 1. Osoitin taskin funktioon (nimi)
        "SPI_Task",     // 2. Debug-nimi taskille
        2048,           // 3. Pinon koko tavuina
        nullptr,        // 4. Taskille välitettävä parametri
        5,              // 5. Prioriteetti
        NULL,           // 6. Taskin kahva
        1               // 7. Ydin-affiniteetti
    );

    printf("Taski luotu ja vuorontaja hoitaa sen nyt.\n");
}

void vParsingTask(void *pvParameters) {

    // Example 8d461f9b69a9936aef8f984e1b9a
    char *message = (char*)pvParameters;
}

void vSPITask(void *pvParameters) {

}
