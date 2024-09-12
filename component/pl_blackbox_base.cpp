#include "pl_blackbox_base.h"
#include "esp_check.h"

//==============================================================================

static const char* TAG = "pl_blackbox_base";

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

esp_err_t BlackBox::Lock(TickType_t timeout) {
  ESP_RETURN_ON_ERROR(mutex.Lock(timeout), TAG, "mutex lock failed");
  return ESP_OK;
}

//==============================================================================

esp_err_t BlackBox::Unlock() {
  ESP_RETURN_ON_ERROR(mutex.Unlock(), TAG, "mutex unlock failed");
  return ESP_OK;
}

//==============================================================================

std::string BlackBox::GetHardwareInfoNvsNamespaceName() {
  LockGuard lg(*this);
  return hardwareInfoNvsNamespaceName;
}

//==============================================================================

void BlackBox::SetHardwareInfoNvsNamespaceName(const std::string& nvsNamespaceName) {
  LockGuard lg(*this);
  hardwareInfoNvsNamespaceName = nvsNamespaceName;
}

//==============================================================================

std::string BlackBox::GetGeneralConfigurationNvsNamespaceName() {
  return generalConfiguration->GetNvsNamespaceName();
}

//==============================================================================

void BlackBox::SetGeneralConfigurationNvsNamespaceName(const std::string& nvsNamespaceName) {
  LockGuard lg(*this);
  generalConfiguration->SetNvsNamespaceName(nvsNamespaceName);
  hardwareInfoLoaded = false;
}

//==============================================================================

BlackBoxHardwareInfo BlackBox::GetHardwareInfo() {
  LockGuard lg(*this);
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
  LockGuard lg(*this);
  return deviceName;
}

//==============================================================================

void BlackBox::SetDeviceName(const std::string& name) {
  LockGuard lg(*this);
  deviceName = name;
}

//==============================================================================

esp_err_t BlackBox::Restart() {
  LockGuard lg(*this);
  esp_restart();
  return ESP_OK;
}

//==============================================================================

bool BlackBox::GetRestartedFlag() {
  LockGuard lg(*this);
  return restartedFlag;
}

//==============================================================================

void BlackBox::ClearRestartedFlag() {
  LockGuard lg(*this);
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

#if TINYUSB_CDC_ENABLED
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
  LockGuard lg(*this);
  return allConfigurations;
}

//==============================================================================

std::vector<std::shared_ptr<BlackBoxHardwareInterfaceConfiguration>> BlackBox::GetHardwareInterfaceConfigurations() {
  LockGuard lg(*this);
  return hardwareInterfaceConfigurations;
}

//==============================================================================

std::vector<std::shared_ptr<BlackBoxServerConfiguration>> BlackBox::GetServerConfigurations() {
  LockGuard lg(*this);
  return serverConfigurations;
}

//==============================================================================

void BlackBox::LoadAllConfigurations() {
  LockGuard lg(*this);
  for (auto& configuration : allConfigurations)
    configuration->Load();
}

//==============================================================================

void BlackBox::SaveAllConfigurations() {
  LockGuard lg(*this);
  for (auto& configuration : allConfigurations)
    configuration->Save();
}

//==============================================================================

void BlackBox::EraseAllConfigurations() {
  LockGuard lg(*this);
  for (auto& configuration : allConfigurations)
    configuration->Erase();
}

//==============================================================================

void BlackBox::ApplyHardwareInterfaceConfigurations() {
  LockGuard lg(*this);
  for (auto& configuration : hardwareInterfaceConfigurations)
    configuration->Apply();
}

//==============================================================================

void BlackBox::ApplyServerConfigurations() {
  LockGuard lg(*this);
  for (auto& configuration : serverConfigurations)
    configuration->Apply();
}

//==============================================================================

BlackBox::GeneralConfiguration::GeneralConfiguration(BlackBox& blackBox) : BlackBoxConfiguration(defaultGeneralConfigurationNvsNamespaceName), blackBox(blackBox) {}
  
//==============================================================================

void BlackBox::GeneralConfiguration::Load() {
  LockGuard lg(*this);
  NvsNamespace nvsNamespace(nvsNamespaceName, NvsAccessMode::readOnly);
  std::string stringValue;

  if (nvsNamespace.Read(generalConfigurationDeviceNameNvsKey, stringValue) == ESP_OK)
    blackBox.SetDeviceName(stringValue);
}

//==============================================================================

void BlackBox::GeneralConfiguration::Save() {
  LockGuard lg(*this);
  NvsNamespace nvsNamespace(nvsNamespaceName, NvsAccessMode::readWrite);

  nvsNamespace.Write(generalConfigurationDeviceNameNvsKey, blackBox.GetDeviceName());
}

//==============================================================================

}