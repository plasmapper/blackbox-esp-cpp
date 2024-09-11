#include "pl_blackbox_stream_server_configuration.h"

//==============================================================================

namespace PL {

//==============================================================================

BlackBoxStreamServerConfiguration::BlackBoxStreamServerConfiguration(std::shared_ptr<StreamServer> streamServer, std::string nvsNamespaceName) :
    BlackBoxServerConfiguration(streamServer, nvsNamespaceName), streamServer(streamServer) { }

//==============================================================================

}