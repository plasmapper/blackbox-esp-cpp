#pragma once
#include "pl_blackbox_network_server_configuration.h"
#include "pl_http.h"

//==============================================================================

namespace PL {

//==============================================================================

/// @brief BlackBox HTTP server configuration
class BlackBoxHttpServerConfiguration : public BlackBoxNetworkServerConfiguration {
public:
  /// @brief Creates a BlackBox HTTP server configuration
  /// @param httpServer HTTP server
  /// @param nvsNamespaceName NVS namespace name
  BlackBoxHttpServerConfiguration(std::shared_ptr<HttpServer> httpServer, std::string nvsNamespaceName);

private:
  std::shared_ptr<HttpServer> httpServer;
};

//==============================================================================

}