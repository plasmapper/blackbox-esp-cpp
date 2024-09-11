#include "pl_blackbox_network_interface_configuration.h"
#include "esp_check.h"

//==============================================================================

namespace PL {

//==============================================================================

const std::string BlackBoxNetworkInterfaceConfiguration::ipV4AddressNvsKey = "ip4Address";
const std::string BlackBoxNetworkInterfaceConfiguration::ipV4NetmaskNvsKey = "ip4Netmask";
const std::string BlackBoxNetworkInterfaceConfiguration::ipV4GatewayNvsKey = "ip4Gateway";
const std::string BlackBoxNetworkInterfaceConfiguration::ipV6GlobalAddressNvsKey = "ip6GlobAddr";
const std::string BlackBoxNetworkInterfaceConfiguration::ipV4DhcpClientEnabledNvsKey = "ip4DhcpcEn";
const std::string BlackBoxNetworkInterfaceConfiguration::ipV6DhcpClientEnabledNvsKey = "ip6DhcpcEn";

//==============================================================================

BlackBoxNetworkInterfaceConfiguration::BlackBoxNetworkInterfaceConfiguration(std::shared_ptr<NetworkInterface> networkInterface, std::string nvsNamespaceName) :
    BlackBoxHardwareInterfaceConfiguration(networkInterface, nvsNamespaceName),
    ipV4Address(networkInterface->GetIpV4Address()), ipV4Netmask(networkInterface->GetIpV4Netmask()), ipV4Gateway(networkInterface->GetIpV4Gateway()),
    ipV6GlobalAddress(networkInterface->GetIpV6GlobalAddress()),
    ipV4DhcpClientEnabled(networkInterface->IsIpV4DhcpClientEnabled()), ipV6DhcpClientEnabled(networkInterface->IsIpV6DhcpClientEnabled()),
    networkInterface(networkInterface) { }

//==============================================================================

void BlackBoxNetworkInterfaceConfiguration::Load() {
  LockGuard lg(*this);
  NvsNamespace nvsNamespace(nvsNamespaceName, NvsAccessMode::readOnly);
  uint8_t u8Value;
  uint32_t u32Value;
  uint32_t ipV6Address[4];

  if (nvsNamespace.Read(ipV4AddressNvsKey, u32Value) == ESP_OK)
    ipV4Address.SetValue(u32Value);
  if (nvsNamespace.Read(ipV4NetmaskNvsKey, u32Value) == ESP_OK)
    ipV4Netmask.SetValue(u32Value);
  if (nvsNamespace.Read(ipV4GatewayNvsKey, u32Value) == ESP_OK)
    ipV4Gateway.SetValue(u32Value);
  if (nvsNamespace.Read(ipV6GlobalAddressNvsKey, ipV6Address, sizeof(ipV6Address), NULL) == ESP_OK)
    ipV6GlobalAddress.SetValue(IpV6Address(ipV6Address[0], ipV6Address[1], ipV6Address[2], ipV6Address[3]));

  if (nvsNamespace.Read(ipV4DhcpClientEnabledNvsKey, u8Value) == ESP_OK)
    ipV4DhcpClientEnabled.SetValue(u8Value);
  if (nvsNamespace.Read(ipV6DhcpClientEnabledNvsKey, u8Value) == ESP_OK)
    ipV6DhcpClientEnabled.SetValue(u8Value);
    
  BlackBoxHardwareInterfaceConfiguration::Load();
}

//==============================================================================

void BlackBoxNetworkInterfaceConfiguration::Save() {
  LockGuard lg(*this);
  NvsNamespace nvsNamespace(nvsNamespaceName, NvsAccessMode::readWrite);
  
  nvsNamespace.Write(ipV4AddressNvsKey, ipV4Address.GetValue().u32);
  nvsNamespace.Write(ipV4NetmaskNvsKey, ipV4Netmask.GetValue().u32);
  nvsNamespace.Write(ipV4GatewayNvsKey, ipV4Gateway.GetValue().u32);
  
  uint32_t ipV6Address[4];
  memcpy(ipV6Address, ipV6GlobalAddress.GetValue().u32, sizeof(ipV6Address));
  nvsNamespace.Write(ipV6GlobalAddressNvsKey, ipV6Address, sizeof(ipV6Address));

  nvsNamespace.Write(ipV4DhcpClientEnabledNvsKey, (uint8_t)ipV4DhcpClientEnabled.GetValue());
  nvsNamespace.Write(ipV6DhcpClientEnabledNvsKey, (uint8_t)ipV6DhcpClientEnabled.GetValue());
  
  BlackBoxHardwareInterfaceConfiguration::Save();
}

//==============================================================================

void BlackBoxNetworkInterfaceConfiguration::Apply() {
  LockGuard lg(*this, *networkInterface);

  if (ipV4DhcpClientEnabled.GetValue())
    networkInterface->EnableIpV4DhcpClient();
  else {
    networkInterface->DisableIpV4DhcpClient();
    networkInterface->SetIpV4Address(ipV4Address.GetValue());
    networkInterface->SetIpV4Netmask(ipV4Netmask.GetValue());
    networkInterface->SetIpV4Gateway(ipV4Gateway.GetValue());
  }

  if (ipV6DhcpClientEnabled.GetValue())
    networkInterface->EnableIpV6DhcpClient();
  else {
    networkInterface->DisableIpV6DhcpClient();
    networkInterface->SetIpV6GlobalAddress(ipV6GlobalAddress.GetValue());
  }

  BlackBoxHardwareInterfaceConfiguration::Apply();
}

//==============================================================================

}