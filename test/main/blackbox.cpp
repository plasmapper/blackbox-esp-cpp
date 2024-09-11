#include "blackbox.h"
#include "unity.h"

//==============================================================================

static std::shared_ptr<BlackBox> blackBox = std::make_shared<BlackBox>();
const PL::BlackBoxHardwareInfo BlackBox::hardwareInfo = {"Test Hardware", {1, 2, 3}, "1234567890"};
const PL::BlackBoxFirmwareInfo BlackBox::firmwareInfo = {"Test Firmware", {4, 5, 6}};
static const std::string testName = "Test Name";

std::shared_ptr<PL::Uart> uart = std::make_shared<PL::Uart>(UART_NUM_1);
const uint32_t baudRate = 19200;
const std::vector<uint32_t> validBaudRates { PL::Uart::defaultBaudRate, baudRate };
const uint16_t dataBits = 7;
const PL::UartParity parity = PL::UartParity::even;
const PL::UartStopBits stopBits = PL::UartStopBits::two;
const PL::UartFlowControl flowControl = PL::UartFlowControl::rtsCts;

std::shared_ptr<PL::EspWiFiStation> wifi = std::make_shared<PL::EspWiFiStation>();
const PL::IpV4Address ipAddress(1, 2, 3, 4);
const PL::IpV4Address netmask(4, 3, 2, 1); 
const PL::IpV4Address gateway(5, 6, 7, 8);
const std::string ssid = CONFIG_TEST_WIFI_SSID;
const std::string password = CONFIG_TEST_WIFI_PASSWORD;
const TickType_t connectionTimeout = 5000 / portTICK_PERIOD_MS;

std::shared_ptr<PL::ModbusServer> uartModbusServer = std::make_shared<PL::ModbusServer>(uart, PL::ModbusProtocol::rtu, 1);

std::shared_ptr<PL::ModbusServer> networkModbusServer = std::make_shared<PL::ModbusServer>(502);
const uint16_t port = 100;
const size_t maxNumberOfClients = 10;
const uint8_t uartModbusServerStationAddress = 5;
const PL::ModbusProtocol uartModbusServerProtocol = PL::ModbusProtocol::ascii;
const uint8_t networkModbusServerStationAddress = 6;
const PL::ModbusProtocol networkModbusServerProtocol = PL::ModbusProtocol::tcp;

//==============================================================================

void TestBlackBox() {
  auto uartConfiguration = blackBox->AddUartConfiguration(uart, "uart");
  uartConfiguration->baudRate.SetValidValues(validBaudRates);
  uartConfiguration->dataBits.DisableValueValidation();
  uartConfiguration->parity.DisableValueValidation();
  uartConfiguration->stopBits.DisableValueValidation();
  uartConfiguration->flowControl.DisableValueValidation();
  uartConfiguration->enabled.DisableValueValidation();

  auto wifiConfiguration = blackBox->AddWiFiConfiguration(wifi, "wifi");
  wifiConfiguration->ipV4DhcpClientEnabled.DisableValueValidation();
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

  TEST_ASSERT(uart->Initialize() == ESP_OK);
  TEST_ASSERT(wifi->Initialize() == ESP_OK);

  auto hardwareInfo = blackBox->GetHardwareInfo();
  TEST_ASSERT(hardwareInfo.name == BlackBox::hardwareInfo.name);
  TEST_ASSERT(hardwareInfo.version.major == BlackBox::hardwareInfo.version.major);
  TEST_ASSERT(hardwareInfo.version.minor == BlackBox::hardwareInfo.version.minor);
  TEST_ASSERT(hardwareInfo.version.patch == BlackBox::hardwareInfo.version.patch);
  TEST_ASSERT(hardwareInfo.uid == BlackBox::hardwareInfo.uid);

  auto firmwareInfo = blackBox->GetFirmwareInfo();
  TEST_ASSERT(firmwareInfo.name == BlackBox::firmwareInfo.name);
  TEST_ASSERT(firmwareInfo.version.major == BlackBox::firmwareInfo.version.major);
  TEST_ASSERT(firmwareInfo.version.minor == BlackBox::firmwareInfo.version.minor);
  TEST_ASSERT(firmwareInfo.version.patch == BlackBox::firmwareInfo.version.patch);

  blackBox->SetDeviceName(testName);
  TEST_ASSERT(blackBox->GetDeviceName() == testName);

  TEST_ASSERT(blackBox->GetRestartedFlag());
  blackBox->ClearRestartedFlag();
  TEST_ASSERT(!blackBox->GetRestartedFlag());

  auto hardwareInterfaceConfigurations = blackBox->GetHardwareInterfaceConfigurations();
  TEST_ASSERT(hardwareInterfaceConfigurations[0] == uartConfiguration);
  TEST_ASSERT(hardwareInterfaceConfigurations[1] == wifiConfiguration);

  auto serverConfigurations = blackBox->GetServerConfigurations();
  TEST_ASSERT(serverConfigurations[0] == uartModbusServerConfiguration);
  TEST_ASSERT(serverConfigurations[1] == networkModbusServerConfiguration);

  TEST_ASSERT(uartConfiguration->baudRate.SetValue(0) == ESP_ERR_INVALID_ARG);
  TEST_ASSERT(uartConfiguration->baudRate.SetValue(baudRate) == ESP_OK);
  TEST_ASSERT(uartConfiguration->dataBits.SetValue(dataBits) == ESP_OK);
  TEST_ASSERT(uartConfiguration->parity.SetValue(parity) == ESP_OK);
  TEST_ASSERT(uartConfiguration->stopBits.SetValue(stopBits) == ESP_OK);
  TEST_ASSERT(uartConfiguration->flowControl.SetValue(flowControl) == ESP_OK);
  TEST_ASSERT(uartConfiguration->enabled.SetValue(true) == ESP_OK);
  TEST_ASSERT(wifiConfiguration->ipV4DhcpClientEnabled.SetValue(false) == ESP_OK);
  TEST_ASSERT(wifiConfiguration->ipV4Address.SetValue(ipAddress) == ESP_OK);
  TEST_ASSERT(wifiConfiguration->ipV4Netmask.SetValue(netmask) == ESP_OK);
  TEST_ASSERT(wifiConfiguration->ipV4Gateway.SetValue(gateway) == ESP_OK);
  TEST_ASSERT(wifiConfiguration->ssid.SetValue(ssid) == ESP_OK);
  TEST_ASSERT(wifiConfiguration->password.SetValue(password) == ESP_OK);
  TEST_ASSERT(wifiConfiguration->enabled.SetValue(true) == ESP_OK);
  TEST_ASSERT(uartModbusServerConfiguration->protocol.SetValue(uartModbusServerProtocol) == ESP_OK);
  TEST_ASSERT(uartModbusServerConfiguration->stationAddress.SetValue(uartModbusServerStationAddress) == ESP_OK);
  TEST_ASSERT(uartModbusServerConfiguration->enabled.SetValue(true) == ESP_OK);
  TEST_ASSERT(networkModbusServerConfiguration->protocol.SetValue(networkModbusServerProtocol) == ESP_OK);
  TEST_ASSERT(networkModbusServerConfiguration->stationAddress.SetValue(networkModbusServerStationAddress) == ESP_OK);
  TEST_ASSERT(networkModbusServerConfiguration->port.SetValue(port) == ESP_OK);
  TEST_ASSERT(networkModbusServerConfiguration->maxNumberOfClients.SetValue(maxNumberOfClients) == ESP_OK);
  TEST_ASSERT(networkModbusServerConfiguration->enabled.SetValue(true) == ESP_OK);

  blackBox->SaveAllConfigurations();

  blackBox->SetDeviceName("");
  TEST_ASSERT(uartConfiguration->baudRate.SetValue(PL::Uart::defaultBaudRate) == ESP_OK);
  TEST_ASSERT(uartConfiguration->dataBits.SetValue(PL::Uart::defaultDataBits) == ESP_OK);
  TEST_ASSERT(uartConfiguration->parity.SetValue(PL::Uart::defaultParity) == ESP_OK);
  TEST_ASSERT(uartConfiguration->stopBits.SetValue(PL::Uart::defaultStopBits) == ESP_OK);
  TEST_ASSERT(uartConfiguration->flowControl.SetValue(PL::Uart::defaultFlowControl) == ESP_OK);
  TEST_ASSERT(uartConfiguration->enabled.SetValue(false) == ESP_OK);
  TEST_ASSERT(wifiConfiguration->ipV4Address.SetValue(0) == ESP_OK);
  TEST_ASSERT(wifiConfiguration->ipV4Netmask.SetValue(0) == ESP_OK);
  TEST_ASSERT(wifiConfiguration->ipV4Gateway.SetValue(0) == ESP_OK);
  TEST_ASSERT(wifiConfiguration->ssid.SetValue("") == ESP_OK);
  TEST_ASSERT(wifiConfiguration->password.SetValue("") == ESP_OK);
  TEST_ASSERT(wifiConfiguration->enabled.SetValue(false) == ESP_OK);
  TEST_ASSERT(uartModbusServerConfiguration->protocol.SetValue(PL::ModbusProtocol::rtu) == ESP_OK);
  TEST_ASSERT(uartModbusServerConfiguration->stationAddress.SetValue(0) == ESP_OK);
  TEST_ASSERT(uartModbusServerConfiguration->enabled.SetValue(false) == ESP_OK);
  TEST_ASSERT(networkModbusServerConfiguration->protocol.SetValue(PL::ModbusProtocol::rtu) == ESP_OK);
  TEST_ASSERT(networkModbusServerConfiguration->stationAddress.SetValue(0) == ESP_OK);
  TEST_ASSERT(networkModbusServerConfiguration->port.SetValue(1) == ESP_OK);
  TEST_ASSERT(networkModbusServerConfiguration->maxNumberOfClients.SetValue(1) == ESP_OK);
  TEST_ASSERT(networkModbusServerConfiguration->enabled.SetValue(false) == ESP_OK);

  blackBox->LoadAllConfigurations();

  TEST_ASSERT(blackBox->GetDeviceName() == testName);

  blackBox->ApplyHardwareInterfaceConfigurations();
  blackBox->ApplyServerConfigurations();

  vTaskDelay(connectionTimeout);

  TEST_ASSERT(uart->IsEnabled());
  TEST_ASSERT_EQUAL(baudRate, uart->GetBaudRate());
  TEST_ASSERT_EQUAL(dataBits, uart->GetDataBits());
  TEST_ASSERT_EQUAL(parity, uart->GetParity());
  TEST_ASSERT_EQUAL(stopBits, uart->GetStopBits());
  TEST_ASSERT_EQUAL(flowControl, uart->GetFlowControl());
  TEST_ASSERT(wifi->IsEnabled());
  TEST_ASSERT(wifi->IsConnected());
  TEST_ASSERT_EQUAL(ipAddress.u32, wifi->GetIpV4Address().u32);
  TEST_ASSERT_EQUAL(netmask.u32, wifi->GetIpV4Netmask().u32);
  TEST_ASSERT_EQUAL(gateway.u32, wifi->GetIpV4Gateway().u32);
  TEST_ASSERT(wifi->GetSsid() == ssid);
  TEST_ASSERT(wifi->GetPassword() == password);

  TEST_ASSERT(uartModbusServer->IsEnabled());
  TEST_ASSERT(networkModbusServer->IsEnabled());
  TEST_ASSERT_EQUAL(uartModbusServerProtocol, uartModbusServer->GetProtocol());
  TEST_ASSERT_EQUAL(uartModbusServerStationAddress, uartModbusServer->GetStationAddress());
  TEST_ASSERT_EQUAL(networkModbusServerProtocol, networkModbusServer->GetProtocol());
  TEST_ASSERT_EQUAL(networkModbusServerStationAddress, networkModbusServer->GetStationAddress());
  TEST_ASSERT_EQUAL(port, ((PL::NetworkServer*)networkModbusServer->GetBaseServer().lock().get())->GetPort());
  TEST_ASSERT_EQUAL(maxNumberOfClients, ((PL::NetworkServer*)networkModbusServer->GetBaseServer().lock().get())->GetMaxNumberOfClients());

  blackBox->EraseAllConfigurations();
}

//==============================================================================

PL::BlackBoxFirmwareInfo BlackBox::GetFirmwareInfo() {
  return firmwareInfo;
}