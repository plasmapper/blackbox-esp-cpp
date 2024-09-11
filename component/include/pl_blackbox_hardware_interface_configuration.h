#pragma once
#include "pl_blackbox_configuration.h"
#include "pl_nvs.h"

//==============================================================================

namespace PL {

//==============================================================================

/// @brief BlackBox hardware interface configuration
class BlackBoxHardwareInterfaceConfiguration : public BlackBoxConfiguration {
public:
  /// @brief enabled parameter NVS key
  static const std::string enabledNvsKey;

  /// @brief Creates a BlackBox hardware interface configuration
  /// @param hardwareInterface hardware interface
  /// @param nvsNamespaceName NVS namespace name
  BlackBoxHardwareInterfaceConfiguration(std::shared_ptr<HardwareInterface> hardwareInterface, std::string nvsNamespaceName);

  /// @brief enabled parameter
  BlackBoxConfigurationParameter<bool> enabled = BlackBoxConfigurationParameter<bool>(true);

  /// @brief Gets the hardware interface
  /// @return hardware interface
  std::shared_ptr<HardwareInterface> GetHardwareInterface();

  void Load() override;
  void Save() override;

  /// @brief Applies the configuration to the hardware interface
  virtual void Apply();

private:
  std::shared_ptr<HardwareInterface> hardwareInterface;
};

//==============================================================================

}