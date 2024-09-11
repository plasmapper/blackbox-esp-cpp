#pragma once
#include "pl_blackbox_server_configuration.h"
#include "pl_modbus.h"

//==============================================================================

namespace PL {

//==============================================================================

/// @brief BlackBox Modbus server configuration
class BlackBoxModbusServerConfiguration : public BlackBoxServerConfiguration {
public:
  /// @brief protocol parameter NVS key
  static const std::string protocolNvsKey;

  /// @brief station address parameter NVS key
  static const std::string stationAddressNvsKey;

  /// @brief port parameter NVS key
  static const std::string portNvsKey;

  /// @brief max number of clients parameter NVS key
  static const std::string maxNumberOfClientsNvsKey;

  /// @brief Creates a BlackBox Modbus server configuration
  /// @param modbusServer Modbus server
  /// @param nvsNamespaceName NVS namespace name
  BlackBoxModbusServerConfiguration(std::shared_ptr<ModbusServer> modbusServer, std::string nvsNamespaceName);

  /// @brief protocol parameter
  BlackBoxConfigurationParameter<ModbusProtocol> protocol;

  /// @brief station address parameter
  BlackBoxConfigurationParameter<uint8_t> stationAddress;

  /// @brief port parameter
  BlackBoxConfigurationParameter<uint16_t> port;

  /// @brief max number of clients parameter
  BlackBoxConfigurationParameter<size_t> maxNumberOfClients;

  void Load() override;
  void Save() override;
  void Apply() override;

private:
  std::shared_ptr<ModbusServer> modbusServer;
};

//==============================================================================

}