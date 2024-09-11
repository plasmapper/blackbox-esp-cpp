#pragma once
#include "pl_blackbox_server_configuration.h"

//==============================================================================

namespace PL {

//==============================================================================

/// @brief BlackBox stream server configuration
class BlackBoxStreamServerConfiguration : public BlackBoxServerConfiguration {
public:
  /// @brief Creates a BlackBox stream server configuration
  /// @param streamServer stream server
  /// @param nvsNamespaceName NVS namespace name
  BlackBoxStreamServerConfiguration(std::shared_ptr<StreamServer> streamServer, std::string nvsNamespaceName);

private:
  std::shared_ptr<StreamServer> streamServer;
};

//==============================================================================

}