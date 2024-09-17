#pragma once
#include "pl_blackbox_configuration.h"
#include "pl_blackbox_configuration_parameter.h"
#include "pl_nvs.h"

//==============================================================================

namespace PL {

//==============================================================================

/// @brief BlackBox hardware interface configuration
class BlackBoxHardwareInterfaceConfiguration : public Lockable, public BlackBoxConfiguration {
public:
  /// @brief enabled parameter NVS key
  static const std::string enabledNvsKey;

  /// @brief Creates a BlackBox hardware interface configuration
  /// @param hardwareInterface hardware interface
  /// @param nvsNamespaceName NVS namespace name
  BlackBoxHardwareInterfaceConfiguration(std::shared_ptr<HardwareInterface> hardwareInterface, std::string nvsNamespaceName);
  ~BlackBoxHardwareInterfaceConfiguration() {}
  BlackBoxHardwareInterfaceConfiguration(const BlackBoxConfiguration&) = delete;
  BlackBoxHardwareInterfaceConfiguration& operator=(const BlackBoxConfiguration&) = delete;

  /// @brief enabled parameter
  BlackBoxConfigurationParameter<bool> enabled = BlackBoxConfigurationParameter<bool>(true);

  /// @brief Gets the hardware interface
  /// @return hardware interface
  std::shared_ptr<HardwareInterface> GetHardwareInterface();

  esp_err_t Lock(TickType_t timeout = portMAX_DELAY) override;
  esp_err_t Unlock() override;
  void Load() override;
  void Save() override;
  void Erase() override;

  /// @brief Applies the configuration to the hardware interface
  virtual void Apply();

protected:
  Mutex mutex;
  std::string nvsNamespaceName;

private:
  std::shared_ptr<HardwareInterface> hardwareInterface;
};

//==============================================================================

}