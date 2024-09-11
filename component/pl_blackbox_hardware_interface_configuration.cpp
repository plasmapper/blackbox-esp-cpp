#include "pl_blackbox_hardware_interface_configuration.h"

//==============================================================================

namespace PL {

//==============================================================================

const std::string BlackBoxHardwareInterfaceConfiguration::enabledNvsKey = "enabled";

//==============================================================================

BlackBoxHardwareInterfaceConfiguration::BlackBoxHardwareInterfaceConfiguration(std::shared_ptr<HardwareInterface> hardwareInterface, std::string nvsNamespaceName) :
    BlackBoxConfiguration(nvsNamespaceName), hardwareInterface(hardwareInterface) {}

//==============================================================================

std::shared_ptr<HardwareInterface> BlackBoxHardwareInterfaceConfiguration::GetHardwareInterface() {
  LockGuard lg(*this);
  return hardwareInterface;
}

//==============================================================================

void BlackBoxHardwareInterfaceConfiguration::Load() {
  LockGuard lg(*this);
  NvsNamespace nvsNamespace(nvsNamespaceName, NvsAccessMode::readOnly);
  uint8_t u8Value;

  if (nvsNamespace.Read(enabledNvsKey, u8Value) == ESP_OK)
    enabled.SetValue(u8Value);
}

//==============================================================================

void BlackBoxHardwareInterfaceConfiguration::Save() {
  LockGuard lg(*this);
  NvsNamespace nvsNamespace(nvsNamespaceName, NvsAccessMode::readWrite);

  nvsNamespace.Write(enabledNvsKey, (uint8_t)enabled.GetValue());
}

//==============================================================================

void BlackBoxHardwareInterfaceConfiguration::Apply() {
  LockGuard lg(*this, *hardwareInterface);
  
  if (enabled.GetValue())
    hardwareInterface->Enable();
  else
    hardwareInterface->Disable();
}

//==============================================================================

}