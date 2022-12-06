#include "pl_blackbox_base.h"
#include "pl_uart.h"
#include "pl_network.h"
#include "pl_nvs.h"
#include "pl_modbus.h"
#include "esp_check.h"

//==============================================================================

static const char* TAG = "pl_blackbox_base";

//==============================================================================

namespace PL {

//==============================================================================

const std::string BlackBox::hardwareInfoNvsNamespace = "plbbHwInfo";
const std::string BlackBox::hardwareInfoNameNvsKey = "name";
const std::string BlackBox::hardwareInfoMajorVersionNvsKey = "verMajor";
const std::string BlackBox::hardwareInfoMinorVersionNvsKey = "verMinor";
const std::string BlackBox::hardwareInfoPatchVersionNvsKey = "verPatch";
const std::string BlackBox::hardwareInfoUidNvsKey = "uid";

const std::string BlackBox::configurationNvsNamespace = "plbbConfig";
const std::string BlackBox::configurationDeviceNameNvsKey = "devName";
const std::string BlackBox::configurationHardwareInterfaceNvsKeyPrefix = "hwi";
const std::string BlackBox::configurationHardwareInterfaceEnabledNvsKeySuffix = "Enabled";
const std::string BlackBox::configurationServerNvsKeyPrefix = "srv";
const std::string BlackBox::configurationServerEnabledNvsKeySuffix = "Enabled";

const std::string BlackBox::configurationUartBaudRateNvsKeySuffix = "BaudRate";
const std::string BlackBox::configurationUartDataBitsNvsKeySuffix = "DataBits";
const std::string BlackBox::configurationUartParityNvsKeySuffix = "Parity";
const std::string BlackBox::configurationUartStopBitsNvsKeySuffix = "StopBits";
const std::string BlackBox::configurationUartFlowControlNvsKeySuffix = "FlwCntrl";

const std::string BlackBox::configurationNetworkInterfaceIpV4DhcpClientEnabledNvsKeySuffix = "Dhcpc4En";
const std::string BlackBox::configurationNetworkInterfaceIpV6DhcpClientEnabledNvsKeySuffix = "Dhcpc6En";
const std::string BlackBox::configurationNetworkInterfaceIpV4AddressNvsKeySuffix = "Ip4Addr";
const std::string BlackBox::configurationNetworkInterfaceIpV4NetmaskNvsKeySuffix = "Ip4NM";
const std::string BlackBox::configurationNetworkInterfaceIpV4GatewayNvsKeySuffix = "Ip4GW";
const std::string BlackBox::configurationNetworkInterfaceIpV6GlobalAddressNvsKeySuffix = "Ip6GAddr";

const std::string BlackBox::configurationWiFiSsidNvsKeySuffix = "WiFiSsid";
const std::string BlackBox::configurationWiFiPasswordNvsKeySuffix = "WiFiPwd";

const std::string BlackBox::configurationNetworkServerPortNvsKeySuffix = "Port";
const std::string BlackBox::configurationNetworkServerMaxNumberOfClientsNvsKeySuffix = "MaxClnts";
const std::string BlackBox::configurationModbusServerProtocolNvsKeySuffix = "Proto";
const std::string BlackBox::configurationModbusServerStationAddressNvsKeySuffix = "StaAddr";

//==============================================================================

esp_err_t BlackBox::Lock (TickType_t timeout) {
  esp_err_t error = mutex.Lock (timeout);
  if (error == ESP_OK)
    return ESP_OK;
  if (error == ESP_ERR_TIMEOUT && timeout == 0)
    return ESP_ERR_TIMEOUT;
  ESP_RETURN_ON_ERROR (error, TAG, "mutex lock failed");
  return ESP_OK;
}

//==============================================================================

esp_err_t BlackBox::Unlock() {
  ESP_RETURN_ON_ERROR (mutex.Unlock(), TAG, "mutex unlock failed");
  return ESP_OK;
}

//==============================================================================

esp_err_t BlackBox::Restart() {
  LockGuard lg (*this);
  esp_restart();
  return ESP_OK;
}

//==============================================================================

void BlackBox::LoadConfiguration() {
  NvsNamespace configurationNvs (configurationNvsNamespace, NvsAccessMode::readOnly);
  uint8_t u8Value;
  uint16_t u16Value;
  uint32_t u32Value;
  std::string stringValue;
  
  if (configurationNvs.Read (configurationDeviceNameNvsKey, stringValue) == ESP_OK)
    SetName (stringValue);

  auto hardwareInterfaces = GetHardwareInterfaces();
  for (size_t i = 0; i < hardwareInterfaces.size(); i++) {
    std::string keyBase = configurationHardwareInterfaceNvsKeyPrefix + std::to_string(i);

    bool enableHardwareInterface = hardwareInterfaces[i]->IsEnabled();
    hardwareInterfaces[i]->Disable();

    if (auto uart = dynamic_cast<Uart*>(hardwareInterfaces[i].get())) {
      if (configurationNvs.Read (keyBase + configurationUartBaudRateNvsKeySuffix, u32Value) == ESP_OK)
        uart->SetBaudRate (u32Value);
      if (configurationNvs.Read (keyBase + configurationUartDataBitsNvsKeySuffix, u16Value) == ESP_OK)
        uart->SetDataBits (u16Value);
      if (configurationNvs.Read (keyBase + configurationUartParityNvsKeySuffix, u8Value) == ESP_OK)
        uart->SetParity ((UartParity)u8Value);
      if (configurationNvs.Read (keyBase + configurationUartStopBitsNvsKeySuffix, u8Value) == ESP_OK)
        uart->SetStopBits ((UartStopBits)u8Value);
      if (configurationNvs.Read (keyBase + configurationUartFlowControlNvsKeySuffix, u8Value) == ESP_OK)
        uart->SetFlowControl ((UartFlowControl)u8Value);
    }

    if (auto networkInterface = dynamic_cast<NetworkInterface*>(hardwareInterfaces[i].get())) {
      if (configurationNvs.Read (keyBase + configurationNetworkInterfaceIpV4DhcpClientEnabledNvsKeySuffix, u8Value) == ESP_OK) {
        if (u8Value)
          networkInterface->EnableIpV4DhcpClient();
        else {
          networkInterface->DisableIpV4DhcpClient();
          if (configurationNvs.Read (keyBase + configurationNetworkInterfaceIpV4AddressNvsKeySuffix, u32Value) == ESP_OK)
            networkInterface->SetIpV4Address (u32Value);
          if (configurationNvs.Read (keyBase + configurationNetworkInterfaceIpV4NetmaskNvsKeySuffix, u32Value) == ESP_OK)
            networkInterface->SetIpV4Netmask (u32Value);
          if (configurationNvs.Read (keyBase + configurationNetworkInterfaceIpV4GatewayNvsKeySuffix, u32Value) == ESP_OK)
            networkInterface->SetIpV4Gateway (u32Value);        
        }  
      }
      if (configurationNvs.Read (keyBase + configurationNetworkInterfaceIpV6DhcpClientEnabledNvsKeySuffix, u8Value) == ESP_OK) {
        if (u8Value)
          networkInterface->EnableIpV6DhcpClient();
        else {
          networkInterface->DisableIpV6DhcpClient();
          uint32_t ipV6Address[4];
          if (configurationNvs.Read (keyBase + configurationNetworkInterfaceIpV6GlobalAddressNvsKeySuffix, ipV6Address, sizeof (ipV6Address), NULL) == ESP_OK) {
            IpV6Address address (ipV6Address[0], ipV6Address[1], ipV6Address[2], ipV6Address[3]);
            networkInterface->SetIpV6GlobalAddress (address);
          } 
        }
      }
    }

    if (auto wifiStation = dynamic_cast<WiFiStation*>(hardwareInterfaces[i].get())) {
      if (configurationNvs.Read (keyBase + configurationWiFiSsidNvsKeySuffix, stringValue) == ESP_OK)
        wifiStation->SetSsid (stringValue);
      if (configurationNvs.Read (keyBase + configurationWiFiPasswordNvsKeySuffix, stringValue) == ESP_OK)
        wifiStation->SetPassword (stringValue);
    }
    
    if (configurationNvs.Read (keyBase + configurationHardwareInterfaceEnabledNvsKeySuffix, u8Value) == ESP_OK)
      enableHardwareInterface = u8Value;

    if (enableHardwareInterface)
      hardwareInterfaces[i]->Enable();
  }

  auto servers = GetServers();
  for (size_t i = 0; i < servers.size(); i++) {
    std::string keyBase = configurationServerNvsKeyPrefix + std::to_string(i);

    bool enableServer = servers[i]->IsEnabled();
    servers[i]->Disable();

    if (auto networkServer = dynamic_cast<NetworkServer*>(servers[i].get())) {
      if (configurationNvs.Read (keyBase + configurationNetworkServerPortNvsKeySuffix, u16Value) == ESP_OK)
        networkServer->SetPort (u16Value);
      if (configurationNvs.Read (keyBase + configurationNetworkServerMaxNumberOfClientsNvsKeySuffix, u16Value) == ESP_OK)
        networkServer->SetMaxNumberOfClients (u16Value);
    }

    if (auto modbusServer = dynamic_cast<ModbusServer*>(servers[i].get())) {
      if (configurationNvs.Read (keyBase + configurationModbusServerProtocolNvsKeySuffix, u8Value) == ESP_OK)
        modbusServer->SetProtocol ((ModbusProtocol)u8Value);
      if (configurationNvs.Read (keyBase + configurationModbusServerStationAddressNvsKeySuffix, u8Value) == ESP_OK)
        modbusServer->SetStationAddress (u8Value);
      if (auto baseServer = modbusServer->GetBaseServer().lock()) {
        if (auto networkServer = dynamic_cast<NetworkServer*>(baseServer.get())) {
          if (configurationNvs.Read (keyBase + configurationNetworkServerPortNvsKeySuffix, u16Value) == ESP_OK)
            networkServer->SetPort (u16Value);
          if (configurationNvs.Read (keyBase + configurationNetworkServerMaxNumberOfClientsNvsKeySuffix, u16Value) == ESP_OK)
            networkServer->SetMaxNumberOfClients (u16Value);          
        }
      }
    }

    if (configurationNvs.Read (keyBase + configurationServerEnabledNvsKeySuffix, u8Value) == ESP_OK)
      enableServer = u8Value;
    
    if (enableServer)
      servers[i]->Enable();
  }
}

//==============================================================================

void BlackBox::SaveConfiguration() {
  NvsNamespace configurationNvs (configurationNvsNamespace, NvsAccessMode::readWrite);  
  configurationNvs.Erase();

  configurationNvs.Write (configurationDeviceNameNvsKey, GetName());

  auto hardwareInterfaces = GetHardwareInterfaces();
  for (size_t i = 0; i < hardwareInterfaces.size(); i++) {
    std::string keyBase = configurationHardwareInterfaceNvsKeyPrefix + std::to_string(i);

    if (auto uart = dynamic_cast<Uart*>(hardwareInterfaces[i].get())) {
      configurationNvs.Write (keyBase + configurationUartBaudRateNvsKeySuffix, uart->GetBaudRate());
      configurationNvs.Write (keyBase + configurationUartDataBitsNvsKeySuffix, uart->GetDataBits());
      configurationNvs.Write (keyBase + configurationUartParityNvsKeySuffix, (uint8_t)uart->GetParity());
      configurationNvs.Write (keyBase + configurationUartStopBitsNvsKeySuffix, (uint8_t)uart->GetStopBits());
      configurationNvs.Write (keyBase + configurationUartFlowControlNvsKeySuffix, (uint8_t)uart->GetFlowControl());
    }

    if (auto networkInterface = dynamic_cast<NetworkInterface*>(hardwareInterfaces[i].get())) {
      configurationNvs.Write (keyBase + configurationNetworkInterfaceIpV4DhcpClientEnabledNvsKeySuffix, (uint8_t)networkInterface->IsIpV4DhcpClientEnabled());
      configurationNvs.Write (keyBase + configurationNetworkInterfaceIpV4AddressNvsKeySuffix, networkInterface->GetIpV4Address().u32);
      configurationNvs.Write (keyBase + configurationNetworkInterfaceIpV4NetmaskNvsKeySuffix, networkInterface->GetIpV4Netmask().u32);
      configurationNvs.Write (keyBase + configurationNetworkInterfaceIpV4GatewayNvsKeySuffix, networkInterface->GetIpV4Gateway().u32);
      configurationNvs.Write (keyBase + configurationNetworkInterfaceIpV6DhcpClientEnabledNvsKeySuffix, (uint8_t)networkInterface->IsIpV6DhcpClientEnabled());
      uint32_t ipV6Address[4];
      memcpy (ipV6Address, networkInterface->GetIpV6GlobalAddress().u32, sizeof (ipV6Address));
      configurationNvs.Write (keyBase + configurationNetworkInterfaceIpV6GlobalAddressNvsKeySuffix, ipV6Address, sizeof (ipV6Address));
    }

    if (auto wifiStation = dynamic_cast<WiFiStation*>(hardwareInterfaces[i].get())) {
      configurationNvs.Write (keyBase + configurationWiFiSsidNvsKeySuffix, wifiStation->GetSsid());
      configurationNvs.Write (keyBase + configurationWiFiPasswordNvsKeySuffix, wifiStation->GetPassword());
    }

    configurationNvs.Write (keyBase + configurationHardwareInterfaceEnabledNvsKeySuffix, (uint8_t)hardwareInterfaces[i]->IsEnabled());
  }

  auto servers = GetServers();
  for (size_t i = 0; i < servers.size(); i++) {
    std::string keyBase = configurationServerNvsKeyPrefix + std::to_string(i);

    if (auto networkServer = dynamic_cast<NetworkServer*>(servers[i].get())) {
      configurationNvs.Write (keyBase + configurationNetworkServerPortNvsKeySuffix, networkServer->GetPort());
      configurationNvs.Write (keyBase + configurationNetworkServerMaxNumberOfClientsNvsKeySuffix, (uint16_t)networkServer->GetMaxNumberOfClients());
    }

    if (auto modbusServer = dynamic_cast<ModbusServer*>(servers[i].get())) {\
      configurationNvs.Write (keyBase + configurationModbusServerProtocolNvsKeySuffix, (uint8_t)modbusServer->GetProtocol());
      configurationNvs.Write (keyBase + configurationModbusServerStationAddressNvsKeySuffix, modbusServer->GetStationAddress());
      if (auto baseServer = modbusServer->GetBaseServer().lock()) {
        if (auto networkServer = dynamic_cast<NetworkServer*>(baseServer.get())) {
          configurationNvs.Write (keyBase + configurationNetworkServerPortNvsKeySuffix, networkServer->GetPort());
          configurationNvs.Write (keyBase + configurationNetworkServerMaxNumberOfClientsNvsKeySuffix, (uint16_t)networkServer->GetMaxNumberOfClients());     
        }
      }
    }

    configurationNvs.Write (keyBase + configurationServerEnabledNvsKeySuffix, (uint8_t)servers[i]->IsEnabled());
  }
}

//==============================================================================

void BlackBox::EraseConfiguration() {
  NvsNamespace configurationNvs (configurationNvsNamespace, NvsAccessMode::readWrite);
  configurationNvs.Erase();
}

//==============================================================================

std::string BlackBox::GetName() {
  LockGuard lg (*this);
  return name;
}

//==============================================================================

BlackBoxHardwareInfo BlackBox::GetHardwareInfo() {
  LockGuard lg (*this);
  if (hardwareInfoLoaded)
    return hardwareInfo;
    
  NvsNamespace hardwareInfoNvs (hardwareInfoNvsNamespace, NvsAccessMode::readOnly);
  uint16_t u16Value;
  std::string stringValue;
  
  if (hardwareInfoNvs.Read (hardwareInfoNameNvsKey, stringValue) == ESP_OK)
    hardwareInfo.name = stringValue;
  if (hardwareInfoNvs.Read (hardwareInfoMajorVersionNvsKey, u16Value) == ESP_OK)
    hardwareInfo.version.major = u16Value;
  if (hardwareInfoNvs.Read (hardwareInfoMinorVersionNvsKey, u16Value) == ESP_OK)
    hardwareInfo.version.minor = u16Value;
  if (hardwareInfoNvs.Read (hardwareInfoPatchVersionNvsKey, u16Value) == ESP_OK)
    hardwareInfo.version.patch = u16Value;
  if (hardwareInfoNvs.Read (hardwareInfoUidNvsKey, stringValue) == ESP_OK)
    hardwareInfo.uid = stringValue;
  
  hardwareInfoLoaded = true;
  return hardwareInfo;
}

//==============================================================================

void BlackBox::SetName (const std::string& name) {
  LockGuard lg (*this);
  this->name = name;
}

//==============================================================================

std::vector<std::shared_ptr<HardwareInterface>> BlackBox::GetHardwareInterfaces(){
  LockGuard lg (*this);
  return hardwareInterfaces;
}

//==============================================================================

void BlackBox::AddHardwareInterface (std::shared_ptr<HardwareInterface> hardwareInterface){
  LockGuard lg (*this);
  hardwareInterfaces.push_back (hardwareInterface);
}

//==============================================================================

std::vector<std::shared_ptr<Server>> BlackBox::GetServers() {
  LockGuard lg (*this);
  return servers;
}

//==============================================================================

void BlackBox::AddServer (std::shared_ptr<Server> server){
  LockGuard lg (*this);
  servers.push_back (server);
}

//==============================================================================

bool BlackBox::GetRestartedFlag() {
  LockGuard lg (*this);
  return restartedFlag;
}

//==============================================================================

void BlackBox::ClearRestartedFlag() {
  LockGuard lg (*this);
  restartedFlag = false;
}

//==============================================================================

}