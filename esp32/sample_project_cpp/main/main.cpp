#include <cstdio>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include <string>
#include <cmath>
#include "ads_b_message.h"


void vParsingTask(void *pvParameters);
void vSPITask(void *pvParameters); 

// Example 8d461f9b69a9936aef8f984e1b9a

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

}

void vParsingTask(void *pvParameters) {

    // Example 8d461f9b69a9936aef8f984e1b9a
    char *message = (char*)pvParameters;
}

void vSPITask(void *pvParameters) {

}
