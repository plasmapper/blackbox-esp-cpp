#include "pl_blackbox_http_server_configuration.h"

//==============================================================================

namespace PL {

//==============================================================================

BlackBoxHttpServerConfiguration::BlackBoxHttpServerConfiguration(std::shared_ptr<HttpServer> httpServer, std::string nvsNamespaceName) :
    BlackBoxNetworkServerConfiguration(httpServer, nvsNamespaceName), httpServer(httpServer) { }

//==============================================================================

}