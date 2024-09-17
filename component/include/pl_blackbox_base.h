#pragma once
#include "pl_blackbox_types.h"
#include "pl_blackbox_hardware_interface_configuration.h"
#include "pl_blackbox_uart_configuration.h"
#include "pl_blackbox_network_interface_configuration.h"
#include "pl_blackbox_ethernet_configuration.h"
#include "pl_blackbox_wifi_station_configuration.h"
#include "pl_blackbox_usb_device_cdc_configuration.h"
#include "pl_blackbox_server_configuration.h"
#include "pl_blackbox_stream_server_configuration.h"
#include "pl_blackbox_network_server_configuration.h"
#include "pl_blackbox_http_server_configuration.h"
#include "pl_blackbox_mdns_server_configuration.h"
#include "pl_blackbox_modbus_server_configuration.h"

//==============================================================================

namespace PL {

//==============================================================================

/// @brief Base class for the BlackBox device
class BlackBox {
public:
  /// @brief Default hardware info NVS namespace
  static const std::string defaultHardwareInfoNvsNamespaceName;
  /// @brief Default general configuration NVS namespace
  static const std::string defaultGeneralConfigurationNvsNamespaceName;

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

  /// @brief General configuration device name NVS key
  static const std::string generalConfigurationDeviceNameNvsKey;

  /// @brief Creates a BlackBox device
  BlackBox();
  ~BlackBox() {}
  BlackBox(const BlackBox&) = delete;
  BlackBox& operator=(const BlackBox&) = delete;

  /// @brief Gets the hardware info NVS namespace name
  /// @return hardware info NVS namespace name
  std::string GetHardwareInfoNvsNamespaceName();

  /// @brief Sets the hardware info NVS namespace name
  /// @param nvsNamespaceName hardware info NVS namespace name
  void SetHardwareInfoNvsNamespaceName(const std::string& nvsNamespaceName);

  /// @brief Gets the general configuration NVS namespace name
  /// @return general configuration NVS namespace name
  std::string GetGeneralConfigurationNvsNamespaceName();

  /// @brief Sets the general configuration NVS namespace name
  /// @param nvsNamespaceName general configuration NVS namespace name
  void SetGeneralConfigurationNvsNamespaceName(const std::string& nvsNamespaceName);

  /// @brief Gets the hardware information
  /// @return hardware information
  virtual BlackBoxHardwareInfo GetHardwareInfo();

  /// @brief Gets the firmware information
  /// @return firmware information
  virtual BlackBoxFirmwareInfo GetFirmwareInfo() = 0;

  /// @brief Gets the device name
  /// @return device name
  std::string GetDeviceName();

  /// @brief Sets the device name
  /// @param name device name
  void SetDeviceName(const std::string& name);

  /// @brief Restarts the device
  /// @return error code
  virtual esp_err_t Restart();

  /// @brief Checks if the device has been restarted
  /// @return true if the device has been restarted
  bool GetRestartedFlag();

  /// @brief Clears the reset flag
  void ClearRestartedFlag();

  /// @brief Adds a configuration
  void AddConfiguration(std::shared_ptr<BlackBoxConfiguration> configuration);

  /// @brief Adds a hardware interface configuration
  /// @param hardwareInterface hardware interface
  /// @return hardware interface configuration
  std::shared_ptr<BlackBoxHardwareInterfaceConfiguration> AddHardwareInterfaceConfiguration(std::shared_ptr<HardwareInterface> hardwareInterface, std::string nvsNamespaceName);
  std::shared_ptr<BlackBoxUartConfiguration> AddUartConfiguration(std::shared_ptr<Uart> hardwareInterface, std::string nvsNamespaceName);
  std::shared_ptr<BlackBoxNetworkInterfaceConfiguration> AddNetworkInterfaceConfiguration(std::shared_ptr<NetworkInterface> hardwareInterface, std::string nvsNamespaceName);
  std::shared_ptr<BlackBoxEthernetConfiguration> AddEthernetConfiguration(std::shared_ptr<Ethernet> hardwareInterface, std::string nvsNamespaceName);
  std::shared_ptr<BlackBoxWiFiStationConfiguration> AddWiFiConfiguration(std::shared_ptr<WiFiStation> hardwareInterface, std::string nvsNamespaceName);
#if TINYUSB_CDC_ENABLED
  std::shared_ptr<BlackBoxUsbDeviceCdcConfiguration> AddUsbDeviceCdcConfiguration(std::shared_ptr<UsbDeviceCdc> hardwareInterface, std::string nvsNamespaceName);
#endif

  /// @brief Adds a server configuration
  /// @param server server
  /// @return server configuration
  std::shared_ptr<BlackBoxServerConfiguration> AddServerConfiguration(std::shared_ptr<Server> server, std::string nvsNamespaceName);
  std::shared_ptr<BlackBoxStreamServerConfiguration> AddStreamServerConfiguration(std::shared_ptr<StreamServer> server, std::string nvsNamespaceName);
  std::shared_ptr<BlackBoxNetworkServerConfiguration> AddNetworkServerConfiguration(std::shared_ptr<NetworkServer> server, std::string nvsNamespaceName);
  std::shared_ptr<BlackBoxModbusServerConfiguration> AddModbusServerConfiguration(std::shared_ptr<ModbusServer> server, std::string nvsNamespaceName);
  std::shared_ptr<BlackBoxHttpServerConfiguration> AddHttpServerConfiguration(std::shared_ptr<HttpServer> server, std::string nvsNamespaceName);
  std::shared_ptr<BlackBoxMdnsServerConfiguration> AddMdnsServerConfiguration(std::shared_ptr<MdnsServer> server, std::string nvsNamespaceName);

  /// @brief Gets all configurations
  /// @return configurations
  std::vector<std::shared_ptr<BlackBoxConfiguration>> GetAllConfigurations();

  /// @brief Gets the hardware interface configurations
  /// @return hardware interface configurations
  std::vector<std::shared_ptr<BlackBoxHardwareInterfaceConfiguration>> GetHardwareInterfaceConfigurations();

  /// @brief Gets the server configurations
  /// @return server configurations
  std::vector<std::shared_ptr<BlackBoxServerConfiguration>> GetServerConfigurations();

  /// @brief Loads all configurations
  void LoadAllConfigurations();

  /// @brief Saves all configurations
  void SaveAllConfigurations();

  /// @brief Erases all configurations
  void EraseAllConfigurations();

  /// @brief Applies hardware interface configurations
  void ApplyHardwareInterfaceConfigurations();

  /// @brief Applies server configurations
  void ApplyServerConfigurations();

private:
  Mutex mutex;
  std::string hardwareInfoNvsNamespaceName = defaultHardwareInfoNvsNamespaceName;
  BlackBoxHardwareInfo hardwareInfo = {};
  bool hardwareInfoLoaded = false;
  std::string deviceName;
  bool restartedFlag = true;
  std::vector<std::shared_ptr<BlackBoxConfiguration>> allConfigurations;
  std::vector<std::shared_ptr<BlackBoxHardwareInterfaceConfiguration>> hardwareInterfaceConfigurations;
  std::vector<std::shared_ptr<BlackBoxServerConfiguration>> serverConfigurations;

  class GeneralConfiguration : public BlackBoxConfiguration {
    public:
      GeneralConfiguration(BlackBox& blackBox);

      void Load() override;
      void Save() override;
      void Erase() override;

      std::string GetNvsNamespaceName();
      void SetNvsNamespaceName(const std::string& nvsNamespaceName);

    private:
      Mutex mutex;
      BlackBox& blackBox;
      std::string nvsNamespaceName;
  };

  std::shared_ptr<GeneralConfiguration> generalConfiguration;
};

//==============================================================================

}