#pragma once
#include "pl_blackbox_hardware_interface_configuration.h"
#include "pl_network.h"

//==============================================================================

namespace PL {

//==============================================================================

/// @brief BlackBox network interface configuration
class BlackBoxNetworkInterfaceConfiguration : public BlackBoxHardwareInterfaceConfiguration {
public:
  /// @brief IPv4 address parameter NVS key
  static const std::string ipV4AddressNvsKey;
  /// @brief IPv4 netmask parameter NVS key
  static const std::string ipV4NetmaskNvsKey;
  /// @brief IPv4 gateway parameter NVS key
  static const std::string ipV4GatewayNvsKey;
  /// @brief IPv6 global address parameter NVS key
  static const std::string ipV6GlobalAddressNvsKey;
  /// @brief IPv4 DHCP client enabled parameter NVS key
  static const std::string ipV4DhcpClientEnabledNvsKey;
  /// @brief IPv6 DHCP client enabled parameter NVS key
  static const std::string ipV6DhcpClientEnabledNvsKey;

  /// @brief Creates a BlackBox network interface configuration
  /// @param networkInterface network interface
  /// @param nvsNamespaceName NVS namespace name
  BlackBoxNetworkInterfaceConfiguration(std::shared_ptr<NetworkInterface> networkInterface, std::string nvsNamespaceName);

  /// @brief IPv4 address parameter
  BlackBoxConfigurationParameter<IpV4Address> ipV4Address;

  /// @brief IPv4 netmask parameter
  BlackBoxConfigurationParameter<IpV4Address> ipV4Netmask;

  /// @brief IPv4 gateway parameter
  BlackBoxConfigurationParameter<IpV4Address> ipV4Gateway;

  /// @brief IPv6 global address parameter
  BlackBoxConfigurationParameter<IpV6Address> ipV6GlobalAddress;

  /// @brief IPv4 DHCP client enabled parameter
  BlackBoxConfigurationParameter<bool> ipV4DhcpClientEnabled;

  /// @brief IPv6 DHCP client enabled parameter
  BlackBoxConfigurationParameter<bool> ipV6DhcpClientEnabled;

  void Load() override;
  void Save() override;
  void Apply() override;

private:
  std::shared_ptr<NetworkInterface> networkInterface;
};

//==============================================================================

}