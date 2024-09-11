#include "pl_blackbox_ethernet_configuration.h"

//==============================================================================

namespace PL {

//==============================================================================

BlackBoxEthernetConfiguration::BlackBoxEthernetConfiguration(std::shared_ptr<Ethernet> ethernet, std::string nvsNamespaceName) :
  BlackBoxNetworkInterfaceConfiguration(ethernet, nvsNamespaceName), ethernet(ethernet) {}

//==============================================================================

}