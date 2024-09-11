#include "pl_blackbox_mdns_server_configuration.h"

//==============================================================================

namespace PL {

//==============================================================================

BlackBoxMdnsServerConfiguration::BlackBoxMdnsServerConfiguration(std::shared_ptr<MdnsServer> mdnsServer, std::string nvsNamespaceName) :
    BlackBoxNetworkServerConfiguration(mdnsServer, nvsNamespaceName), mdnsServer(mdnsServer) { }

//==============================================================================

}