#pragma once
#include "pl_blackbox_network_interface_configuration.h"

//==============================================================================

namespace PL {

//==============================================================================

/// @brief BlackBox Ethernet configuration
class BlackBoxEthernetConfiguration : public BlackBoxNetworkInterfaceConfiguration {
public:
  /// @brief Creates a BlackBox Ethernet configuration
  /// @param ethernet Ethernet
  /// @param nvsNamespaceName NVS namespace name
  BlackBoxEthernetConfiguration(std::shared_ptr<Ethernet> ethernet, std::string nvsNamespaceName);

private:
  std::shared_ptr<Ethernet> ethernet;
};

//==============================================================================

}