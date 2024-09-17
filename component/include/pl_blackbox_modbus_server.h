#pragma once
#include "pl_blackbox_base.h"
#include "pl_modbus.h"

//==============================================================================

namespace PL {

//==============================================================================

class BlackBoxModbusServer : public ModbusServer {
public:
  /// @brief Memory area size for holding and input registers
  inline static const uint16_t registerMemoryAreaSize = 200;
  /// @brief Memory area size for coils
  inline static const uint16_t coilMemoryAreaSize = 4;
  /// @brief General configuration memory address
  static const uint16_t generalConfigurationMemoryAddress = 0;
  /// @brief Hardware interface configuration memory address
  static const uint16_t hardwareInterfaceConfigurationMemoryAddress = generalConfigurationMemoryAddress + registerMemoryAreaSize / 2;
  /// @brief Server configuration memory address
  static const uint16_t serverConfigurationMemoryAddress = hardwareInterfaceConfigurationMemoryAddress + registerMemoryAreaSize / 2;

  /// @brief BlackBox signature
  static const std::string plbbSignature;
  /// @brief Memory map version
  static const uint16_t memoryMapVersion = 1;

  /// @brief Maximum device, firmware and hardware name size
  inline static const size_t maxNameSize = 32;
  /// @brief Maximum Wi-Fi SSID size
  inline static const size_t maxWiFiSsidSize = 32;
  /// @brief Maximum Wi-Fi password size
  inline static const size_t maxWiFiPasswordSize = 64;

  /// @brief Creates a stream BlackBox Modbus server with shared transaction buffer
  /// @param blackBox BlackBox
  /// @param port UART port
  /// @param protocol Modbus protocol
  /// @param stationAddress station address
  /// @param buffer transaction buffer
  BlackBoxModbusServer(std::shared_ptr<BlackBox> blackBox, std::shared_ptr<Stream> stream, ModbusProtocol protocol, uint8_t stationAddress, std::shared_ptr<Buffer> buffer);

  /// @brief Creates a stream BlackBox Modbus server and allocates a transaction buffer
  /// @param blackBox BlackBox
  /// @param port UART port
  /// @param protocol Modbus protocol
  /// @param stationAddress station address
  /// @param bufferSize transaction buffer size
  BlackBoxModbusServer(std::shared_ptr<BlackBox> blackBox, std::shared_ptr<Stream> stream, ModbusProtocol protocol, uint8_t stationAddress, size_t bufferSize = defaultBufferSize);
  
  /// @brief Creates a network BlackBox Modbus server with shared transaction buffer
  /// @param blackBox BlackBox
  /// @param port network port
  /// @param buffer transaction buffer
  BlackBoxModbusServer(std::shared_ptr<BlackBox> blackBox, uint16_t port, std::shared_ptr<Buffer> buffer);
  
  /// @brief Creates a network BlackBox Modbus server and allocates a transaction buffer
  /// @param blackBox BlackBox
  /// @param port network port
  /// @param bufferSize transaction buffer size
  BlackBoxModbusServer(std::shared_ptr<BlackBox> blackBox, uint16_t port, size_t bufferSize = defaultBufferSize);

private:
  std::shared_ptr<BlackBox> blackBox;
  uint16_t selectedHardwareInterfaceIndex = 0;
  uint16_t selectedServerIndex = 0;

  #pragma pack(push, 1)
  union MemoryData {
    uint8_t dummy[registerMemoryAreaSize];
    
    struct GeneralConfigurationHR {
      uint16_t restart:1;
      uint16_t saveConfiguration:1;
      uint16_t :0;
      uint16_t clearRestartedFlag:1;
      uint16_t :0;
      char name[maxNameSize];
      uint16_t selectedHardwareInterfaceIndex;
      uint16_t selectedServerIndex;
    } generalConfigurationHR;

    struct GeneralConfigurationIR {
      uint16_t statusBits;
      uint16_t restartedFlag:1;
      uint16_t :0;
      char plbbSignature[4];
      uint16_t memoryMapVersion;
      struct {
        char name[maxNameSize];
        struct {
          uint16_t major;
          uint16_t minor;
          uint16_t patch;
        } version;
        char uid[maxNameSize];
      } hardwareInfo;
      struct {
        char name[maxNameSize];
        struct {
          uint16_t major;
          uint16_t minor;
          uint16_t patch;
        } version;
      } firmwareInfo;
      uint16_t numberOfHardwareInterfaces;
      uint16_t numberOfServers;
    } generalConfigurationIR;

    union HardwareInterfaceConfigurationHR {
      struct Common {
        uint16_t enabled:1;
        uint16_t :0;
        uint16_t clearStickyStatusBits;
      } common;

      struct Uart {
        uint8_t common[sizeof(Common)];
        uint32_t baudRate;
        uint16_t dataBits;
        uint16_t parity;
        uint16_t stopBits;
        uint16_t flowControl;
      } uart;
      
      struct NetworkInterface {
        uint16_t enabled:1;
        uint16_t ipV4DhcpClientEnabled:1;
        uint16_t ipV6DhcpClientEnabled:1;
        uint16_t :0;
        uint16_t clearStickyStatusBits;
        uint32_t ipV4Address;
        uint32_t ipV4Netmask;
        uint32_t ipV4Gateway;
        uint32_t ipV6GlobalAddress[4];
      } networkInterface;
      
      struct Ethernet {
        uint8_t networkInterface[sizeof(NetworkInterface)];
      } ethernet;
      
      struct WiFi {
        uint8_t networkInterface[sizeof(NetworkInterface)];
        char ssid[maxWiFiSsidSize];
        char password[maxWiFiPasswordSize];
      } wifi;

      struct UsbDeviceCdc {
        uint8_t common[sizeof(Common)];
      } usbDeviceCdc;
    } hardwareInterfaceConfigurationHR;

    union HardwareInterfaceConfigurationIR {
      struct Common {
        uint16_t statusBits;
        uint16_t stickyStatusBits;
        uint16_t type;
        char name[maxNameSize];
      } common;

      struct Uart {
        uint8_t common[sizeof(Common)];
      } uart;
      
      struct NetworkInterface {
        uint16_t connected : 1;
        uint16_t :0;
        uint16_t stickyStatusBits;
        uint16_t type;
        char name[maxNameSize];
        uint32_t ipV6LinkLocalAddress[4];
      } networkInterface;
      
      struct Ethernet {
        uint8_t networkInterface[sizeof(NetworkInterface)];
      } ethernet;
      
      struct WiFi {
        uint8_t networkInterface[sizeof(NetworkInterface)];
      } wifi;

      struct UsbDeviceCdc {
        uint8_t common[sizeof(Common)];
      } usbDeviceCdc;
    } hardwareInterfaceConfigurationIR;

    union ServerConfigurationHR {
      struct Common {
        uint16_t enabled:1;
        uint16_t :0;
        uint16_t clearStickyStatusBits;
      } common;

      struct StreamServer {
        uint8_t common[sizeof(Common)];
      } streamServer;
      
      struct NetworkServer {
        uint8_t common[sizeof(Common)];
        uint16_t port;
        uint16_t maxNumberOfClients;
      } networkServer;
      
      struct ModbusServer {
        uint8_t common[sizeof(Common)];
        uint16_t protocol;
        uint16_t stationAddress;
      } modbusServer;
      
      struct NetworkModbusServer {
        uint8_t modbusServer[sizeof(ModbusServer)];
        uint16_t port;
        uint16_t maxNumberOfClients;
      } networkModbusServer;

      struct HttpServer {
        uint8_t networkServer[sizeof(NetworkServer)];
      } httpServer;

      struct MdnsServer {
        uint8_t networkServer[sizeof(NetworkServer)];
      } mdnsServer;
    } serverConfigurationHR;

    union ServerConfigurationIR {
      struct Common {
        uint16_t statusBits;
        uint16_t stickyStatusBits;
        uint16_t type;
        char name[maxNameSize];
      } common;

      struct StreamServer {
        uint8_t common[sizeof(Common)];
      } streamServer;
      
      struct NetworkServer {
        uint8_t common[sizeof(Common)];
      } networkServer;
      
      struct ModbusServer {
        uint8_t common[sizeof(Common)];
      } modbusServer;
      
      struct NetworkModbusServer {
        uint8_t common[sizeof(Common)];
      } networkModbusServer;

      struct HttpServer {
        uint8_t networkServer[sizeof(NetworkServer)];
      } httpServer;

      struct MdnsServer {
        uint8_t networkServer[sizeof(NetworkServer)];
      } mdnsServer;
    } serverConfigurationIR;
  } memoryData;
  #pragma pack(pop)

  class GeneralConfigurationHR : public ModbusMemoryArea {
  public:
    GeneralConfigurationHR(BlackBoxModbusServer& modbusServer, ModbusMemoryType memoryType, size_t size);
    esp_err_t OnRead() override;
    esp_err_t OnWrite() override;
  
  private:
    BlackBoxModbusServer& modbusServer;
  };

  class GeneralConfigurationIR : public ModbusMemoryArea {
  public:
    GeneralConfigurationIR(BlackBoxModbusServer& modbusServer);
    esp_err_t OnRead() override;
  
  private:
    BlackBoxModbusServer& modbusServer;
  };

  class HardwareInterfaceConfigurationHR : public ModbusMemoryArea {
  public:
    HardwareInterfaceConfigurationHR(BlackBoxModbusServer& modbusServer, ModbusMemoryType memoryType, size_t size);
    esp_err_t OnRead() override;
    esp_err_t OnWrite() override;
  
  private:
    BlackBoxModbusServer& modbusServer;
  };

  class HardwareInterfaceConfigurationIR : public ModbusMemoryArea {
  public:
    HardwareInterfaceConfigurationIR(BlackBoxModbusServer& modbusServer);
    esp_err_t OnRead() override;
  
  private:
    BlackBoxModbusServer& modbusServer;
  };

  class ServerConfigurationHR : public ModbusMemoryArea {
  public:
    ServerConfigurationHR(BlackBoxModbusServer& modbusServer, ModbusMemoryType memoryType, size_t size);
    esp_err_t OnRead() override;
    esp_err_t OnWrite() override;
  
  private:
    BlackBoxModbusServer& modbusServer;
  };

  class ServerConfigurationIR : public ModbusMemoryArea {
  public:
    ServerConfigurationIR(BlackBoxModbusServer& modbusServer);
    esp_err_t OnRead() override;
  
  private:
    BlackBoxModbusServer& modbusServer;
  };
  
  std::shared_ptr<PL::TypedBuffer<MemoryData>> memoryDataBuffer;

  void AddMemoryAreas();
};

//==============================================================================

}