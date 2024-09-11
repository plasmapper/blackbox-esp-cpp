#include "blackbox.h"
#include "blackbox_modbus.h"
#include "unity.h"

//==============================================================================

static auto blackBox = std::make_shared<BlackBox>();
static auto server = std::make_shared<PL::BlackBoxModbusServer>(blackBox, 502);
static PL::ModbusClient client(PL::IpV4Address(127, 0, 0, 1), 502);
static const char testName[PL::BlackBoxModbusServer::maxNameSize] = "Changed name";
static uint16_t data[PL::BlackBoxModbusServer::registerMemoryAreaSize / 2];

const uint32_t baudRate = 115200;
const uint16_t dataBits = 8;
const PL::UartParity parity = PL::UartParity::odd;
const PL::UartStopBits stopBits = PL::UartStopBits::onePointFive;
const PL::UartFlowControl flowControl = PL::UartFlowControl::rts;

const PL::IpV4Address ipAddress(8, 7, 6, 5);
const PL::IpV4Address netmask(5, 6, 7, 8); 
const PL::IpV4Address gateway(1, 2, 3, 4);
const std::string ssid = "ssid";
const std::string password = "password";
const TickType_t connectionTimeout = 5000 / portTICK_PERIOD_MS;

const uint16_t port = 101;
const size_t maxNumberOfClients = 11;
const uint8_t uartModbusServerStationAddress = 7;
const PL::ModbusProtocol uartModbusServerProtocol = PL::ModbusProtocol::tcp;
const uint8_t networkModbusServerStationAddress = 8;
const PL::ModbusProtocol networkModbusServerProtocol = PL::ModbusProtocol::ascii;

//==============================================================================

void TestBlackBoxModbus() {
  auto uartConfiguration = blackBox->AddUartConfiguration(uart, "uart");
  uartConfiguration->baudRate.DisableValueValidation();
  uartConfiguration->dataBits.DisableValueValidation();
  uartConfiguration->parity.DisableValueValidation();
  uartConfiguration->stopBits.DisableValueValidation();
  uartConfiguration->flowControl.DisableValueValidation();
  uartConfiguration->enabled.DisableValueValidation();

  auto wifiConfiguration = blackBox->AddWiFiConfiguration(wifi, "wifi");  wifiConfiguration->ipV4DhcpClientEnabled.DisableValueValidation();
  wifiConfiguration->ipV4Address.DisableValueValidation();
  wifiConfiguration->ipV4Netmask.DisableValueValidation();
  wifiConfiguration->ipV4Gateway.DisableValueValidation();
  wifiConfiguration->ipV6GlobalAddress.DisableValueValidation();
  wifiConfiguration->ipV6DhcpClientEnabled.DisableValueValidation();
  wifiConfiguration->ssid.DisableValueValidation();
  wifiConfiguration->password.DisableValueValidation();
  wifiConfiguration->enabled.DisableValueValidation();

  auto uartModbusServerConfiguration = blackBox->AddModbusServerConfiguration(uartModbusServer, "uartMbSrv");
  uartModbusServerConfiguration->protocol.DisableValueValidation();
  uartModbusServerConfiguration->stationAddress.DisableValueValidation();
  uartModbusServerConfiguration->enabled.DisableValueValidation();

  auto networkModbusServerConfiguration = blackBox->AddModbusServerConfiguration(networkModbusServer, "nwMbSrv");
  networkModbusServerConfiguration->protocol.DisableValueValidation();
  networkModbusServerConfiguration->stationAddress.DisableValueValidation();
  networkModbusServerConfiguration->port.DisableValueValidation();
  networkModbusServerConfiguration->maxNumberOfClients.DisableValueValidation();
  networkModbusServerConfiguration->enabled.DisableValueValidation();

  blackBox->AddModbusServerConfiguration(server, "bbMbSrv");

  TEST_ASSERT(server->Enable() == ESP_OK);
  vTaskDelay(10);

  // General
  TEST_ASSERT(client.ReadInputRegisters(PL::BlackBoxModbusServer::generalConfigurationMemoryAddress, PL::BlackBoxModbusServer::registerMemoryAreaSize / 2, data, NULL) == ESP_OK);
  TEST_ASSERT_EQUAL(blackBox->GetRestartedFlag(), data[1]);
  TEST_ASSERT(memcmp(PL::BlackBoxModbusServer::plbbSignature.data(), &data[2], 4) == 0);
  TEST_ASSERT_EQUAL(PL::BlackBoxModbusServer::memoryMapVersion, data[4]);
  auto hardwareInfo = blackBox->GetHardwareInfo();
  TEST_ASSERT(hardwareInfo.name == (char*)&data[5]);
  TEST_ASSERT_EQUAL(hardwareInfo.version.major, data[21]);
  TEST_ASSERT_EQUAL(hardwareInfo.version.minor, data[22]);
  TEST_ASSERT_EQUAL(hardwareInfo.version.patch, data[23]);
  TEST_ASSERT(hardwareInfo.uid == (char*)&data[24]);
  auto firmwareInfo = blackBox->GetFirmwareInfo();
  TEST_ASSERT(firmwareInfo.name == (char*)&data[40]);
  TEST_ASSERT_EQUAL(firmwareInfo.version.major, data[56]);
  TEST_ASSERT_EQUAL(firmwareInfo.version.minor, data[57]);
  TEST_ASSERT_EQUAL(firmwareInfo.version.patch, data[58]);
  TEST_ASSERT_EQUAL(blackBox->GetHardwareInterfaceConfigurations().size(), data[59]);
  TEST_ASSERT_EQUAL(blackBox->GetServerConfigurations().size(), data[60]);

  TEST_ASSERT(client.ReadHoldingRegisters(PL::BlackBoxModbusServer::generalConfigurationMemoryAddress, PL::BlackBoxModbusServer::registerMemoryAreaSize / 2, data, NULL) == ESP_OK);
  TEST_ASSERT(blackBox->GetDeviceName() == (char*)&data[2]);

  TEST_ASSERT(client.WriteSingleCoil(16, true, NULL) == ESP_OK);
  TEST_ASSERT(client.WriteMultipleHoldingRegisters(PL::BlackBoxModbusServer::generalConfigurationMemoryAddress + 2, sizeof(testName) / 2, testName, NULL) == ESP_OK);
  vTaskDelay(10);
  TEST_ASSERT_EQUAL(false, blackBox->GetRestartedFlag());
  TEST_ASSERT(blackBox->GetDeviceName() == testName);

  uint16_t hardwareInterfaceIndexToSet;
  uint16_t actualHardwareInterfaceIndex;

  // UART
  hardwareInterfaceIndexToSet = 0;
  TEST_ASSERT(uartConfiguration->enabled.SetValue(true) == ESP_OK);
  TEST_ASSERT(client.WriteSingleHoldingRegister(PL::BlackBoxModbusServer::generalConfigurationMemoryAddress + 18, hardwareInterfaceIndexToSet, NULL) == ESP_OK);
  vTaskDelay(10);
  TEST_ASSERT(client.ReadHoldingRegisters(PL::BlackBoxModbusServer::generalConfigurationMemoryAddress + 18, 1, &actualHardwareInterfaceIndex, NULL) == ESP_OK);
  TEST_ASSERT_EQUAL(hardwareInterfaceIndexToSet, actualHardwareInterfaceIndex);
  TEST_ASSERT(client.ReadInputRegisters(PL::BlackBoxModbusServer::hardwareInterfaceConfigurationMemoryAddress, PL::BlackBoxModbusServer::registerMemoryAreaSize / 2, data, NULL) == ESP_OK);
  TEST_ASSERT_EQUAL(PL::BlackBoxHardwareInterfaceType::uart, data[2]);
  TEST_ASSERT(uart->GetName() == (char*)&data[3]);

  TEST_ASSERT(client.ReadHoldingRegisters(PL::BlackBoxModbusServer::hardwareInterfaceConfigurationMemoryAddress, PL::BlackBoxModbusServer::registerMemoryAreaSize / 2, data, NULL) == ESP_OK);
  TEST_ASSERT_EQUAL(1, data[0]);
  TEST_ASSERT_EQUAL(uart->GetBaudRate(), *(uint32_t*)&data[2]);
  TEST_ASSERT_EQUAL(uart->GetDataBits(), data[4]);
  TEST_ASSERT_EQUAL(uart->GetParity(), data[5]);
  TEST_ASSERT_EQUAL(uart->GetStopBits(), data[6]);
  TEST_ASSERT_EQUAL(uart->GetFlowControl(), data[7]);

  TEST_ASSERT(client.WriteSingleCoil(PL::BlackBoxModbusServer::hardwareInterfaceConfigurationMemoryAddress, false, NULL) == ESP_OK);
  TEST_ASSERT(client.WriteMultipleHoldingRegisters(PL::BlackBoxModbusServer::hardwareInterfaceConfigurationMemoryAddress + 2, 2, &baudRate, NULL) == ESP_OK);
  TEST_ASSERT(client.WriteSingleHoldingRegister(PL::BlackBoxModbusServer::hardwareInterfaceConfigurationMemoryAddress + 4, dataBits, NULL) == ESP_OK);
  TEST_ASSERT(client.WriteSingleHoldingRegister(PL::BlackBoxModbusServer::hardwareInterfaceConfigurationMemoryAddress + 5, (uint16_t)parity, NULL) == ESP_OK);
  TEST_ASSERT(client.WriteSingleHoldingRegister(PL::BlackBoxModbusServer::hardwareInterfaceConfigurationMemoryAddress + 6, (uint16_t)stopBits, NULL) == ESP_OK);
  TEST_ASSERT(client.WriteSingleHoldingRegister(PL::BlackBoxModbusServer::hardwareInterfaceConfigurationMemoryAddress + 7, (uint16_t)flowControl, NULL) == ESP_OK);
  vTaskDelay(10);
  TEST_ASSERT(!uartConfiguration->enabled.GetValue());
  TEST_ASSERT_EQUAL(baudRate, uartConfiguration->baudRate.GetValue());
  TEST_ASSERT_EQUAL(dataBits, uartConfiguration->dataBits.GetValue());
  TEST_ASSERT_EQUAL(parity, uartConfiguration->parity.GetValue());
  TEST_ASSERT_EQUAL(stopBits, uartConfiguration->stopBits.GetValue());
  TEST_ASSERT_EQUAL(flowControl, uartConfiguration->flowControl.GetValue());

  // Wi-Fi
  hardwareInterfaceIndexToSet = 1;
  TEST_ASSERT(wifiConfiguration->enabled.SetValue(true) == ESP_OK);
  TEST_ASSERT(wifiConfiguration->ipV4DhcpClientEnabled.SetValue(true) == ESP_OK);
  TEST_ASSERT(client.WriteSingleHoldingRegister(PL::BlackBoxModbusServer::generalConfigurationMemoryAddress + 18, hardwareInterfaceIndexToSet, NULL) == ESP_OK);
  vTaskDelay(connectionTimeout);
  TEST_ASSERT(client.ReadHoldingRegisters(PL::BlackBoxModbusServer::generalConfigurationMemoryAddress + 18, 1, &actualHardwareInterfaceIndex, NULL) == ESP_OK);
  TEST_ASSERT_EQUAL(hardwareInterfaceIndexToSet, actualHardwareInterfaceIndex);
  TEST_ASSERT(wifi->IsConnected());
  TEST_ASSERT(wifi->GetIpV4Address().u32);
  TEST_ASSERT(wifi->GetIpV6LinkLocalAddress().u32[0]);
  TEST_ASSERT(client.ReadInputRegisters(PL::BlackBoxModbusServer::hardwareInterfaceConfigurationMemoryAddress, PL::BlackBoxModbusServer::registerMemoryAreaSize / 2, data, NULL) == ESP_OK);
  TEST_ASSERT_EQUAL(1, data[0]);
  TEST_ASSERT_EQUAL(PL::BlackBoxHardwareInterfaceType::wifiStation, data[2]);
  TEST_ASSERT(wifi->GetName() == (char*)&data[3]);
  auto ipV6LinkLocalAddress = wifi->GetIpV6LinkLocalAddress();
  TEST_ASSERT_EQUAL(ipV6LinkLocalAddress.u32[0], ((uint32_t*)&data[19])[0]);
  TEST_ASSERT_EQUAL(ipV6LinkLocalAddress.u32[1], ((uint32_t*)&data[19])[1]);
  TEST_ASSERT_EQUAL(ipV6LinkLocalAddress.u32[2], ((uint32_t*)&data[19])[2]);
  TEST_ASSERT_EQUAL(ipV6LinkLocalAddress.u32[3], ((uint32_t*)&data[19])[3]);

  TEST_ASSERT(client.ReadHoldingRegisters(PL::BlackBoxModbusServer::hardwareInterfaceConfigurationMemoryAddress, PL::BlackBoxModbusServer::registerMemoryAreaSize / 2, data, NULL) == ESP_OK);
  TEST_ASSERT_EQUAL(3, data[0]);
  TEST_ASSERT_EQUAL(wifi->GetIpV4Address().u32, *(uint32_t*)&data[2]);
  TEST_ASSERT_EQUAL(wifi->GetIpV4Netmask().u32, *(uint32_t*)&data[4]);
  TEST_ASSERT_EQUAL(wifi->GetIpV4Gateway().u32, *(uint32_t*)&data[6]);
  auto ipV6GlobalAddress = wifi->GetIpV6GlobalAddress();
  TEST_ASSERT_EQUAL(ipV6GlobalAddress.u32[0], ((uint32_t*)&data[8])[0]);
  TEST_ASSERT_EQUAL(ipV6GlobalAddress.u32[1], ((uint32_t*)&data[8])[1]);
  TEST_ASSERT_EQUAL(ipV6GlobalAddress.u32[2], ((uint32_t*)&data[8])[2]);
  TEST_ASSERT_EQUAL(ipV6GlobalAddress.u32[3], ((uint32_t*)&data[8])[3]);
  TEST_ASSERT(wifi->GetSsid() == (char*)&data[16]);

  TEST_ASSERT(client.WriteSingleCoil(PL::BlackBoxModbusServer::hardwareInterfaceConfigurationMemoryAddress, false, NULL) == ESP_OK);
  TEST_ASSERT(client.WriteSingleCoil(PL::BlackBoxModbusServer::hardwareInterfaceConfigurationMemoryAddress + 1, false, NULL) == ESP_OK);
  TEST_ASSERT(client.WriteMultipleHoldingRegisters(PL::BlackBoxModbusServer::hardwareInterfaceConfigurationMemoryAddress + 2, 2, &ipAddress, NULL) == ESP_OK);
  TEST_ASSERT(client.WriteMultipleHoldingRegisters(PL::BlackBoxModbusServer::hardwareInterfaceConfigurationMemoryAddress + 4, 2, &netmask, NULL) == ESP_OK);
  TEST_ASSERT(client.WriteMultipleHoldingRegisters(PL::BlackBoxModbusServer::hardwareInterfaceConfigurationMemoryAddress + 6, 2, &gateway, NULL) == ESP_OK);
  TEST_ASSERT(client.WriteMultipleHoldingRegisters(PL::BlackBoxModbusServer::hardwareInterfaceConfigurationMemoryAddress + 16, sizeof(ssid) / 2, ssid.data(), NULL) == ESP_OK);
  TEST_ASSERT(client.WriteMultipleHoldingRegisters(PL::BlackBoxModbusServer::hardwareInterfaceConfigurationMemoryAddress + 32, sizeof(password) / 2, password.data(), NULL) == ESP_OK);
  vTaskDelay(10);
  TEST_ASSERT(!wifiConfiguration->enabled.GetValue());
  TEST_ASSERT(!wifiConfiguration->ipV4DhcpClientEnabled.GetValue());
  TEST_ASSERT_EQUAL(ipAddress.u32, wifiConfiguration->ipV4Address.GetValue().u32);
  TEST_ASSERT_EQUAL(netmask.u32, wifiConfiguration->ipV4Netmask.GetValue().u32);
  TEST_ASSERT_EQUAL(gateway.u32, wifiConfiguration->ipV4Gateway.GetValue().u32);
  TEST_ASSERT(wifiConfiguration->ssid.GetValue() == ssid);
  TEST_ASSERT(wifiConfiguration->password.GetValue() == password);

  int16_t serverIndexToSet;
  uint16_t actualServerIndex;

  // UART server
  serverIndexToSet = 0;
  uartModbusServerConfiguration->enabled.SetValue(true);
  uartModbusServerConfiguration->Apply();
  TEST_ASSERT(client.WriteSingleHoldingRegister(PL::BlackBoxModbusServer::generalConfigurationMemoryAddress + 19, serverIndexToSet, NULL) == ESP_OK);
  vTaskDelay(10);
  TEST_ASSERT(client.ReadHoldingRegisters(PL::BlackBoxModbusServer::generalConfigurationMemoryAddress + 19, 1, &actualServerIndex, NULL) == ESP_OK);
  TEST_ASSERT_EQUAL(serverIndexToSet, actualServerIndex);
  TEST_ASSERT(client.ReadInputRegisters(PL::BlackBoxModbusServer::serverConfigurationMemoryAddress, PL::BlackBoxModbusServer::registerMemoryAreaSize / 2, data, NULL) == ESP_OK);
  TEST_ASSERT_EQUAL(PL::BlackBoxServerType::streamModbusServer, data[2]);
  TEST_ASSERT(uartModbusServer->GetName() == (char*)&data[3]);

  TEST_ASSERT(client.ReadHoldingRegisters(PL::BlackBoxModbusServer::serverConfigurationMemoryAddress, PL::BlackBoxModbusServer::registerMemoryAreaSize / 2, data, NULL) == ESP_OK);
  TEST_ASSERT_EQUAL(1, data[0]);
  TEST_ASSERT_EQUAL(uartModbusServer->GetProtocol(), data[2]);
  TEST_ASSERT_EQUAL(uartModbusServer->GetStationAddress(), data[3]);

  TEST_ASSERT(client.WriteSingleCoil(PL::BlackBoxModbusServer::serverConfigurationMemoryAddress, false, NULL) == ESP_OK);
  TEST_ASSERT(client.WriteSingleHoldingRegister(PL::BlackBoxModbusServer::serverConfigurationMemoryAddress + 2, (uint16_t)uartModbusServerProtocol, NULL) == ESP_OK);
  TEST_ASSERT(client.WriteSingleHoldingRegister(PL::BlackBoxModbusServer::serverConfigurationMemoryAddress + 3, uartModbusServerStationAddress, NULL) == ESP_OK);
  vTaskDelay(10);
  TEST_ASSERT(!uartModbusServerConfiguration->enabled.GetValue());
  TEST_ASSERT_EQUAL(uartModbusServerStationAddress, uartModbusServerConfiguration->stationAddress.GetValue());
  TEST_ASSERT_EQUAL(uartModbusServerProtocol, uartModbusServerConfiguration->protocol.GetValue());

  // Network server
  serverIndexToSet = 1;
  networkModbusServerConfiguration->enabled.SetValue(true);
  networkModbusServerConfiguration->Apply();
  TEST_ASSERT(client.WriteSingleHoldingRegister(PL::BlackBoxModbusServer::generalConfigurationMemoryAddress + 19, serverIndexToSet, NULL) == ESP_OK);
  vTaskDelay(10);
  TEST_ASSERT(client.ReadHoldingRegisters(PL::BlackBoxModbusServer::generalConfigurationMemoryAddress + 19, 1, &actualServerIndex, NULL) == ESP_OK);
  TEST_ASSERT_EQUAL(serverIndexToSet, actualServerIndex);
  TEST_ASSERT(client.ReadInputRegisters(PL::BlackBoxModbusServer::serverConfigurationMemoryAddress, PL::BlackBoxModbusServer::registerMemoryAreaSize / 2, data, NULL) == ESP_OK);
  TEST_ASSERT_EQUAL(PL::BlackBoxServerType::networkModbusServer, data[2]);
  TEST_ASSERT(networkModbusServer->GetName() == (char*)&data[3]);

  TEST_ASSERT(client.ReadHoldingRegisters(PL::BlackBoxModbusServer::serverConfigurationMemoryAddress, PL::BlackBoxModbusServer::registerMemoryAreaSize / 2, data, NULL) == ESP_OK);
  TEST_ASSERT_EQUAL(1, data[0]);
  TEST_ASSERT_EQUAL(networkModbusServer->GetProtocol(), data[2]);
  TEST_ASSERT_EQUAL(networkModbusServer->GetStationAddress(), data[3]);

  TEST_ASSERT(client.WriteSingleCoil(PL::BlackBoxModbusServer::serverConfigurationMemoryAddress, false, NULL) == ESP_OK);
  TEST_ASSERT(client.WriteSingleHoldingRegister(PL::BlackBoxModbusServer::serverConfigurationMemoryAddress + 2, (uint16_t)networkModbusServerProtocol, NULL) == ESP_OK);
  TEST_ASSERT(client.WriteSingleHoldingRegister(PL::BlackBoxModbusServer::serverConfigurationMemoryAddress + 3, networkModbusServerStationAddress, NULL) == ESP_OK);
  vTaskDelay(10);
  TEST_ASSERT(!networkModbusServerConfiguration->enabled.GetValue());
  TEST_ASSERT_EQUAL(networkModbusServerStationAddress, networkModbusServerConfiguration->stationAddress.GetValue());
  TEST_ASSERT_EQUAL(networkModbusServerProtocol, networkModbusServerConfiguration->protocol.GetValue());
}