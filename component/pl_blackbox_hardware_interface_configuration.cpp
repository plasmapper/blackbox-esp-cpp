#include "pl_blackbox_hardware_interface_configuration.h"

//==============================================================================

const char * const TAG = "pl_blackbox_hardware_interface_configuration";

//==============================================================================

namespace PL {

//==============================================================================

const std::string BlackBoxHardwareInterfaceConfiguration::enabledNvsKey = "enabled";

//==============================================================================

BlackBoxHardwareInterfaceConfiguration::BlackBoxHardwareInterfaceConfiguration(std::shared_ptr<HardwareInterface> hardwareInterface, std::string nvsNamespaceName) :
    nvsNamespaceName(nvsNamespaceName), hardwareInterface(hardwareInterface) {}

//==============================================================================

esp_err_t BlackBoxHardwareInterfaceConfiguration::Lock(TickType_t timeout) {
  esp_err_t error = mutex.Lock(timeout);
  if (error == ESP_OK)
    return ESP_OK;
  if (error == ESP_ERR_TIMEOUT && timeout == 0)
    return ESP_ERR_TIMEOUT;
  ESP_RETURN_ON_ERROR(error, TAG, "mutex lock failed");
  return ESP_OK;
}

//==============================================================================

esp_err_t BlackBoxHardwareInterfaceConfiguration::Unlock() {
  ESP_RETURN_ON_ERROR(mutex.Unlock(), CONFIG_PARAM_TAG, "mutex unlock failed");
  return ESP_OK;
}

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

void BlackBoxHardwareInterfaceConfiguration::Erase() {
  LockGuard lg(*this);
  NvsNamespace nvsNamespace(nvsNamespaceName, NvsAccessMode::readWrite);
  nvsNamespace.Erase();
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