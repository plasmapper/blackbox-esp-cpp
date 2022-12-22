#include "pl_blackbox_modbus_server.h"
#include "esp_check.h"

//==============================================================================

namespace PL {

//==============================================================================

const std::string BlackBoxModbusServer::plbbSignature = "PLBB";

//==============================================================================

BlackBoxModbusServer::BlackBoxModbusServer (std::shared_ptr<BlackBox> blackBox, std::shared_ptr<Uart> uart, ModbusProtocol protocol, uint8_t stationAddress, std::shared_ptr<Buffer> buffer) :
    ModbusServer (uart, protocol, stationAddress, buffer), blackBox (blackBox) {
  AddMemoryAreas();
}

//==============================================================================

BlackBoxModbusServer::BlackBoxModbusServer (std::shared_ptr<BlackBox> blackBox, std::shared_ptr<Uart> uart, ModbusProtocol protocol, uint8_t stationAddress, size_t bufferSize) :
    ModbusServer (uart, protocol, stationAddress, bufferSize), blackBox (blackBox) {
  AddMemoryAreas();
}

//==============================================================================

BlackBoxModbusServer::BlackBoxModbusServer (std::shared_ptr<BlackBox> blackBox, uint16_t port, std::shared_ptr<Buffer> buffer) :
    ModbusServer (port, buffer), blackBox (blackBox) {
  AddMemoryAreas();
}

//==============================================================================

BlackBoxModbusServer::BlackBoxModbusServer (std::shared_ptr<BlackBox> blackBox, uint16_t port, size_t bufferSize) :
    ModbusServer (port, bufferSize), blackBox (blackBox) {
  AddMemoryAreas();
}

//==============================================================================

void BlackBoxModbusServer::AddMemoryAreas() {
  memoryDataBuffer = std::make_shared<PL::TypedBuffer<MemoryData>>(&memoryData);

  AddMemoryArea (std::make_shared<GeneralConfigurationHR>(*this, PL::ModbusMemoryType::holdingRegisters, registerMemoryAreaSize));
  AddMemoryArea (std::make_shared<GeneralConfigurationHR>(*this, PL::ModbusMemoryType::coils, coilMemoryAreaSize));
  AddMemoryArea (std::make_shared<GeneralConfigurationIR>(*this));

  AddMemoryArea (std::make_shared<HardwareInterfaceConfigurationHR>(*this, PL::ModbusMemoryType::holdingRegisters, registerMemoryAreaSize));
  AddMemoryArea (std::make_shared<HardwareInterfaceConfigurationHR>(*this, PL::ModbusMemoryType::coils, coilMemoryAreaSize));
  AddMemoryArea (std::make_shared<HardwareInterfaceConfigurationIR>(*this));

  AddMemoryArea (std::make_shared<ServerConfigurationHR>(*this, PL::ModbusMemoryType::holdingRegisters, registerMemoryAreaSize));
  AddMemoryArea (std::make_shared<ServerConfigurationHR>(*this, PL::ModbusMemoryType::coils, coilMemoryAreaSize));
  AddMemoryArea (std::make_shared<ServerConfigurationIR>(*this));
}

//==============================================================================

BlackBoxModbusServer::GeneralConfigurationHR::GeneralConfigurationHR (BlackBoxModbusServer& modbusServer, ModbusMemoryType memoryType, size_t size) :
  ModbusMemoryArea (memoryType, generalConfigurationMemoryAddress, modbusServer.memoryDataBuffer->data, size, modbusServer.memoryDataBuffer),
  modbusServer (modbusServer) {}

//==============================================================================

esp_err_t BlackBoxModbusServer::GeneralConfigurationHR::OnRead() {
  BlackBox& blackBox = *modbusServer.blackBox;
  
  memset (modbusServer.memoryDataBuffer->data, 0, registerMemoryAreaSize);
  auto& hr = modbusServer.memoryDataBuffer->data->generalConfigurationHR;

  size_t maxNameSize = BlackBoxModbusServer::maxNameSize;
  auto name = blackBox.GetName();
  memcpy (hr.name, name.data(), std::min (maxNameSize, name.size()));
  hr.selectedHardwareInterfaceIndex = modbusServer.selectedHardwareInterfaceIndex;
  hr.selectedServerIndex = modbusServer.selectedServerIndex;
  return ESP_OK;
}

//==============================================================================

esp_err_t BlackBoxModbusServer::GeneralConfigurationHR::OnWrite() {
  BlackBox& blackBox = *modbusServer.blackBox;

  auto& hr = modbusServer.memoryDataBuffer->data->generalConfigurationHR;

  if (hr.restart)
    blackBox.Restart();
  if (hr.saveConfiguration)
    blackBox.SaveConfiguration();
  if (hr.clearRestartedFlag)
    blackBox.ClearRestartedFlag();
  std::string name (hr.name, maxNameSize);
  blackBox.SetName (name.c_str());
  size_t numberOfHardwareInterfaces = blackBox.GetHardwareInterfaces().size();
  if (numberOfHardwareInterfaces)
    modbusServer.selectedHardwareInterfaceIndex = std::min (hr.selectedHardwareInterfaceIndex, (uint16_t)(numberOfHardwareInterfaces - 1));
  size_t numberOfServers = blackBox.GetServers().size();
  if (numberOfServers)
    modbusServer.selectedServerIndex = std::min (hr.selectedServerIndex, (uint16_t)(numberOfServers - 1));
  return ESP_OK;
}

//==============================================================================

BlackBoxModbusServer::GeneralConfigurationIR::GeneralConfigurationIR (BlackBoxModbusServer& modbusServer) :
  ModbusMemoryArea (PL::ModbusMemoryType::inputRegisters, generalConfigurationMemoryAddress, modbusServer.memoryDataBuffer->data, registerMemoryAreaSize, modbusServer.memoryDataBuffer),
  modbusServer (modbusServer) {}

//==============================================================================

esp_err_t BlackBoxModbusServer::GeneralConfigurationIR::OnRead() {
  BlackBox& blackBox = *modbusServer.blackBox;
  
  memset (modbusServer.memoryDataBuffer->data, 0, registerMemoryAreaSize);
  auto& ir = modbusServer.memoryDataBuffer->data->generalConfigurationIR;

  size_t maxNameSize = BlackBoxModbusServer::maxNameSize;

  ir.restartedFlag = blackBox.GetRestartedFlag();
  memcpy (ir.plbbSignature, plbbSignature.data(), sizeof (ir.plbbSignature));
  ir.memoryMapVersion = memoryMapVersion;
  auto hardwareInfo = blackBox.GetHardwareInfo();
  memcpy (ir.hardwareInfo.name, hardwareInfo.name.data(), std::min (maxNameSize, hardwareInfo.name.size()));
  ir.hardwareInfo.version.major = hardwareInfo.version.major;
  ir.hardwareInfo.version.minor = hardwareInfo.version.minor;
  ir.hardwareInfo.version.patch = hardwareInfo.version.patch;
  memcpy (ir.hardwareInfo.uid, hardwareInfo.uid.data(), std::min (maxNameSize, hardwareInfo.uid.size()));
  auto firmwareInfo = blackBox.GetFirmwareInfo();
  memcpy (ir.firmwareInfo.name, firmwareInfo.name.data(), std::min (maxNameSize, firmwareInfo.name.size()));
  ir.firmwareInfo.version.major = firmwareInfo.version.major;
  ir.firmwareInfo.version.minor = firmwareInfo.version.minor;
  ir.firmwareInfo.version.patch = firmwareInfo.version.patch;
  ir.numberOfHardwareInterfaces = blackBox.GetHardwareInterfaces().size();
  ir.numberOfServers = blackBox.GetServers().size();
  return ESP_OK;
}
//==============================================================================

BlackBoxModbusServer::HardwareInterfaceConfigurationHR::HardwareInterfaceConfigurationHR (BlackBoxModbusServer& modbusServer, ModbusMemoryType memoryType, size_t size) :
  ModbusMemoryArea (memoryType, hardwareInterfaceConfigurationMemoryAddress, modbusServer.memoryDataBuffer->data, size, modbusServer.memoryDataBuffer),
  modbusServer (modbusServer) {}

//==============================================================================

esp_err_t BlackBoxModbusServer::HardwareInterfaceConfigurationHR::OnRead() {
  BlackBox& blackBox = *modbusServer.blackBox;
  
  memset (modbusServer.memoryDataBuffer->data, 0, registerMemoryAreaSize);
  auto& hr = modbusServer.memoryDataBuffer->data->hardwareInterfaceConfigurationHR;

  auto hardwareInterface = blackBox.GetHardwareInterfaces()[modbusServer.selectedHardwareInterfaceIndex];
  if (hardwareInterface->Lock(5) != ESP_OK)
    return ESP_ERR_INVALID_STATE;

  size_t maxNameSize = BlackBoxModbusServer::maxNameSize;

  hr.common.enabled = hardwareInterface->IsEnabled();

  if (auto uart = dynamic_cast<PL::Uart*>(hardwareInterface.get())) {
    hr.uart.baudRate = uart->GetBaudRate();
    hr.uart.dataBits = uart->GetDataBits();
    hr.uart.parity = (uint16_t)uart->GetParity();
    hr.uart.stopBits = (uint16_t)uart->GetStopBits();
    hr.uart.flowControl = (uint16_t)uart->GetFlowControl();
  }

  if (auto networkInterface = dynamic_cast<PL::NetworkInterface*>(hardwareInterface.get())) {
    hr.networkInterface.ipV4DhcpClientEnabled = networkInterface->IsIpV4DhcpClientEnabled();
    hr.networkInterface.ipV6DhcpClientEnabled = networkInterface->IsIpV6DhcpClientEnabled();
    hr.networkInterface.ipV4Address = networkInterface->GetIpV4Address().u32;
    hr.networkInterface.ipV4Netmask = networkInterface->GetIpV4Netmask().u32;
    hr.networkInterface.ipV4Gateway = networkInterface->GetIpV4Gateway().u32;
    memcpy (hr.networkInterface.ipV6GlobalAddress, networkInterface->GetIpV6GlobalAddress().u32, sizeof (hr.networkInterface.ipV6GlobalAddress));
  }

  if (auto wifiStation = dynamic_cast<PL::WiFiStation*>(hardwareInterface.get())) {
    auto ssid = wifiStation->GetSsid();
    memcpy (hr.wifi.ssid, ssid.data(), std::min (maxNameSize, ssid.size()));
    auto password = wifiStation->GetPassword();
    memcpy (hr.wifi.password, password.data(), std::min (maxNameSize, password.size()));
  }

  hardwareInterface->Unlock();
  return ESP_OK;
}

//==============================================================================

esp_err_t BlackBoxModbusServer::HardwareInterfaceConfigurationHR::OnWrite() {
  BlackBox& blackBox = *modbusServer.blackBox;

  auto& hr = modbusServer.memoryDataBuffer->data->hardwareInterfaceConfigurationHR;

  auto hardwareInterface = blackBox.GetHardwareInterfaces()[modbusServer.selectedHardwareInterfaceIndex];
  if (hardwareInterface->Lock(5) != ESP_OK)
    return ESP_ERR_INVALID_STATE;

  hardwareInterface->Disable();

  if (auto uart = dynamic_cast<PL::Uart*>(hardwareInterface.get())) {
    uart->SetBaudRate (hr.uart.baudRate);
    uart->SetDataBits (hr.uart.dataBits);
    uart->SetParity ((UartParity)hr.uart.parity);
    uart->SetStopBits ((UartStopBits)hr.uart.stopBits);
    uart->SetFlowControl ((UartFlowControl)hr.uart.flowControl);
  }

  if (auto networkInterface = dynamic_cast<PL::NetworkInterface*>(hardwareInterface.get())) {
    if (hr.networkInterface.ipV4DhcpClientEnabled) {
      networkInterface->EnableIpV4DhcpClient();
    }
    else {
      networkInterface->DisableIpV4DhcpClient();
      networkInterface->SetIpV4Address (hr.networkInterface.ipV4Address);
      networkInterface->SetIpV4Netmask (hr.networkInterface.ipV4Netmask);
      networkInterface->SetIpV4Gateway (hr.networkInterface.ipV4Gateway);
    }

    if (hr.networkInterface.ipV6DhcpClientEnabled) {
      networkInterface->EnableIpV6DhcpClient();
    }
    else {
      networkInterface->DisableIpV6DhcpClient();
      networkInterface->SetIpV6GlobalAddress (IpV6Address (hr.networkInterface.ipV6GlobalAddress[0], hr.networkInterface.ipV6GlobalAddress[1],
                                                           hr.networkInterface.ipV6GlobalAddress[2], hr.networkInterface.ipV6GlobalAddress[3]));
    }
  }

  if (auto wifiStation = dynamic_cast<PL::WiFiStation*>(hardwareInterface.get())) {
    std::string ssid (hr.wifi.ssid, maxNameSize);
    wifiStation->SetSsid (ssid.c_str());
    std::string password (hr.wifi.password, maxNameSize);
    wifiStation->SetPassword (password.c_str());
  }

  if (hr.common.enabled)
    hardwareInterface->Enable();

  hardwareInterface->Unlock();
  return ESP_OK;
}

//==============================================================================

BlackBoxModbusServer::HardwareInterfaceConfigurationIR::HardwareInterfaceConfigurationIR (BlackBoxModbusServer& modbusServer) :
  ModbusMemoryArea (PL::ModbusMemoryType::inputRegisters, hardwareInterfaceConfigurationMemoryAddress, modbusServer.memoryDataBuffer->data, registerMemoryAreaSize, modbusServer.memoryDataBuffer),
  modbusServer (modbusServer) {}

//==============================================================================

esp_err_t BlackBoxModbusServer::HardwareInterfaceConfigurationIR::OnRead() {
  BlackBox& blackBox = *modbusServer.blackBox;
  
  memset (modbusServer.memoryDataBuffer->data, 0, registerMemoryAreaSize);
  auto& ir = modbusServer.memoryDataBuffer->data->hardwareInterfaceConfigurationIR;

  auto hardwareInterface = blackBox.GetHardwareInterfaces()[modbusServer.selectedHardwareInterfaceIndex];
  if (hardwareInterface->Lock(5) != ESP_OK)
    return ESP_ERR_INVALID_STATE;

  size_t maxNameSize = BlackBoxModbusServer::maxNameSize;

  auto name = hardwareInterface->GetName();
  memcpy (ir.common.name, name.data(), std::min (maxNameSize, name.size()));

  if (dynamic_cast<PL::Uart*>(hardwareInterface.get()))
    ir.common.type = (uint16_t)BlackBoxHardwareInterfaceType::uart;

  if (auto networkInterface = dynamic_cast<PL::NetworkInterface*>(hardwareInterface.get())) {
    ir.common.type = (uint16_t)BlackBoxHardwareInterfaceType::networkInterface;
    ir.networkInterface.connected = networkInterface->IsConnected();
    memcpy (ir.networkInterface.ipV6LinkLocalAddress, networkInterface->GetIpV6LinkLocalAddress().u32, sizeof (ir.networkInterface.ipV6LinkLocalAddress));
  }

  if (dynamic_cast<PL::Ethernet*>(hardwareInterface.get()))
    ir.common.type = (uint16_t)BlackBoxHardwareInterfaceType::ethernet;

  if (dynamic_cast<PL::WiFiStation*>(hardwareInterface.get()))
    ir.common.type = (uint16_t)BlackBoxHardwareInterfaceType::wifiStation;

  hardwareInterface->Unlock();
  return ESP_OK;
}

//==============================================================================

BlackBoxModbusServer::ServerConfigurationHR::ServerConfigurationHR (BlackBoxModbusServer& modbusServer, ModbusMemoryType memoryType, size_t size) :
  ModbusMemoryArea (memoryType, serverConfigurationMemoryAddress, modbusServer.memoryDataBuffer->data, size, modbusServer.memoryDataBuffer),
  modbusServer (modbusServer) {}

//==============================================================================

esp_err_t BlackBoxModbusServer::ServerConfigurationHR::OnRead() {
  BlackBox& blackBox = *modbusServer.blackBox;
  
  memset (modbusServer.memoryDataBuffer->data, 0, registerMemoryAreaSize);
  auto& hr = modbusServer.memoryDataBuffer->data->serverConfigurationHR;

  auto server = blackBox.GetServers()[modbusServer.selectedServerIndex];
  if (server->Lock(5) != ESP_OK)
    return ESP_ERR_INVALID_STATE;

  hr.common.enabled = server->IsEnabled();

  if (auto networkServer = dynamic_cast<PL::NetworkServer*>(server.get())) {
    hr.networkServer.port = networkServer->GetPort();
    hr.networkServer.maxNumberOfClients = networkServer->GetMaxNumberOfClients();
  }

  if (auto modbusServer = dynamic_cast<PL::ModbusServer*>(server.get())) {
    hr.modbusServer.protocol = (uint16_t)modbusServer->GetProtocol();
    hr.modbusServer.stationAddress = modbusServer->GetStationAddress();
    if (auto baseServer = modbusServer->GetBaseServer().lock()) {
      if (auto networkServer = dynamic_cast<PL::NetworkServer*>(baseServer.get())) {
        hr.modbusNetworkServer.port = networkServer->GetPort();
        hr.modbusNetworkServer.maxNumberOfClients = networkServer->GetMaxNumberOfClients();
      }
    }
  }

  server->Unlock();
  return ESP_OK;
}

//==============================================================================

esp_err_t BlackBoxModbusServer::ServerConfigurationHR::OnWrite() {
  BlackBox& blackBox = *modbusServer.blackBox;

  auto& hr = modbusServer.memoryDataBuffer->data->serverConfigurationHR;

  auto server = blackBox.GetServers()[modbusServer.selectedServerIndex];
  if (server->Lock(5) != ESP_OK)
    return ESP_ERR_INVALID_STATE;

  server->Disable();

  if (auto networkServer = dynamic_cast<PL::NetworkServer*>(server.get())) {
    networkServer->SetPort (hr.networkServer.port);
    networkServer->SetMaxNumberOfClients (hr.networkServer.maxNumberOfClients);
  }

  if (auto modbusServer = dynamic_cast<PL::ModbusServer*>(server.get())) {
    modbusServer->SetProtocol ((ModbusProtocol)hr.modbusServer.protocol);
    modbusServer->SetStationAddress (std::min (hr.modbusServer.stationAddress, (uint16_t)255));
    if (auto baseServer = modbusServer->GetBaseServer().lock()) {
      if (auto networkServer = dynamic_cast<PL::NetworkServer*>(baseServer.get())) {
        networkServer->SetPort (hr.modbusNetworkServer.port);
        networkServer->SetMaxNumberOfClients (hr.modbusNetworkServer.maxNumberOfClients);
      }
    }
  }

  if (hr.common.enabled)
    server->Enable();

  server->Unlock();
  return ESP_OK;
}

//==============================================================================

BlackBoxModbusServer::ServerConfigurationIR::ServerConfigurationIR (BlackBoxModbusServer& modbusServer) :
  ModbusMemoryArea (PL::ModbusMemoryType::inputRegisters, serverConfigurationMemoryAddress, modbusServer.memoryDataBuffer->data, registerMemoryAreaSize, modbusServer.memoryDataBuffer),
  modbusServer (modbusServer) {}

//==============================================================================

esp_err_t BlackBoxModbusServer::ServerConfigurationIR::OnRead() {
  BlackBox& blackBox = *modbusServer.blackBox;
  
  memset (modbusServer.memoryDataBuffer->data, 0, registerMemoryAreaSize);
  auto& ir = modbusServer.memoryDataBuffer->data->serverConfigurationIR;

  auto server = blackBox.GetServers()[modbusServer.selectedServerIndex];
  if (server->Lock(5) != ESP_OK)
    return ESP_ERR_INVALID_STATE;

  size_t maxNameSize = BlackBoxModbusServer::maxNameSize;

  auto name = server->GetName();
  memcpy (ir.common.name, name.data(), std::min (maxNameSize, name.size()));

  if (dynamic_cast<PL::UartServer*>(server.get()))
    ir.common.type = (uint16_t)BlackBoxServerType::uartServer;

  if (dynamic_cast<PL::NetworkServer*>(server.get()))
    ir.common.type = (uint16_t)BlackBoxServerType::networkServer;

  if (auto modbusServer = dynamic_cast<PL::ModbusServer*>(server.get())) {
    if (auto baseServer = modbusServer->GetBaseServer().lock()) {
      if (dynamic_cast<PL::NetworkServer*>(baseServer.get()))
        ir.common.type = (uint16_t)BlackBoxServerType::modbusTcpServer;
      else
        ir.common.type = (uint16_t)BlackBoxServerType::modbusSerialServer;
    }
  }

  server->Unlock();
  return ESP_OK;
}

//==============================================================================

}