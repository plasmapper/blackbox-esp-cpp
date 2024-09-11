#include "pl_blackbox_modbus_server.h"
#include "esp_check.h"

//==============================================================================

namespace PL {

//==============================================================================

const std::string BlackBoxModbusServer::plbbSignature = "PLBB";

//==============================================================================

BlackBoxModbusServer::BlackBoxModbusServer(std::shared_ptr<BlackBox> blackBox, std::shared_ptr<Stream> stream, ModbusProtocol protocol, uint8_t stationAddress, std::shared_ptr<Buffer> buffer) :
    ModbusServer(stream, protocol, stationAddress, buffer), blackBox(blackBox) {
  AddMemoryAreas();
}

//==============================================================================

BlackBoxModbusServer::BlackBoxModbusServer(std::shared_ptr<BlackBox> blackBox, std::shared_ptr<Stream> stream, ModbusProtocol protocol, uint8_t stationAddress, size_t bufferSize) :
    ModbusServer(stream, protocol, stationAddress, bufferSize), blackBox(blackBox) {
  AddMemoryAreas();
}

//==============================================================================

BlackBoxModbusServer::BlackBoxModbusServer(std::shared_ptr<BlackBox> blackBox, uint16_t port, std::shared_ptr<Buffer> buffer) :
    ModbusServer(port, buffer), blackBox(blackBox) {
  AddMemoryAreas();
}

//==============================================================================

BlackBoxModbusServer::BlackBoxModbusServer(std::shared_ptr<BlackBox> blackBox, uint16_t port, size_t bufferSize) :
    ModbusServer(port, bufferSize), blackBox(blackBox) {
  AddMemoryAreas();
}

//==============================================================================

void BlackBoxModbusServer::AddMemoryAreas() {
  memoryDataBuffer = std::make_shared<PL::TypedBuffer<MemoryData>>(&memoryData);

  AddMemoryArea(std::make_shared<GeneralConfigurationHR>(*this, PL::ModbusMemoryType::holdingRegisters, registerMemoryAreaSize));
  AddMemoryArea(std::make_shared<GeneralConfigurationHR>(*this, PL::ModbusMemoryType::coils, coilMemoryAreaSize));
  AddMemoryArea(std::make_shared<GeneralConfigurationIR>(*this));

  AddMemoryArea(std::make_shared<HardwareInterfaceConfigurationHR>(*this, PL::ModbusMemoryType::holdingRegisters, registerMemoryAreaSize));
  AddMemoryArea(std::make_shared<HardwareInterfaceConfigurationHR>(*this, PL::ModbusMemoryType::coils, coilMemoryAreaSize));
  AddMemoryArea(std::make_shared<HardwareInterfaceConfigurationIR>(*this));

  AddMemoryArea(std::make_shared<ServerConfigurationHR>(*this, PL::ModbusMemoryType::holdingRegisters, registerMemoryAreaSize));
  AddMemoryArea(std::make_shared<ServerConfigurationHR>(*this, PL::ModbusMemoryType::coils, coilMemoryAreaSize));
  AddMemoryArea(std::make_shared<ServerConfigurationIR>(*this));
}

//==============================================================================

BlackBoxModbusServer::GeneralConfigurationHR::GeneralConfigurationHR(BlackBoxModbusServer& modbusServer, ModbusMemoryType memoryType, size_t size) :
  ModbusMemoryArea(memoryType, generalConfigurationMemoryAddress, modbusServer.memoryDataBuffer->data, size, modbusServer.memoryDataBuffer),
  modbusServer(modbusServer) {}

//==============================================================================

esp_err_t BlackBoxModbusServer::GeneralConfigurationHR::OnRead() {
  BlackBox& blackBox = *modbusServer.blackBox;
  
  memset(modbusServer.memoryDataBuffer->data, 0, registerMemoryAreaSize);
  auto& hr = modbusServer.memoryDataBuffer->data->generalConfigurationHR;

  auto name = blackBox.GetDeviceName();
  memcpy(hr.name, name.data(), std::min(maxNameSize, name.size()));
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
    blackBox.SaveAllConfigurations();
  if (hr.clearRestartedFlag)
    blackBox.ClearRestartedFlag();
  std::string name(hr.name, maxNameSize);
  blackBox.SetDeviceName(name.c_str());
  size_t numberOfHardwareInterfaces = blackBox.GetHardwareInterfaceConfigurations().size();
  if (numberOfHardwareInterfaces)
    modbusServer.selectedHardwareInterfaceIndex = std::min(hr.selectedHardwareInterfaceIndex, (uint16_t)(numberOfHardwareInterfaces - 1));
  size_t numberOfServers = blackBox.GetServerConfigurations().size();
  if (numberOfServers)
    modbusServer.selectedServerIndex = std::min(hr.selectedServerIndex, (uint16_t)(numberOfServers - 1));
  return ESP_OK;
}

//==============================================================================

BlackBoxModbusServer::GeneralConfigurationIR::GeneralConfigurationIR(BlackBoxModbusServer& modbusServer) :
  ModbusMemoryArea(PL::ModbusMemoryType::inputRegisters, generalConfigurationMemoryAddress, modbusServer.memoryDataBuffer->data, registerMemoryAreaSize, modbusServer.memoryDataBuffer),
  modbusServer(modbusServer) {}

//==============================================================================

esp_err_t BlackBoxModbusServer::GeneralConfigurationIR::OnRead() {
  BlackBox& blackBox = *modbusServer.blackBox;
  
  memset(modbusServer.memoryDataBuffer->data, 0, registerMemoryAreaSize);
  auto& ir = modbusServer.memoryDataBuffer->data->generalConfigurationIR;

  ir.restartedFlag = blackBox.GetRestartedFlag();
  memcpy(ir.plbbSignature, plbbSignature.data(), sizeof(ir.plbbSignature));
  ir.memoryMapVersion = memoryMapVersion;
  auto hardwareInfo = blackBox.GetHardwareInfo();
  memcpy(ir.hardwareInfo.name, hardwareInfo.name.data(), std::min(maxNameSize, hardwareInfo.name.size()));
  ir.hardwareInfo.version.major = hardwareInfo.version.major;
  ir.hardwareInfo.version.minor = hardwareInfo.version.minor;
  ir.hardwareInfo.version.patch = hardwareInfo.version.patch;
  memcpy(ir.hardwareInfo.uid, hardwareInfo.uid.data(), std::min(maxNameSize, hardwareInfo.uid.size()));
  auto firmwareInfo = blackBox.GetFirmwareInfo();
  memcpy(ir.firmwareInfo.name, firmwareInfo.name.data(), std::min(maxNameSize, firmwareInfo.name.size()));
  ir.firmwareInfo.version.major = firmwareInfo.version.major;
  ir.firmwareInfo.version.minor = firmwareInfo.version.minor;
  ir.firmwareInfo.version.patch = firmwareInfo.version.patch;
  ir.numberOfHardwareInterfaces = blackBox.GetHardwareInterfaceConfigurations().size();
  ir.numberOfServers = blackBox.GetServerConfigurations().size();
  return ESP_OK;
}
//==============================================================================

BlackBoxModbusServer::HardwareInterfaceConfigurationHR::HardwareInterfaceConfigurationHR(BlackBoxModbusServer& modbusServer, ModbusMemoryType memoryType, size_t size) :
  ModbusMemoryArea(memoryType, hardwareInterfaceConfigurationMemoryAddress, modbusServer.memoryDataBuffer->data, size, modbusServer.memoryDataBuffer),
  modbusServer(modbusServer) {}

//==============================================================================

esp_err_t BlackBoxModbusServer::HardwareInterfaceConfigurationHR::OnRead() {
  BlackBox& blackBox = *modbusServer.blackBox;
  
  memset(modbusServer.memoryDataBuffer->data, 0, registerMemoryAreaSize);
  auto& hr = modbusServer.memoryDataBuffer->data->hardwareInterfaceConfigurationHR;

  size_t numberOfHardwareInterfaces = blackBox.GetHardwareInterfaceConfigurations().size();
  if (!numberOfHardwareInterfaces || modbusServer.selectedHardwareInterfaceIndex >= numberOfHardwareInterfaces)
    return ESP_OK;

  auto hardwareInterfaceConfiguration = blackBox.GetHardwareInterfaceConfigurations()[modbusServer.selectedHardwareInterfaceIndex];

  hr.common.enabled = hardwareInterfaceConfiguration->enabled.GetValue();

  if (auto uartConfiguration = dynamic_cast<PL::BlackBoxUartConfiguration*>(hardwareInterfaceConfiguration.get())) {
    hr.uart.baudRate = uartConfiguration->baudRate.GetValue();
    hr.uart.dataBits = uartConfiguration->dataBits.GetValue();
    hr.uart.parity = (uint16_t)uartConfiguration->parity.GetValue();
    hr.uart.stopBits = (uint16_t)uartConfiguration->stopBits.GetValue();
    hr.uart.flowControl = (uint16_t)uartConfiguration->flowControl.GetValue();
  }

  if (auto networkInterfaceConfiguration = dynamic_cast<PL::BlackBoxNetworkInterfaceConfiguration*>(hardwareInterfaceConfiguration.get())) {
    hr.networkInterface.ipV4DhcpClientEnabled = networkInterfaceConfiguration->ipV4DhcpClientEnabled.GetValue();
    hr.networkInterface.ipV6DhcpClientEnabled = networkInterfaceConfiguration->ipV6DhcpClientEnabled.GetValue();

    if (auto networkInterface = dynamic_cast<PL::NetworkInterface*>(networkInterfaceConfiguration->GetHardwareInterface().get())) {
      if (hr.networkInterface.ipV4DhcpClientEnabled) {
        hr.networkInterface.ipV4Address = networkInterface->GetIpV4Address().u32;
        hr.networkInterface.ipV4Netmask = networkInterface->GetIpV4Netmask().u32;
        hr.networkInterface.ipV4Gateway = networkInterface->GetIpV4Gateway().u32;
      }
      else {
        hr.networkInterface.ipV4Address = networkInterfaceConfiguration->ipV4Address.GetValue().u32;
        hr.networkInterface.ipV4Netmask = networkInterfaceConfiguration->ipV4Netmask.GetValue().u32;
        hr.networkInterface.ipV4Gateway = networkInterfaceConfiguration->ipV4Gateway.GetValue().u32;
      }
      if (hr.networkInterface.ipV6DhcpClientEnabled)
        memcpy(hr.networkInterface.ipV6GlobalAddress, networkInterface->GetIpV6GlobalAddress().u32, sizeof(hr.networkInterface.ipV6GlobalAddress));
      else
        memcpy(hr.networkInterface.ipV6GlobalAddress, networkInterfaceConfiguration->ipV6GlobalAddress.GetValue().u32, sizeof(hr.networkInterface.ipV6GlobalAddress));
    }
  }

  if (auto wifiStationConfiguration = dynamic_cast<PL::BlackBoxWiFiStationConfiguration*>(hardwareInterfaceConfiguration.get())) {
    auto ssid = wifiStationConfiguration->ssid.GetValue();
    memcpy(hr.wifi.ssid, ssid.data(), std::min(maxWiFiSsidSize, ssid.size()));
    memset(hr.wifi.password, 255, sizeof(hr.wifi.password));
  }

  return ESP_OK;
}

//==============================================================================

esp_err_t BlackBoxModbusServer::HardwareInterfaceConfigurationHR::OnWrite() {
  BlackBox& blackBox = *modbusServer.blackBox;

  auto& hr = modbusServer.memoryDataBuffer->data->hardwareInterfaceConfigurationHR;

  size_t numberOfHardwareInterfaces = blackBox.GetHardwareInterfaceConfigurations().size();
  if (!numberOfHardwareInterfaces || modbusServer.selectedHardwareInterfaceIndex >= numberOfHardwareInterfaces)
    return ESP_OK;

  auto hardwareInterfaceConfiguration = blackBox.GetHardwareInterfaceConfigurations()[modbusServer.selectedHardwareInterfaceIndex];

  hardwareInterfaceConfiguration->enabled.SetValue(hr.common.enabled);

  if (auto uartConfiguration = dynamic_cast<PL::BlackBoxUartConfiguration*>(hardwareInterfaceConfiguration.get())) {
    uartConfiguration->baudRate.SetValue(hr.uart.baudRate);
    uartConfiguration->dataBits.SetValue(hr.uart.dataBits);
    uartConfiguration->parity.SetValue((UartParity)hr.uart.parity);
    uartConfiguration->stopBits.SetValue((UartStopBits)hr.uart.stopBits);
    uartConfiguration->flowControl.SetValue((UartFlowControl)hr.uart.flowControl);
  }

  if (auto networkInterfaceConfiguration = dynamic_cast<PL::BlackBoxNetworkInterfaceConfiguration*>(hardwareInterfaceConfiguration.get())) {
    networkInterfaceConfiguration->ipV4DhcpClientEnabled.SetValue(hr.networkInterface.ipV4DhcpClientEnabled);
    if (!hr.networkInterface.ipV4DhcpClientEnabled) {
      networkInterfaceConfiguration->ipV4Address.SetValue(hr.networkInterface.ipV4Address);
      networkInterfaceConfiguration->ipV4Netmask.SetValue(hr.networkInterface.ipV4Netmask);
      networkInterfaceConfiguration->ipV4Gateway.SetValue(hr.networkInterface.ipV4Gateway);
    }

    networkInterfaceConfiguration->ipV6DhcpClientEnabled.SetValue(hr.networkInterface.ipV6DhcpClientEnabled);
    if (!hr.networkInterface.ipV6DhcpClientEnabled) {
      networkInterfaceConfiguration->ipV6GlobalAddress.SetValue(IpV6Address(hr.networkInterface.ipV6GlobalAddress[0], hr.networkInterface.ipV6GlobalAddress[1],
        hr.networkInterface.ipV6GlobalAddress[2], hr.networkInterface.ipV6GlobalAddress[3]));
    }
  }

  if (auto wifiStationConfiguration = dynamic_cast<PL::BlackBoxWiFiStationConfiguration*>(hardwareInterfaceConfiguration.get())) {
    std::string ssid(hr.wifi.ssid, maxWiFiSsidSize);
    wifiStationConfiguration->ssid.SetValue(ssid.c_str());
    bool passwordIsValid = true;
    for (int i = 0; i < hr.wifi.password[i] && i < sizeof(hr.wifi.password); i++) {
      if (hr.wifi.password[i] < 32 || hr.wifi.password[i] > 126)
        passwordIsValid = false;
    }

    if (passwordIsValid) {
      std::string password(hr.wifi.password, maxWiFiPasswordSize);
      wifiStationConfiguration->password.SetValue(password.c_str());
    }
  }

  return ESP_OK;
}

//==============================================================================

BlackBoxModbusServer::HardwareInterfaceConfigurationIR::HardwareInterfaceConfigurationIR(BlackBoxModbusServer& modbusServer) :
  ModbusMemoryArea(PL::ModbusMemoryType::inputRegisters, hardwareInterfaceConfigurationMemoryAddress, modbusServer.memoryDataBuffer->data, registerMemoryAreaSize, modbusServer.memoryDataBuffer),
  modbusServer(modbusServer) {}

//==============================================================================

esp_err_t BlackBoxModbusServer::HardwareInterfaceConfigurationIR::OnRead() {
  BlackBox& blackBox = *modbusServer.blackBox;
  
  memset(modbusServer.memoryDataBuffer->data, 0, BlackBoxModbusServer::registerMemoryAreaSize);
  auto& ir = modbusServer.memoryDataBuffer->data->hardwareInterfaceConfigurationIR;

  size_t numberOfHardwareInterfaces = blackBox.GetHardwareInterfaceConfigurations().size();
  if (!numberOfHardwareInterfaces || modbusServer.selectedHardwareInterfaceIndex >= numberOfHardwareInterfaces)
    return ESP_OK;

  auto hardwareInterface = blackBox.GetHardwareInterfaceConfigurations()[modbusServer.selectedHardwareInterfaceIndex]->GetHardwareInterface();

  auto name = hardwareInterface->GetName();
  memcpy(ir.common.name, name.data(), std::min(maxNameSize, name.size()));

  if (dynamic_cast<PL::Uart*>(hardwareInterface.get()))
    ir.common.type = (uint16_t)BlackBoxHardwareInterfaceType::uart;

  if (auto networkInterface = dynamic_cast<PL::NetworkInterface*>(hardwareInterface.get())) {
    ir.common.type = (uint16_t)BlackBoxHardwareInterfaceType::networkInterface;
    ir.networkInterface.connected = networkInterface->IsConnected();
    memcpy(ir.networkInterface.ipV6LinkLocalAddress, networkInterface->GetIpV6LinkLocalAddress().u32, sizeof(ir.networkInterface.ipV6LinkLocalAddress));
  }

  if (dynamic_cast<PL::Ethernet*>(hardwareInterface.get()))
    ir.common.type = (uint16_t)BlackBoxHardwareInterfaceType::ethernet;

  if (dynamic_cast<PL::WiFiStation*>(hardwareInterface.get()))
    ir.common.type = (uint16_t)BlackBoxHardwareInterfaceType::wifiStation;

#if TINYUSB_CDC_ENABLED
  if (dynamic_cast<PL::UsbDeviceCdc*>(hardwareInterface.get()))
    ir.common.type = (uint16_t)BlackBoxHardwareInterfaceType::usbDeviceCdc;
#endif

  return ESP_OK;
}

//==============================================================================

BlackBoxModbusServer::ServerConfigurationHR::ServerConfigurationHR(BlackBoxModbusServer& modbusServer, ModbusMemoryType memoryType, size_t size) :
  ModbusMemoryArea(memoryType, serverConfigurationMemoryAddress, modbusServer.memoryDataBuffer->data, size, modbusServer.memoryDataBuffer),
  modbusServer(modbusServer) {}

//==============================================================================

esp_err_t BlackBoxModbusServer::ServerConfigurationHR::OnRead() {
  BlackBox& blackBox = *modbusServer.blackBox;
  
  memset(modbusServer.memoryDataBuffer->data, 0, registerMemoryAreaSize);
  auto& hr = modbusServer.memoryDataBuffer->data->serverConfigurationHR;

  size_t numberOfServers = blackBox.GetServerConfigurations().size();
  if (!numberOfServers || modbusServer.selectedServerIndex >= numberOfServers)
    return ESP_OK;

  auto serverConfiguration = blackBox.GetServerConfigurations()[modbusServer.selectedServerIndex];

  hr.common.enabled = serverConfiguration->enabled.GetValue();

  if (auto networkServerConfiguration = dynamic_cast<PL::BlackBoxNetworkServerConfiguration*>(serverConfiguration.get())) {
    hr.networkServer.port = networkServerConfiguration->port.GetValue();
    hr.networkServer.maxNumberOfClients = networkServerConfiguration->maxNumberOfClients.GetValue();
  }

  if (auto modbusServerConfiguration = dynamic_cast<PL::BlackBoxModbusServerConfiguration*>(serverConfiguration.get())) {
    hr.modbusServer.protocol = (uint16_t)modbusServerConfiguration->protocol.GetValue();
    hr.modbusServer.stationAddress = modbusServerConfiguration->stationAddress.GetValue();
    if (auto modbusServer = dynamic_cast<PL::ModbusServer*>(serverConfiguration->GetServer().get())) {
      if (auto baseServer = modbusServer->GetBaseServer().lock()) {
        if (dynamic_cast<PL::NetworkServer*>(baseServer.get())) {
          hr.networkModbusServer.port = modbusServerConfiguration->port.GetValue();
          hr.networkModbusServer.maxNumberOfClients = modbusServerConfiguration->maxNumberOfClients.GetValue();
        }
      }
    }
  }

  return ESP_OK;
}

//==============================================================================

esp_err_t BlackBoxModbusServer::ServerConfigurationHR::OnWrite() {
  BlackBox& blackBox = *modbusServer.blackBox;

  auto& hr = modbusServer.memoryDataBuffer->data->serverConfigurationHR;

  size_t numberOfServers = blackBox.GetServerConfigurations().size();
  if (!numberOfServers || modbusServer.selectedServerIndex >= numberOfServers)
    return ESP_OK;

  auto serverConfiguration = blackBox.GetServerConfigurations()[modbusServer.selectedServerIndex];

  serverConfiguration->enabled.SetValue(hr.common.enabled);

  if (auto networkServerConfiguration = dynamic_cast<PL::BlackBoxNetworkServerConfiguration*>(serverConfiguration.get())) {
    networkServerConfiguration->port.SetValue(hr.networkServer.port);
    networkServerConfiguration->maxNumberOfClients.SetValue(hr.networkServer.maxNumberOfClients);
  }

  if (auto modbusServerConfiguration = dynamic_cast<PL::BlackBoxModbusServerConfiguration*>(serverConfiguration.get())) {
    modbusServerConfiguration->protocol.SetValue((ModbusProtocol)hr.modbusServer.protocol);
    modbusServerConfiguration->stationAddress.SetValue(std::min(hr.modbusServer.stationAddress, (uint16_t)255));
    if (auto modbusServer = dynamic_cast<PL::ModbusServer*>(serverConfiguration->GetServer().get())) {
      if (auto baseServer = modbusServer->GetBaseServer().lock()) {
        if (dynamic_cast<PL::NetworkServer*>(baseServer.get())) {
          modbusServerConfiguration->port.SetValue(hr.networkModbusServer.port);
          modbusServerConfiguration->maxNumberOfClients.SetValue(hr.networkModbusServer.maxNumberOfClients);
        }
      }
    }
  }

  return ESP_OK;
}

//==============================================================================

BlackBoxModbusServer::ServerConfigurationIR::ServerConfigurationIR(BlackBoxModbusServer& modbusServer) :
  ModbusMemoryArea(PL::ModbusMemoryType::inputRegisters, serverConfigurationMemoryAddress, modbusServer.memoryDataBuffer->data, registerMemoryAreaSize, modbusServer.memoryDataBuffer),
  modbusServer(modbusServer) {}

//==============================================================================

esp_err_t BlackBoxModbusServer::ServerConfigurationIR::OnRead() {
  BlackBox& blackBox = *modbusServer.blackBox;
  
  memset(modbusServer.memoryDataBuffer->data, 0, registerMemoryAreaSize);
  auto& ir = modbusServer.memoryDataBuffer->data->serverConfigurationIR;

  size_t numberOfServers = blackBox.GetServerConfigurations().size();
  if (!numberOfServers || modbusServer.selectedServerIndex >= numberOfServers)
    return ESP_OK;

  auto server = blackBox.GetServerConfigurations()[modbusServer.selectedServerIndex]->GetServer();

  size_t maxNameSize = BlackBoxModbusServer::maxNameSize;

  auto name = server->GetName();
  memcpy(ir.common.name, name.data(), std::min(maxNameSize, name.size()));

  if (dynamic_cast<PL::StreamServer*>(server.get()))
    ir.common.type = (uint16_t)BlackBoxServerType::streamServer;

  if (dynamic_cast<PL::NetworkServer*>(server.get()))
    ir.common.type = (uint16_t)BlackBoxServerType::networkServer;

  if (auto modbusServer = dynamic_cast<PL::ModbusServer*>(server.get())) {
    if (auto baseServer = modbusServer->GetBaseServer().lock()) {
      if (dynamic_cast<PL::NetworkServer*>(baseServer.get()))
        ir.common.type = (uint16_t)BlackBoxServerType::networkModbusServer;
      else
        ir.common.type = (uint16_t)BlackBoxServerType::streamModbusServer;
    }
  }

  if (dynamic_cast<PL::HttpServer*>(server.get()))
    ir.common.type = (uint16_t)BlackBoxServerType::httpServer;

  if (dynamic_cast<PL::MdnsServer*>(server.get()))
    ir.common.type = (uint16_t)BlackBoxServerType::mdnsServer;

  return ESP_OK;
}

//==============================================================================

}