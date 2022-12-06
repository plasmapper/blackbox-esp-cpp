#include "blackbox.h"
#include "unity.h"

//==============================================================================

static std::shared_ptr<BlackBox> blackBox = std::make_shared<BlackBox>();
const PL::BlackBoxHardwareInfo BlackBox::hardwareInfo = {"Test Hardware", {1, 2, 3}, "1234567890"};
const PL::BlackBoxFirmwareInfo BlackBox::firmwareInfo = {"Test Firmware", {4, 5, 6}};
static const std::string testName = "Test Name";

std::shared_ptr<PL::Uart> uart = std::make_shared<PL::Uart>(UART_NUM_1);
const uint32_t baudRate = 19200;
const uint16_t dataBits = 7;
const PL::UartParity parity = PL::UartParity::even;
const PL::UartStopBits stopBits = PL::UartStopBits::two;
const PL::UartFlowControl flowControl = PL::UartFlowControl::rtsCts;

std::shared_ptr<PL::EspWiFiStation> wifi = std::make_shared<PL::EspWiFiStation>();
const PL::IpV4Address ipAddress (1, 2, 3, 4);
const PL::IpV4Address netmask (4, 3, 2, 1); 
const PL::IpV4Address gateway (5, 6, 7, 8);
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
  blackBox->AddHardwareInterface (uart);
  blackBox->AddHardwareInterface (wifi);
  blackBox->AddServer (uartModbusServer);
  blackBox->AddServer (networkModbusServer);

  TEST_ASSERT (uart->Initialize() == ESP_OK);
  TEST_ASSERT (wifi->Initialize() == ESP_OK);

  auto hardwareInfo = blackBox->GetHardwareInfo();
  TEST_ASSERT (hardwareInfo.name == BlackBox::hardwareInfo.name);
  TEST_ASSERT (hardwareInfo.version.major == BlackBox::hardwareInfo.version.major);
  TEST_ASSERT (hardwareInfo.version.minor == BlackBox::hardwareInfo.version.minor);
  TEST_ASSERT (hardwareInfo.version.patch == BlackBox::hardwareInfo.version.patch);
  TEST_ASSERT (hardwareInfo.uid == BlackBox::hardwareInfo.uid);

  auto firmwareInfo = blackBox->GetFirmwareInfo();
  TEST_ASSERT (firmwareInfo.name == BlackBox::firmwareInfo.name);
  TEST_ASSERT (firmwareInfo.version.major == BlackBox::firmwareInfo.version.major);
  TEST_ASSERT (firmwareInfo.version.minor == BlackBox::firmwareInfo.version.minor);
  TEST_ASSERT (firmwareInfo.version.patch == BlackBox::firmwareInfo.version.patch);

  blackBox->SetName (testName);
  TEST_ASSERT (blackBox->GetName() == testName);

  TEST_ASSERT (blackBox->GetRestartedFlag());
  blackBox->ClearRestartedFlag();
  TEST_ASSERT (!blackBox->GetRestartedFlag());

  auto hardwareInterfaces = blackBox->GetHardwareInterfaces();
  TEST_ASSERT (hardwareInterfaces[0] == uart);
  TEST_ASSERT (hardwareInterfaces[1] == wifi);

  auto servers = blackBox->GetServers();
  TEST_ASSERT (servers[0] == uartModbusServer);
  TEST_ASSERT (servers[1] == networkModbusServer);

  TEST_ASSERT (uart->SetBaudRate (baudRate) == ESP_OK);
  TEST_ASSERT (uart->SetDataBits (dataBits) == ESP_OK);
  TEST_ASSERT (uart->SetParity (parity) == ESP_OK);
  TEST_ASSERT (uart->SetStopBits (stopBits) == ESP_OK);
  TEST_ASSERT (uart->SetFlowControl (flowControl) == ESP_OK);
  TEST_ASSERT (uart->Enable() == ESP_OK);
  TEST_ASSERT (wifi->DisableIpV4DhcpClient() == ESP_OK);
  TEST_ASSERT (wifi->SetIpV4Address (ipAddress) == ESP_OK);
  TEST_ASSERT (wifi->SetIpV4Netmask (netmask) == ESP_OK);
  TEST_ASSERT (wifi->SetIpV4Gateway (gateway) == ESP_OK);
  TEST_ASSERT (wifi->SetSsid (ssid) == ESP_OK);
  TEST_ASSERT (wifi->SetPassword (password) == ESP_OK);\
  TEST_ASSERT (wifi->Enable() == ESP_OK);
  TEST_ASSERT (uartModbusServer->SetProtocol (uartModbusServerProtocol) == ESP_OK);
  TEST_ASSERT (uartModbusServer->SetStationAddress (uartModbusServerStationAddress) == ESP_OK);
  TEST_ASSERT (networkModbusServer->SetProtocol (networkModbusServerProtocol) == ESP_OK);
  TEST_ASSERT (networkModbusServer->SetStationAddress (networkModbusServerStationAddress) == ESP_OK);
  TEST_ASSERT (((PL::NetworkServer*)networkModbusServer->GetBaseServer().lock().get())->SetPort (port) == ESP_OK);
  TEST_ASSERT (((PL::NetworkServer*)networkModbusServer->GetBaseServer().lock().get())->SetMaxNumberOfClients (maxNumberOfClients) == ESP_OK);
  TEST_ASSERT (uartModbusServer->Enable() == ESP_OK);
  TEST_ASSERT (networkModbusServer->Enable() == ESP_OK);

  blackBox->SaveConfiguration();

  TEST_ASSERT (uart->SetBaudRate (PL::Uart::defaultBaudRate) == ESP_OK);
  TEST_ASSERT (uart->SetDataBits (PL::Uart::defaultDataBits) == ESP_OK);
  TEST_ASSERT (uart->SetParity (PL::Uart::defaultParity) == ESP_OK);
  TEST_ASSERT (uart->SetStopBits (PL::Uart::defaultStopBits) == ESP_OK);
  TEST_ASSERT (uart->SetFlowControl (PL::Uart::defaultFlowControl) == ESP_OK);
  TEST_ASSERT (uart->Disable() == ESP_OK);
  TEST_ASSERT (wifi->SetIpV4Address (0) == ESP_OK);
  TEST_ASSERT (wifi->SetIpV4Netmask (0) == ESP_OK);
  TEST_ASSERT (wifi->SetIpV4Gateway (0) == ESP_OK);
  TEST_ASSERT (wifi->SetSsid ("") == ESP_OK);
  TEST_ASSERT (wifi->SetPassword ("") == ESP_OK);
  TEST_ASSERT (wifi->Disable() == ESP_OK);
  TEST_ASSERT (uartModbusServer->SetProtocol (PL::ModbusProtocol::rtu) == ESP_OK);
  TEST_ASSERT (uartModbusServer->SetStationAddress (0) == ESP_OK);
  TEST_ASSERT (networkModbusServer->SetProtocol (PL::ModbusProtocol::rtu) == ESP_OK);
  TEST_ASSERT (networkModbusServer->SetStationAddress (0) == ESP_OK);
  TEST_ASSERT (((PL::NetworkServer*)networkModbusServer->GetBaseServer().lock().get())->SetPort(1) == ESP_OK);
  TEST_ASSERT (((PL::NetworkServer*)networkModbusServer->GetBaseServer().lock().get())->SetMaxNumberOfClients (1) == ESP_OK);
  TEST_ASSERT (uartModbusServer->Disable() == ESP_OK);
  TEST_ASSERT (networkModbusServer->Disable() == ESP_OK);

  blackBox->LoadConfiguration();
  vTaskDelay (connectionTimeout);

  TEST_ASSERT (uart->IsEnabled());
  TEST_ASSERT_EQUAL (baudRate, uart->GetBaudRate());
  TEST_ASSERT_EQUAL (dataBits, uart->GetDataBits());
  TEST_ASSERT_EQUAL (parity, uart->GetParity());
  TEST_ASSERT_EQUAL (stopBits, uart->GetStopBits());
  TEST_ASSERT_EQUAL (flowControl, uart->GetFlowControl());
  TEST_ASSERT (wifi->IsEnabled());
  TEST_ASSERT (wifi->IsConnected());
  TEST_ASSERT_EQUAL (ipAddress.u32, wifi->GetIpV4Address().u32);
  TEST_ASSERT_EQUAL (netmask.u32, wifi->GetIpV4Netmask().u32);
  TEST_ASSERT_EQUAL (gateway.u32, wifi->GetIpV4Gateway().u32);
  TEST_ASSERT (wifi->GetSsid() == ssid);
  TEST_ASSERT (wifi->GetPassword() == password);

  TEST_ASSERT (uartModbusServer->IsEnabled());
  TEST_ASSERT (networkModbusServer->IsEnabled());
  TEST_ASSERT_EQUAL (uartModbusServerProtocol, uartModbusServer->GetProtocol());
  TEST_ASSERT_EQUAL (uartModbusServerStationAddress, uartModbusServer->GetStationAddress());
  TEST_ASSERT_EQUAL (networkModbusServerProtocol, networkModbusServer->GetProtocol());
  TEST_ASSERT_EQUAL (networkModbusServerStationAddress, networkModbusServer->GetStationAddress());
  TEST_ASSERT_EQUAL (port, ((PL::NetworkServer*)networkModbusServer->GetBaseServer().lock().get())->GetPort());
  TEST_ASSERT_EQUAL (maxNumberOfClients, ((PL::NetworkServer*)networkModbusServer->GetBaseServer().lock().get())->GetMaxNumberOfClients());

  blackBox->EraseConfiguration();
}

//==============================================================================

PL::BlackBoxFirmwareInfo BlackBox::GetFirmwareInfo() {
  return firmwareInfo;
}