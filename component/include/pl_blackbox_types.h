#pragma once
#include "pl_common.h"

//==============================================================================

namespace PL {

//==============================================================================

/// @brief BlackBox hardware information
struct BlackBoxHardwareInfo {
  /// @brief Name
  std::string name;
  /// @brief Version
  SemanticVersion version;
  /// @brief Unique ID
  std::string uid;
};

//==============================================================================

/// @brief BlackBox firmware information
struct BlackBoxFirmwareInfo {
  /// @brief Name
  std::string name;
  /// @brief Version
  SemanticVersion version;
};

//==============================================================================

/// @brief BlackBox hardware interface
enum class BlackBoxHardwareInterfaceType : uint8_t {
  /// @brief unknown
  unknown = 0,
  /// @brief UART
  uart = 1,
  /// @brief network interface
  networkInterface = 2,
  /// @brief Ethernet
  ethernet = 3,
  /// @brief Wi-Fi station
  wifiStation = 4
};

//==============================================================================

/// @brief BlackBox server
enum class BlackBoxServerType : uint8_t {
  /// @brief unknown
  unknown = 0,
  /// @brief UART server
  uartServer = 1,
  /// @brief network server
  networkServer = 2,
  /// @brief Modbus serial server
  modbusSerialServer = 3,
  /// @brief Modbus TCP server
  modbusTcpServer = 4
};

//==============================================================================

}