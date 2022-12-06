#pragma once
#include "pl_blackbox_types.h"

//==============================================================================

namespace PL {

//==============================================================================

/// @brief Base class for the BlackBox device
class BlackBox : public Lockable {
public:
  /// @brief Hardware information NVS namespace
  static const std::string hardwareInfoNvsNamespace;
  /// @brief Hardware name NVS key
  static const std::string hardwareInfoNameNvsKey;
  /// @brief Hardware major version NVS key
  static const std::string hardwareInfoMajorVersionNvsKey;
  /// @brief Hardware minor version NVS key
  static const std::string hardwareInfoMinorVersionNvsKey;
  /// @brief Hardware patch version NVS key
  static const std::string hardwareInfoPatchVersionNvsKey;
  /// @brief Hardware UID NVS key
  static const std::string hardwareInfoUidNvsKey;

  /// @brief Configuration NVS namespace
  static const std::string configurationNvsNamespace;
  /// @brief Device name NVS key
  static const std::string configurationDeviceNameNvsKey;
  /// @brief Hardware interface NVS key prefix
  static const std::string configurationHardwareInterfaceNvsKeyPrefix;
  /// @brief Hardware interface enabled NVS key suffix
  static const std::string configurationHardwareInterfaceEnabledNvsKeySuffix;
  /// @brief Server NVS key prefix
  static const std::string configurationServerNvsKeyPrefix;
  /// @brief Server enabled NVS key suffix
  static const std::string configurationServerEnabledNvsKeySuffix;

  /// @brief UART baud rate NVS key suffix
  static const std::string configurationUartBaudRateNvsKeySuffix;
  /// @brief UART data bits NVS key suffix
  static const std::string configurationUartDataBitsNvsKeySuffix;
  /// @brief UART parity NVS key suffix
  static const std::string configurationUartParityNvsKeySuffix;
  /// @brief UART stop bits NVS key suffix
  static const std::string configurationUartStopBitsNvsKeySuffix;
  /// @brief UART flow control NVS key suffix
  static const std::string configurationUartFlowControlNvsKeySuffix;

  /// @brief Network interface IPv4 address NVS key suffix
  static const std::string configurationNetworkInterfaceIpV4AddressNvsKeySuffix;
  /// @brief Network interface IPv4 netmask NVS key suffix
  static const std::string configurationNetworkInterfaceIpV4NetmaskNvsKeySuffix;
  /// @brief Network interface IPv4 gateway NVS key suffix
  static const std::string configurationNetworkInterfaceIpV4GatewayNvsKeySuffix;
  /// @brief Network interface IPv6 global address NVS key suffix
  static const std::string configurationNetworkInterfaceIpV6GlobalAddressNvsKeySuffix;
  /// @brief Network interface IPv4 DHCP client enabled NVS key suffix
  static const std::string configurationNetworkInterfaceIpV4DhcpClientEnabledNvsKeySuffix;
  /// @brief Network interface IPv6 DHCP client enabled NVS key suffix
  static const std::string configurationNetworkInterfaceIpV6DhcpClientEnabledNvsKeySuffix;
  
  /// @brief WiFi SSID NVS key suffix
  static const std::string configurationWiFiSsidNvsKeySuffix;
  /// @brief WiFi Password NVS key suffix
  static const std::string configurationWiFiPasswordNvsKeySuffix;

  /// @brief Network server port NVS key suffix
  static const std::string configurationNetworkServerPortNvsKeySuffix;
  /// @brief Network server maximum number of clients NVS key suffix
  static const std::string configurationNetworkServerMaxNumberOfClientsNvsKeySuffix;

  /// @brief Modbus server protocol NVS key suffix
  static const std::string configurationModbusServerProtocolNvsKeySuffix;
  /// @brief Modbus server station address NVS key suffix
  static const std::string configurationModbusServerStationAddressNvsKeySuffix;

  /// @brief Create a BlackBox device
  BlackBox() {}

  esp_err_t Lock (TickType_t timeout = portMAX_DELAY) override;
  esp_err_t Unlock() override;

  /// @brief Restart the device
  /// @return error code
  esp_err_t Restart();

  /// @brief Load the device configuration (default: load from NVS)
  virtual void LoadConfiguration();

  /// @brief Save the device configuration (default: save to NVS)
  virtual void SaveConfiguration();

  /// @brief Erase the device configuration (default: erase from NVS)
  virtual void EraseConfiguration();

  /// @brief Get the device name
  /// @return name
  std::string GetName();

  /// @brief Set the device name
  /// @param name 
  void SetName (const std::string& name);

  /// @brief Get the hardware information (default: load from NVS)
  /// @return hardware information
  virtual BlackBoxHardwareInfo GetHardwareInfo();

  /// @brief Get the firmware information
  /// @return firmware information
  virtual BlackBoxFirmwareInfo GetFirmwareInfo() = 0;

  /// @brief Get the hardware interfaces
  /// @return hardware interfaces
  std::vector<std::shared_ptr<HardwareInterface>> GetHardwareInterfaces();

  /// @brief Add the hardware interface 
  /// @param hardwareInterface hardware interface 
  void AddHardwareInterface (std::shared_ptr<HardwareInterface> hardwareInterface);

  /// @brief Get the servers
  /// @return servers
  std::vector<std::shared_ptr<Server>> GetServers();

  /// @brief Add the server 
  /// @param hardwareInterface server
  void AddServer (std::shared_ptr<Server> server);

  /// @brief Check if the device has been restarted
  /// @return true if the device has been restarted
  bool GetRestartedFlag();

  /// @brief Clear the reset flag
  void ClearRestartedFlag();

private:
  Mutex mutex;
  BlackBoxHardwareInfo hardwareInfo = {};
  bool hardwareInfoLoaded = false;
  std::string name;
  std::vector<std::shared_ptr<HardwareInterface>> hardwareInterfaces;
  std::vector<std::shared_ptr<Server>> servers;
  bool restartedFlag = true;
  
};

//==============================================================================

}