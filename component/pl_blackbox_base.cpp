#include "pl_blackbox_base.h"
#include "esp_check.h"

//==============================================================================

namespace PL {

//==============================================================================

const std::string BlackBox::defaultHardwareInfoNvsNamespaceName = "plbbHwInfo";
const std::string BlackBox::defaultGeneralConfigurationNvsNamespaceName = "plbbConfig";

const std::string BlackBox::hardwareInfoNameNvsKey = "name";
const std::string BlackBox::hardwareInfoMajorVersionNvsKey = "verMajor";
const std::string BlackBox::hardwareInfoMinorVersionNvsKey = "verMinor";
const std::string BlackBox::hardwareInfoPatchVersionNvsKey = "verPatch";
const std::string BlackBox::hardwareInfoUidNvsKey = "uid";

const std::string BlackBox::generalConfigurationDeviceNameNvsKey = "devName";

//==============================================================================

BlackBox::BlackBox() : generalConfiguration(std::make_shared<GeneralConfiguration>(*this)) {
  allConfigurations.push_back(generalConfiguration);
}

//==============================================================================

std::string BlackBox::GetHardwareInfoNvsNamespaceName() {
  LockGuard lg(mutex);
  return hardwareInfoNvsNamespaceName;
}

//==============================================================================

void BlackBox::SetHardwareInfoNvsNamespaceName(const std::string& nvsNamespaceName) {
  LockGuard lg(mutex);
  hardwareInfoNvsNamespaceName = nvsNamespaceName;
}

//==============================================================================

std::string BlackBox::GetGeneralConfigurationNvsNamespaceName() {
  return generalConfiguration->GetNvsNamespaceName();
}

//==============================================================================

void BlackBox::SetGeneralConfigurationNvsNamespaceName(const std::string& nvsNamespaceName) {
  LockGuard lg(mutex);
  generalConfiguration->SetNvsNamespaceName(nvsNamespaceName);
  hardwareInfoLoaded = false;
}

//==============================================================================

BlackBoxHardwareInfo BlackBox::GetHardwareInfo() {
  LockGuard lg(mutex);
  if (hardwareInfoLoaded)
    return hardwareInfo;
    
  NvsNamespace hardwareInfoNvs(hardwareInfoNvsNamespaceName, NvsAccessMode::readOnly);
  uint16_t u16Value;
  std::string stringValue;
  
  if (hardwareInfoNvs.Read(hardwareInfoNameNvsKey, stringValue) == ESP_OK)
    hardwareInfo.name = stringValue;
  if (hardwareInfoNvs.Read(hardwareInfoMajorVersionNvsKey, u16Value) == ESP_OK)
    hardwareInfo.version.major = u16Value;
  if (hardwareInfoNvs.Read(hardwareInfoMinorVersionNvsKey, u16Value) == ESP_OK)
    hardwareInfo.version.minor = u16Value;
  if (hardwareInfoNvs.Read(hardwareInfoPatchVersionNvsKey, u16Value) == ESP_OK)
    hardwareInfo.version.patch = u16Value;
  if (hardwareInfoNvs.Read(hardwareInfoUidNvsKey, stringValue) == ESP_OK)
    hardwareInfo.uid = stringValue;
  
  hardwareInfoLoaded = true;
  return hardwareInfo;
}

//==============================================================================

std::string BlackBox::GetDeviceName() {
  LockGuard lg(mutex);
  return deviceName;
}

//==============================================================================

void BlackBox::SetDeviceName(const std::string& name) {
  LockGuard lg(mutex);
  deviceName = name;
}

//==============================================================================

esp_err_t BlackBox::Restart() {
  LockGuard lg(mutex);
  esp_restart();
  return ESP_OK;
}

//==============================================================================

bool BlackBox::GetRestartedFlag() {
  LockGuard lg(mutex);
  return restartedFlag;
}

//==============================================================================

void BlackBox::ClearRestartedFlag() {
  LockGuard lg(mutex);
  restartedFlag = false;
}

//==============================================================================

void BlackBox::AddConfiguration(std::shared_ptr<BlackBoxConfiguration> configuration) {
  allConfigurations.push_back(configuration);
}

//==============================================================================

std::shared_ptr<BlackBoxHardwareInterfaceConfiguration> BlackBox::AddHardwareInterfaceConfiguration(std::shared_ptr<HardwareInterface> hardwareInterface, std::string nvsNamespaceName) {
  auto configuration = std::make_shared<BlackBoxHardwareInterfaceConfiguration>(hardwareInterface, nvsNamespaceName);
  hardwareInterfaceConfigurations.push_back(configuration);
  allConfigurations.push_back(configuration);
  return configuration;
}

//==============================================================================

std::shared_ptr<BlackBoxUartConfiguration> BlackBox::AddUartConfiguration(std::shared_ptr<Uart> hardwareInterface, std::string nvsNamespaceName) {
  auto configuration = std::make_shared<BlackBoxUartConfiguration>(hardwareInterface, nvsNamespaceName);
  hardwareInterfaceConfigurations.push_back(configuration);
  allConfigurations.push_back(configuration);
  return configuration;
}

//==============================================================================

std::shared_ptr<BlackBoxNetworkInterfaceConfiguration> BlackBox::AddNetworkInterfaceConfiguration(std::shared_ptr<NetworkInterface> hardwareInterface, std::string nvsNamespaceName) {
  auto configuration = std::make_shared<BlackBoxNetworkInterfaceConfiguration>(hardwareInterface, nvsNamespaceName);
  hardwareInterfaceConfigurations.push_back(configuration);
  allConfigurations.push_back(configuration);
  return configuration;
}

//==============================================================================

std::shared_ptr<BlackBoxEthernetConfiguration> BlackBox::AddEthernetConfiguration(std::shared_ptr<Ethernet> hardwareInterface, std::string nvsNamespaceName) {
  auto configuration = std::make_shared<BlackBoxEthernetConfiguration>(hardwareInterface, nvsNamespaceName);
  hardwareInterfaceConfigurations.push_back(configuration);
  allConfigurations.push_back(configuration);
  return configuration;
}

//==============================================================================

std::shared_ptr<BlackBoxWiFiStationConfiguration> BlackBox::AddWiFiConfiguration(std::shared_ptr<WiFiStation> hardwareInterface, std::string nvsNamespaceName) {
  auto configuration = std::make_shared<BlackBoxWiFiStationConfiguration>(hardwareInterface, nvsNamespaceName);
  hardwareInterfaceConfigurations.push_back(configuration);
  allConfigurations.push_back(configuration);
  return configuration;
}

//==============================================================================

#if CONFIG_TINYUSB_CDC_ENABLED
std::shared_ptr<BlackBoxUsbDeviceCdcConfiguration> BlackBox::AddUsbDeviceCdcConfiguration(std::shared_ptr<UsbDeviceCdc> hardwareInterface, std::string nvsNamespaceName) {
  auto configuration = std::make_shared<BlackBoxUsbDeviceCdcConfiguration>(hardwareInterface, nvsNamespaceName);
  hardwareInterfaceConfigurations.push_back(configuration);
  allConfigurations.push_back(configuration);
  return configuration;
}
#endif

//==============================================================================

std::shared_ptr<BlackBoxServerConfiguration> BlackBox::AddServerConfiguration(std::shared_ptr<Server> server, std::string nvsNamespaceName) {
  auto configuration = std::make_shared<BlackBoxServerConfiguration>(server, nvsNamespaceName);
  serverConfigurations.push_back(configuration);
  allConfigurations.push_back(configuration);
  return configuration;
}

//==============================================================================

std::shared_ptr<BlackBoxStreamServerConfiguration> BlackBox::AddStreamServerConfiguration(std::shared_ptr<StreamServer> server, std::string nvsNamespaceName) {
  auto configuration = std::make_shared<BlackBoxStreamServerConfiguration>(server, nvsNamespaceName);
  serverConfigurations.push_back(configuration);
  allConfigurations.push_back(configuration);
  return configuration;
}

//==============================================================================

std::shared_ptr<BlackBoxNetworkServerConfiguration> BlackBox::AddNetworkServerConfiguration(std::shared_ptr<NetworkServer> server, std::string nvsNamespaceName) {
  auto configuration = std::make_shared<BlackBoxNetworkServerConfiguration>(server, nvsNamespaceName);
  serverConfigurations.push_back(configuration);
  allConfigurations.push_back(configuration);
  return configuration;
}

//==============================================================================

std::shared_ptr<BlackBoxModbusServerConfiguration> BlackBox::AddModbusServerConfiguration(std::shared_ptr<ModbusServer> server, std::string nvsNamespaceName) {
  auto configuration = std::make_shared<BlackBoxModbusServerConfiguration>(server, nvsNamespaceName);
  serverConfigurations.push_back(configuration);
  allConfigurations.push_back(configuration);
  return configuration;
}

//==============================================================================

std::shared_ptr<BlackBoxHttpServerConfiguration> BlackBox::AddHttpServerConfiguration(std::shared_ptr<HttpServer> server, std::string nvsNamespaceName) {
  auto configuration = std::make_shared<BlackBoxHttpServerConfiguration>(server, nvsNamespaceName);
  serverConfigurations.push_back(configuration);
  allConfigurations.push_back(configuration);
  return configuration;
}

//==============================================================================

std::shared_ptr<BlackBoxMdnsServerConfiguration> BlackBox::AddMdnsServerConfiguration(std::shared_ptr<MdnsServer> server, std::string nvsNamespaceName) {
  auto configuration = std::make_shared<BlackBoxMdnsServerConfiguration>(server, nvsNamespaceName);
  serverConfigurations.push_back(configuration);
  allConfigurations.push_back(configuration);
  return configuration;
}

//==============================================================================

std::vector<std::shared_ptr<BlackBoxConfiguration>> BlackBox::GetAllConfigurations() {
  LockGuard lg(mutex);
  return allConfigurations;
}

//==============================================================================

std::vector<std::shared_ptr<BlackBoxHardwareInterfaceConfiguration>> BlackBox::GetHardwareInterfaceConfigurations() {
  LockGuard lg(mutex);
  return hardwareInterfaceConfigurations;
}

//==============================================================================

std::vector<std::shared_ptr<BlackBoxServerConfiguration>> BlackBox::GetServerConfigurations() {
  LockGuard lg(mutex);
  return serverConfigurations;
}

//==============================================================================

void BlackBox::LoadAllConfigurations() {
  LockGuard lg(mutex);
  for (auto& configuration : allConfigurations)
    configuration->Load();
}

//==============================================================================

void BlackBox::SaveAllConfigurations() {
  LockGuard lg(mutex);
  for (auto& configuration : allConfigurations)
    configuration->Save();
}

//==============================================================================

void BlackBox::EraseAllConfigurations() {
  LockGuard lg(mutex);
  for (auto& configuration : allConfigurations)
    configuration->Erase();
}

//==============================================================================

void BlackBox::ApplyHardwareInterfaceConfigurations() {
  LockGuard lg(mutex);
  for (auto& configuration : hardwareInterfaceConfigurations)
    configuration->Apply();
}

//==============================================================================

void BlackBox::ApplyServerConfigurations() {
  LockGuard lg(mutex);
  for (auto& configuration : serverConfigurations)
    configuration->Apply();
}

//==============================================================================

BlackBox::GeneralConfiguration::GeneralConfiguration(BlackBox& blackBox) : blackBox(blackBox), nvsNamespaceName(defaultGeneralConfigurationNvsNamespaceName) {}
  
//==============================================================================

void BlackBox::GeneralConfiguration::Load() {
  LockGuard lg(mutex);
  NvsNamespace nvsNamespace(nvsNamespaceName, NvsAccessMode::readOnly);
  std::string stringValue;

  if (nvsNamespace.Read(generalConfigurationDeviceNameNvsKey, stringValue) == ESP_OK)
    blackBox.SetDeviceName(stringValue);
}

//==============================================================================

void BlackBox::GeneralConfiguration::Save() {
  LockGuard lg(mutex);
  NvsNamespace nvsNamespace(nvsNamespaceName, NvsAccessMode::readWrite);

  nvsNamespace.Write(generalConfigurationDeviceNameNvsKey, blackBox.GetDeviceName());
}

//==============================================================================

void BlackBox::GeneralConfiguration::Erase() {
  LockGuard lg(mutex);
  NvsNamespace nvsNamespace(nvsNamespaceName, NvsAccessMode::readWrite);
  nvsNamespace.Erase();
}

//==============================================================================

std::string BlackBox::GeneralConfiguration::GetNvsNamespaceName() {
  LockGuard lg(mutex);
  return nvsNamespaceName;
}

//==============================================================================

void BlackBox::GeneralConfiguration::SetNvsNamespaceName(const std::string& nvsNamespaceName) {
  LockGuard lg(mutex);
  this->nvsNamespaceName = nvsNamespaceName;
}

//==============================================================================

}