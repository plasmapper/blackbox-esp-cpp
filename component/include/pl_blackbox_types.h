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
  wifiStation = 4,
  /// @brief USB device CDC
  usbDeviceCdc = 5
};

//==============================================================================

/// @brief BlackBox server
enum class BlackBoxServerType : uint8_t {
  /// @brief unknown
  unknown = 0,
  /// @brief stream server
  streamServer = 1,
  /// @brief network server
  networkServer = 2,
  /// @brief Modbus serial server
  streamModbusServer = 3,
  /// @brief Modbus TCP server
  networkModbusServer = 4,
  /// @brief HTTP server
  httpServer = 5,
  /// @brief mDNS server
  mdnsServer = 6
};

//==============================================================================

}