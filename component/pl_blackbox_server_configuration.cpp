#include "pl_blackbox_server_configuration.h"

//==============================================================================

namespace PL {

//==============================================================================


const std::string BlackBoxServerConfiguration::enabledNvsKey = "enabled";

//==============================================================================

BlackBoxServerConfiguration::BlackBoxServerConfiguration(std::shared_ptr<Server> server, std::string nvsNamespaceName) :
  BlackBoxConfiguration(nvsNamespaceName), server(server) {}

//==============================================================================

std::shared_ptr<Server> BlackBoxServerConfiguration::GetServer() {
  LockGuard lg(*this);
  return server;
}

//==============================================================================

void BlackBoxServerConfiguration::Load() {
  LockGuard lg(*this);
  NvsNamespace nvsNamespace(nvsNamespaceName, NvsAccessMode::readOnly);
  uint8_t u8Value;

  if (nvsNamespace.Read(enabledNvsKey, u8Value) == ESP_OK)
    enabled.SetValue(u8Value);
}

//==============================================================================

void BlackBoxServerConfiguration::Save() {
  LockGuard lg(*this);
  NvsNamespace nvsNamespace(nvsNamespaceName, NvsAccessMode::readWrite);

  nvsNamespace.Write(enabledNvsKey, (uint8_t)enabled.GetValue());
}

//==============================================================================

void BlackBoxServerConfiguration::Apply() {
  LockGuard lg(*this, *server);

  if (enabled.GetValue())
    server->Enable();
  else
    server->Disable();
}

//==============================================================================

}