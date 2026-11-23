idf_component_register(SRCS "main.cpp" "ads_b_message.cpp"
                    INCLUDE_DIRS "."
                    PRIV_REQUIRES esp_driver_spi esp_driver_gpio 
                    )

