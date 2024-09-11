#pragma once
#include "pl_blackbox_network_server_configuration.h"
#include "pl_mdns.h"

//==============================================================================

namespace PL {

//==============================================================================

/// @brief BlackBox mDNS server configuration
class BlackBoxMdnsServerConfiguration : public BlackBoxNetworkServerConfiguration {
public:
  /// @brief Creates a BlackBox mDNS server configuration
  /// @param mDnsServer mDNS server
  /// @param nvsNamespaceName NVS namespace name
  BlackBoxMdnsServerConfiguration(std::shared_ptr<MdnsServer> mdnsServer, std::string nvsNamespaceName);

private:
  std::shared_ptr<MdnsServer> mdnsServer;
};

//==============================================================================

}