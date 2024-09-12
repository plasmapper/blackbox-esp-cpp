#include "pl_blackbox_configuration.h"

//==============================================================================

namespace PL {

//==============================================================================

BlackBoxConfiguration::BlackBoxConfiguration(std::string nvsNamespaceName) : nvsNamespaceName(nvsNamespaceName) { }

//==============================================================================

esp_err_t BlackBoxConfiguration::Lock(TickType_t timeout) {
  ESP_RETURN_ON_ERROR(mutex.Lock(timeout), CONFIG_PARAM_TAG, "mutex lock failed");
  return ESP_OK;
}

//==============================================================================

esp_err_t BlackBoxConfiguration::Unlock() {
  ESP_RETURN_ON_ERROR(mutex.Unlock(), CONFIG_PARAM_TAG, "mutex unlock failed");
  return ESP_OK;
}

//==============================================================================

std::string BlackBoxConfiguration::GetNvsNamespaceName() {
  LockGuard lg(*this);
  return nvsNamespaceName;
}

//==============================================================================

void BlackBoxConfiguration::SetNvsNamespaceName(const std::string& nvsNamespaceName) {
  LockGuard lg(*this);
  this->nvsNamespaceName = nvsNamespaceName;
}

//==============================================================================

void BlackBoxConfiguration::Erase() {
  LockGuard lg(*this);
  NvsNamespace nvsNamespace(nvsNamespaceName, NvsAccessMode::readWrite);
  nvsNamespace.Erase();
}

//==============================================================================

}