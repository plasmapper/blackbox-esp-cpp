cmake_minimum_required(VERSION 3.5)

idf_component_register(SRCS "pl_blackbox_mdns_server_configuration.cpp" "pl_blackbox_http_server_configuration.cpp" "pl_blackbox_base.cpp"
                       "pl_blackbox_configuration.cpp" "pl_blackbox_hardware_interface_configuration.cpp" "pl_blackbox_uart_configuration.cpp" 
                       "pl_blackbox_network_interface_configuration.cpp" "pl_blackbox_ethernet_configuration.cpp" "pl_blackbox_wifi_station_configuration.cpp"
                       "pl_blackbox_usb_device_cdc_configuration.cpp"
                       "pl_blackbox_server_configuration.cpp" "pl_blackbox_stream_server_configuration.cpp" "pl_blackbox_network_server_configuration.cpp"
                       "pl_blackbox_modbus_server_configuration.cpp" "pl_blackbox_http_server_configuration.cpp" "pl_blackbox_mdns_server_configuration.cpp"
                       "pl_blackbox_modbus_server.cpp"
                       INCLUDE_DIRS "include" REQUIRES "pl_common" "pl_uart" "pl_network" "pl_nvs" "pl_modbus")