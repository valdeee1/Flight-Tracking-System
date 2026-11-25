idf_component_register(SRCS "main.cpp" "ads_b_message.cpp" "aircraft_tracker.cpp"
                    INCLUDE_DIRS "."
                    PRIV_REQUIRES esp_driver_spi esp_driver_gpio 
                    )

