cmake_minimum_required(VERSION 3.5)

idf_component_register(SRCS "pl_blackbox_base.cpp" "pl_blackbox_modbus_server.cpp" INCLUDE_DIRS "include"
                       REQUIRES "pl_common" "pl_uart" "pl_network" "pl_nvs" "pl_modbus")