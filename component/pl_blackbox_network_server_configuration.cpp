#include "pl_blackbox_network_server_configuration.h"

//==============================================================================

namespace PL {

//==============================================================================

const std::string BlackBoxNetworkServerConfiguration::portNvsKey = "port";
const std::string BlackBoxNetworkServerConfiguration::maxNumberOfClientsNvsKey = "maxClients";

//==============================================================================

BlackBoxNetworkServerConfiguration::BlackBoxNetworkServerConfiguration(std::shared_ptr<NetworkServer> networkServer, std::string nvsNamespaceName) :
    BlackBoxServerConfiguration(networkServer, nvsNamespaceName), port(networkServer->GetPort()), maxNumberOfClients(networkServer->GetMaxNumberOfClients()), networkServer(networkServer) { }

//==============================================================================

void BlackBoxNetworkServerConfiguration::Load() {
  LockGuard lg(*this);
  NvsNamespace nvsNamespace(nvsNamespaceName, NvsAccessMode::readOnly);
  uint16_t u16Value;
  uint16_t u32Value;

  if (nvsNamespace.Read(portNvsKey, u16Value) == ESP_OK)
    port.SetValue(u16Value);
  if (nvsNamespace.Read(maxNumberOfClientsNvsKey, u32Value) == ESP_OK)
    maxNumberOfClients.SetValue(u32Value);

  BlackBoxServerConfiguration::Load();
}

//==============================================================================

void BlackBoxNetworkServerConfiguration::Save() {
  LockGuard lg(*this);
  NvsNamespace nvsNamespace(nvsNamespaceName, NvsAccessMode::readWrite);
  
  nvsNamespace.Write(portNvsKey, port.GetValue());
  nvsNamespace.Write(maxNumberOfClientsNvsKey, (uint32_t)maxNumberOfClients.GetValue());
  
  BlackBoxServerConfiguration::Save();
}

//==============================================================================

void BlackBoxNetworkServerConfiguration::Apply() {
  LockGuard lg(*this, *networkServer);

  networkServer->SetPort(port.GetValue());
  networkServer->SetMaxNumberOfClients(maxNumberOfClients.GetValue());

  BlackBoxServerConfiguration::Apply();
}

//==============================================================================

}