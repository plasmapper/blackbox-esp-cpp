#pragma once
#include "pl_blackbox_configuration_parameter.h"
#include "pl_nvs.h"

//==============================================================================

namespace PL {

//==============================================================================

/// @brief Base class for a BlackBox configuration
class BlackBoxConfiguration : public Lockable {
public:
  /// @brief Creates a BlackBox configuration
  /// @param nvsNamespaceName NVS namespace name
  BlackBoxConfiguration(std::string nvsNamespaceName);
  ~BlackBoxConfiguration() {}
  BlackBoxConfiguration(const BlackBoxConfiguration&) = delete;
  BlackBoxConfiguration& operator=(const BlackBoxConfiguration&) = delete;

  esp_err_t Lock(TickType_t timeout = portMAX_DELAY) override;
  esp_err_t Unlock() override;

  /// @brief Gets NVS namespace name
  /// @return NVS namespace name
  std::string GetNvsNamespaceName();

  /// @brief Sets NVS namespace name
  /// @param NVS namespace name
  void SetNvsNamespaceName(const std::string& nvsNamespaceName);

  /// @brief Loads the configuration
  virtual void Load() = 0;

  /// @brief Saves the configuration
  virtual void Save() = 0;

  /// @brief Erases the configuration
  void Erase();

protected:
  std::string nvsNamespaceName;

private:
  Mutex mutex;
};

//==============================================================================

}