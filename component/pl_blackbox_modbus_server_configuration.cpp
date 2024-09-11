#include "pl_blackbox_modbus_server_configuration.h"

//==============================================================================

namespace PL {

//==============================================================================

const std::string BlackBoxModbusServerConfiguration::protocolNvsKey = "protocol";
const std::string BlackBoxModbusServerConfiguration::stationAddressNvsKey = "staAddress";
const std::string BlackBoxModbusServerConfiguration::portNvsKey = "port";
const std::string BlackBoxModbusServerConfiguration::maxNumberOfClientsNvsKey = "maxClients";

//==============================================================================

BlackBoxModbusServerConfiguration::BlackBoxModbusServerConfiguration(std::shared_ptr<ModbusServer> modbusServer, std::string nvsNamespaceName) :
    BlackBoxServerConfiguration(modbusServer, nvsNamespaceName), protocol(modbusServer->GetProtocol()), stationAddress(modbusServer->GetStationAddress()),
    port(0), maxNumberOfClients(0), modbusServer(modbusServer) {
  if (auto baseServer = modbusServer->GetBaseServer().lock()) {
    if (auto networkServer = dynamic_cast<NetworkServer*>(baseServer.get())) {
      uint16_t portValue = networkServer->GetPort();
      port.SetValidValues(std::vector<uint16_t> { portValue });
      port.SetValue(portValue);

      size_t maxNumberOfClientsValue = networkServer->GetMaxNumberOfClients();
      maxNumberOfClients.SetValidValues(std::vector<size_t> { maxNumberOfClientsValue });
      maxNumberOfClients.SetValue(maxNumberOfClientsValue);
    }
  }
}

//==============================================================================

void BlackBoxModbusServerConfiguration::Load() {
  LockGuard lg(*this);
  NvsNamespace nvsNamespace(nvsNamespaceName, NvsAccessMode::readOnly);
  uint8_t u8Value;
  uint16_t u16Value;
  uint32_t u32Value;

  if (nvsNamespace.Read(protocolNvsKey, u8Value) == ESP_OK)
    protocol.SetValue((ModbusProtocol)u8Value);
  if (nvsNamespace.Read(stationAddressNvsKey, u8Value) == ESP_OK)
    stationAddress.SetValue(u8Value);

  if (auto baseServer = modbusServer->GetBaseServer().lock()) {
    if (dynamic_cast<NetworkServer*>(baseServer.get())) {
      if (nvsNamespace.Read(portNvsKey, u16Value) == ESP_OK)
        port.SetValue(u16Value);
      if (nvsNamespace.Read(maxNumberOfClientsNvsKey, u32Value) == ESP_OK)
        maxNumberOfClients.SetValue(u32Value);
    }
  }

  BlackBoxServerConfiguration::Load();
}

//==============================================================================

void BlackBoxModbusServerConfiguration::Save() {
  LockGuard lg(*this);
  NvsNamespace nvsNamespace(nvsNamespaceName, NvsAccessMode::readWrite);

  nvsNamespace.Write(protocolNvsKey, (uint8_t)protocol.GetValue());
  nvsNamespace.Write(stationAddressNvsKey, stationAddress.GetValue());
  if (auto baseServer = modbusServer->GetBaseServer().lock()) {
    if (dynamic_cast<NetworkServer*>(baseServer.get())) {
      nvsNamespace.Write(portNvsKey, port.GetValue());
      nvsNamespace.Write(maxNumberOfClientsNvsKey, (uint32_t)maxNumberOfClients.GetValue());
    }
  }

  BlackBoxServerConfiguration::Save();
}

//==============================================================================

void BlackBoxModbusServerConfiguration::Apply() {
  LockGuard lg(*this, *modbusServer);
  
  modbusServer->SetProtocol(protocol.GetValue());
  modbusServer->SetStationAddress(stationAddress.GetValue());
  if (auto baseServer = modbusServer->GetBaseServer().lock()) {
    if (auto networkServer = dynamic_cast<NetworkServer*>(baseServer.get())) {
      networkServer->SetPort(port.GetValue());
      networkServer->SetMaxNumberOfClients(maxNumberOfClients.GetValue());          
    }
  }
  
  BlackBoxServerConfiguration::Apply();
}
//==============================================================================

}