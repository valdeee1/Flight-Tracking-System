#include <cstdio>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_log.h"
#include <string>
#include <cmath>
#include <cstring>
#include "ads_b_message.h"

static const char *TAG = "SPI";

void vParsingTask(void *pvParameters);
void vSPITask(void *pvParameters); 
void initSPIAndInterrupts();

constexpr gpio_num_t SPI_MISO_PIN = GPIO_NUM_19;
constexpr gpio_num_t SPI_MOSI_PIN = GPIO_NUM_23;
constexpr gpio_num_t SPI_SCK_PIN = GPIO_NUM_18;
constexpr gpio_num_t SPI_CS_PIN = GPIO_NUM_5;
constexpr gpio_num_t SPI_IRQ_PIN = GPIO_NUM_4;

static TaskHandle_t spi_task_handle = nullptr;
static spi_device_handle_t spi_handle = nullptr;

static void IRAM_ATTR spi_irq_handler(void* arg) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    // Notify the SPI task that an interrupt has occurred
    vTaskNotifyGiveFromISR(spi_task_handle, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);

}

// Example 8d461f9b69a9936aef8f984e1b9a

extern "C" void app_main(void)
{
    initSPIAndInterrupts();

    QueueHandle_t spi_queue = xQueueCreate(10, sizeof(uint8_t[14]));

    xTaskCreatePinnedToCore(
        vParsingTask,
        "Parsing_Task",
        2048,
        nullptr,
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
        &spi_task_handle,// 6. Taskin kahva
        1               // 7. Ydin-affiniteetti
    );

}

void vParsingTask(void *pvParameters) {

    // Example 8d461f9b69a9936aef8f984e1b9a
    while(true)
    {

    }
    
}

void vSPITask(void *pvParameters) {

    uint8_t rx_buffer[14] = {0}; 

    spi_transaction_t transmit;
    
    std::memset(&transmit, 0, sizeof(transmit));
    transmit.length = 14*8; // Size of the message in bits
    transmit.rx_buffer = rx_buffer;
    transmit.tx_buffer = nullptr;

    while(true)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        esp_err_t ret = spi_device_polling_transmit(spi_handle, &transmit);

        if(ret == ESP_OK)
        {
           ESP_LOGI(TAG, "Viesti vastaanotettu: %02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X",
                     rx_buffer[0], rx_buffer[1], rx_buffer[2], rx_buffer[3],
                     rx_buffer[4], rx_buffer[5], rx_buffer[6], rx_buffer[7],
                     rx_buffer[8], rx_buffer[9], rx_buffer[10], rx_buffer[11],
                     rx_buffer[12], rx_buffer[13]);
            
        } else {
            ESP_LOGE(TAG, "SPI ERROR: %s", esp_err_to_name(ret));
        }
    }
}

void initSPIAndInterrupts() {

    /*
    
    TODO muuta master arkkitehtuuri slaveksi

    */
    spi_bus_config_t buscfg = {};
    buscfg.miso_io_num = SPI_MISO_PIN;
    buscfg.mosi_io_num = SPI_MOSI_PIN;
    buscfg.sclk_io_num = SPI_SCK_PIN;
    buscfg.quadwp_io_num = -1; 
    buscfg.quadhd_io_num = -1; 
    buscfg.max_transfer_sz = 0; 

    ESP_ERROR_CHECK(spi_bus_initialize(SPI3_HOST, &buscfg, SPI_DMA_CH_AUTO));

    spi_device_interface_config_t devcfg = {};
    devcfg.clock_speed_hz = 10 * 1000 * 1000; 
    devcfg.mode = 0; // SPI mode 0
    devcfg.spics_io_num = SPI_CS_PIN;
    devcfg.queue_size = 1;

    // Initialize the SPI bus

    // Add the device to the SPI bus
    ESP_ERROR_CHECK(spi_bus_add_device(SPI3_HOST, &devcfg, &spi_handle));

    // Initializes interrupt pin for spi
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_POSEDGE;
    io_conf.pin_bit_mask = 1ULL << SPI_IRQ_PIN;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_down_en = GPIO_PULLDOWN_ENABLE;
    ESP_ERROR_CHECK(gpio_config(&io_conf));

    ESP_ERROR_CHECK(gpio_install_isr_service(0));
    ESP_ERROR_CHECK(gpio_isr_handler_add(SPI_IRQ_PIN, spi_irq_handler, NULL));
}
