#pragma once
#include "pl_blackbox_server_configuration.h"
#include "pl_network.h"

//==============================================================================

namespace PL {

//==============================================================================

/// @brief BlackBox network server configuration
class BlackBoxNetworkServerConfiguration : public BlackBoxServerConfiguration {
public:
  /// @brief port parameter NVS key
  static const std::string portNvsKey;

  /// @brief max number of clients parameter NVS key
  static const std::string maxNumberOfClientsNvsKey;

  /// @brief Creates a BlackBox network server configuration
  /// @param networkServer network server
  /// @param nvsNamespaceName NVS namespace name
  BlackBoxNetworkServerConfiguration(std::shared_ptr<NetworkServer> networkServer, std::string nvsNamespaceName);

  /// @brief port parameter
  BlackBoxConfigurationParameter<uint16_t> port;

  /// @brief max number of clients parameter
  BlackBoxConfigurationParameter<size_t> maxNumberOfClients;

  void Load() override;
  void Save() override;
  void Apply() override;

private:
  std::shared_ptr<NetworkServer> networkServer;
};

//==============================================================================

}