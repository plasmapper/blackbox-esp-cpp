#pragma once
#include "pl_blackbox_network_interface_configuration.h"

//==============================================================================

namespace PL {

//==============================================================================

/// @brief BlackBox Wi-Fi configuration
class BlackBoxWiFiStationConfiguration : public BlackBoxNetworkInterfaceConfiguration {
public:
  /// @brief SSID parameter NVS key
  static const std::string ssidNvsKey;
  /// @brief password parameter NVS key
  static const std::string passwordNvsKey;

  /// @brief Creates a BlackBox Wi-Fi station configuration
  /// @param wifiStation Wi-Fi station
  /// @param nvsNamespaceName NVS namespace name
  BlackBoxWiFiStationConfiguration(std::shared_ptr<WiFiStation> wifiStation, std::string nvsNamespaceName);

  /// @brief SSID parameter
  BlackBoxConfigurationParameter<std::string> ssid;

    /// @brief password parameter
  BlackBoxConfigurationParameter<std::string> password;

  void Load() override;
  void Save() override;
  void Apply() override;

private:
  std::shared_ptr<WiFiStation> wifiStation;
};

//==============================================================================

}