#pragma once
#include "pl_blackbox_base.h"
#include "pl_modbus.h"

//==============================================================================

// Multi-byte fields are read/written relying on the target being little-endian (true for every ESP32 variant)
static_assert(__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__, "MemoryData composition assumes a little-endian host");

//==============================================================================

namespace PL {

//==============================================================================

class BlackBoxModbusServer : public ModbusServer {
public:
  /// @brief Memory area size for holding and input registers
  static constexpr uint16_t registerMemoryAreaSize = 200;
  /// @brief Memory area size for coils
  static constexpr uint16_t coilMemoryAreaSize = 4;
  /// @brief General configuration memory address
  static constexpr uint16_t generalConfigurationMemoryAddress = 0;
  /// @brief Hardware interface configuration memory address
  static constexpr uint16_t hardwareInterfaceConfigurationMemoryAddress = generalConfigurationMemoryAddress + registerMemoryAreaSize / 2;
  /// @brief Server configuration memory address
  static constexpr uint16_t serverConfigurationMemoryAddress = hardwareInterfaceConfigurationMemoryAddress + registerMemoryAreaSize / 2;

  /// @brief BlackBox signature
  static const std::string plbbSignature;
  /// @brief Memory map version
  static constexpr uint16_t memoryMapVersion = 1;

  /// @brief Maximum device, firmware and hardware name size
  static constexpr size_t maxNameSize = 32;
  /// @brief Maximum Wi-Fi SSID size
  static constexpr size_t maxWiFiSsidSize = 32;
  /// @brief Maximum Wi-Fi password size
  static constexpr size_t maxWiFiPasswordSize = 64;

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

  /// @note Every derived class must call StopTask as the first statement of its own destructor
  /// so that the base server task does not access partially destroyed memory areas.
  ~BlackBoxModbusServer();

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
    static_assert(offsetof(GeneralConfigurationHR, name) == 4, "GeneralConfigurationHR::name offset does not match the register map");
    static_assert(offsetof(GeneralConfigurationHR, selectedHardwareInterfaceIndex) == 36, "GeneralConfigurationHR::selectedHardwareInterfaceIndex offset does not match the register map");
    static_assert(offsetof(GeneralConfigurationHR, selectedServerIndex) == 38, "GeneralConfigurationHR::selectedServerIndex offset does not match the register map");
    static_assert(sizeof(GeneralConfigurationHR) == 40, "GeneralConfigurationHR size does not match the register map");

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
    static_assert(offsetof(GeneralConfigurationIR, statusBits) == 0, "GeneralConfigurationIR::statusBits offset does not match the register map");
    static_assert(offsetof(GeneralConfigurationIR, plbbSignature) == 4, "GeneralConfigurationIR::plbbSignature offset does not match the register map");
    static_assert(offsetof(GeneralConfigurationIR, memoryMapVersion) == 8, "GeneralConfigurationIR::memoryMapVersion offset does not match the register map");
    static_assert(offsetof(GeneralConfigurationIR, hardwareInfo.name) == 10, "GeneralConfigurationIR::hardwareInfo.name offset does not match the register map");
    static_assert(offsetof(GeneralConfigurationIR, hardwareInfo.version.major) == 42, "GeneralConfigurationIR::hardwareInfo.version.major offset does not match the register map");
    static_assert(offsetof(GeneralConfigurationIR, hardwareInfo.version.minor) == 44, "GeneralConfigurationIR::hardwareInfo.version.minor offset does not match the register map");
    static_assert(offsetof(GeneralConfigurationIR, hardwareInfo.version.patch) == 46, "GeneralConfigurationIR::hardwareInfo.version.patch offset does not match the register map");
    static_assert(offsetof(GeneralConfigurationIR, hardwareInfo.uid) == 48, "GeneralConfigurationIR::hardwareInfo.uid offset does not match the register map");
    static_assert(offsetof(GeneralConfigurationIR, firmwareInfo.name) == 80, "GeneralConfigurationIR::firmwareInfo.name offset does not match the register map");
    static_assert(offsetof(GeneralConfigurationIR, firmwareInfo.version.major) == 112, "GeneralConfigurationIR::firmwareInfo.version.major offset does not match the register map");
    static_assert(offsetof(GeneralConfigurationIR, firmwareInfo.version.minor) == 114, "GeneralConfigurationIR::firmwareInfo.version.minor offset does not match the register map");
    static_assert(offsetof(GeneralConfigurationIR, firmwareInfo.version.patch) == 116, "GeneralConfigurationIR::firmwareInfo.version.patch offset does not match the register map");
    static_assert(offsetof(GeneralConfigurationIR, numberOfHardwareInterfaces) == 118, "GeneralConfigurationIR::numberOfHardwareInterfaces offset does not match the register map");
    static_assert(offsetof(GeneralConfigurationIR, numberOfServers) == 120, "GeneralConfigurationIR::numberOfServers offset does not match the register map");
    static_assert(sizeof(GeneralConfigurationIR) == 122, "GeneralConfigurationIR size does not match the register map");

    union HardwareInterfaceConfigurationHR {
      struct Common {
        uint16_t enabled:1;
        uint16_t :0;
        uint16_t clearStickyStatusBits;
      } common;
      static_assert(offsetof(Common, clearStickyStatusBits) == 2, "HardwareInterfaceConfigurationHR::Common::clearStickyStatusBits offset does not match the register map");
      static_assert(sizeof(Common) == 4, "HardwareInterfaceConfigurationHR::Common size does not match the register map");

      struct Uart {
        uint8_t common[sizeof(Common)];
        uint32_t baudRate;
        uint16_t dataBits;
        uint16_t parity;
        uint16_t stopBits;
        uint16_t flowControl;
      } uart;
      static_assert(offsetof(Uart, baudRate) == 4, "HardwareInterfaceConfigurationHR::Uart::baudRate offset does not match the register map");
      static_assert(offsetof(Uart, dataBits) == 8, "HardwareInterfaceConfigurationHR::Uart::dataBits offset does not match the register map");
      static_assert(offsetof(Uart, parity) == 10, "HardwareInterfaceConfigurationHR::Uart::parity offset does not match the register map");
      static_assert(offsetof(Uart, stopBits) == 12, "HardwareInterfaceConfigurationHR::Uart::stopBits offset does not match the register map");
      static_assert(offsetof(Uart, flowControl) == 14, "HardwareInterfaceConfigurationHR::Uart::flowControl offset does not match the register map");
      static_assert(sizeof(Uart) == 16, "HardwareInterfaceConfigurationHR::Uart size does not match the register map");

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
      static_assert(offsetof(NetworkInterface, clearStickyStatusBits) == 2, "HardwareInterfaceConfigurationHR::NetworkInterface::clearStickyStatusBits offset does not match the register map");
      static_assert(offsetof(NetworkInterface, ipV4Address) == 4, "HardwareInterfaceConfigurationHR::NetworkInterface::ipV4Address offset does not match the register map");
      static_assert(offsetof(NetworkInterface, ipV4Netmask) == 8, "HardwareInterfaceConfigurationHR::NetworkInterface::ipV4Netmask offset does not match the register map");
      static_assert(offsetof(NetworkInterface, ipV4Gateway) == 12, "HardwareInterfaceConfigurationHR::NetworkInterface::ipV4Gateway offset does not match the register map");
      static_assert(offsetof(NetworkInterface, ipV6GlobalAddress) == 16, "HardwareInterfaceConfigurationHR::NetworkInterface::ipV6GlobalAddress offset does not match the register map");
      static_assert(sizeof(NetworkInterface) == 32, "HardwareInterfaceConfigurationHR::NetworkInterface size does not match the register map");

      struct Ethernet {
        uint8_t networkInterface[sizeof(NetworkInterface)];
      } ethernet;
      static_assert(sizeof(Ethernet) == 32, "HardwareInterfaceConfigurationHR::Ethernet size does not match the register map");

      struct WiFi {
        uint8_t networkInterface[sizeof(NetworkInterface)];
        char ssid[maxWiFiSsidSize];
        char password[maxWiFiPasswordSize];
      } wifi;
      static_assert(offsetof(WiFi, ssid) == 32, "HardwareInterfaceConfigurationHR::WiFi::ssid offset does not match the register map");
      static_assert(offsetof(WiFi, password) == 64, "HardwareInterfaceConfigurationHR::WiFi::password offset does not match the register map");
      static_assert(sizeof(WiFi) == 128, "HardwareInterfaceConfigurationHR::WiFi size does not match the register map");

      struct UsbDeviceCdc {
        uint8_t common[sizeof(Common)];
      } usbDeviceCdc;
      static_assert(sizeof(UsbDeviceCdc) == 4, "HardwareInterfaceConfigurationHR::UsbDeviceCdc size does not match the register map");
    } hardwareInterfaceConfigurationHR;
    static_assert(sizeof(HardwareInterfaceConfigurationHR) == 128, "HardwareInterfaceConfigurationHR size does not match the register map");

    union HardwareInterfaceConfigurationIR {
      struct Common {
        uint16_t statusBits;
        uint16_t stickyStatusBits;
        uint16_t type;
        char name[maxNameSize];
      } common;
      static_assert(offsetof(Common, statusBits) == 0, "HardwareInterfaceConfigurationIR::Common::statusBits offset does not match the register map");
      static_assert(offsetof(Common, stickyStatusBits) == 2, "HardwareInterfaceConfigurationIR::Common::stickyStatusBits offset does not match the register map");
      static_assert(offsetof(Common, type) == 4, "HardwareInterfaceConfigurationIR::Common::type offset does not match the register map");
      static_assert(offsetof(Common, name) == 6, "HardwareInterfaceConfigurationIR::Common::name offset does not match the register map");
      static_assert(sizeof(Common) == 38, "HardwareInterfaceConfigurationIR::Common size does not match the register map");

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
      static_assert(offsetof(NetworkInterface, stickyStatusBits) == 2, "HardwareInterfaceConfigurationIR::NetworkInterface::stickyStatusBits offset does not match the register map");
      static_assert(offsetof(NetworkInterface, type) == 4, "HardwareInterfaceConfigurationIR::NetworkInterface::type offset does not match the register map");
      static_assert(offsetof(NetworkInterface, name) == 6, "HardwareInterfaceConfigurationIR::NetworkInterface::name offset does not match the register map");
      static_assert(offsetof(NetworkInterface, ipV6LinkLocalAddress) == 38, "HardwareInterfaceConfigurationIR::NetworkInterface::ipV6LinkLocalAddress offset does not match the register map");
      static_assert(sizeof(NetworkInterface) == 54, "HardwareInterfaceConfigurationIR::NetworkInterface size does not match the register map");

      struct Ethernet {
        uint8_t networkInterface[sizeof(NetworkInterface)];
      } ethernet;
      static_assert(sizeof(Ethernet) == 54, "HardwareInterfaceConfigurationIR::Ethernet size does not match the register map");

      struct WiFi {
        uint8_t networkInterface[sizeof(NetworkInterface)];
      } wifi;
      static_assert(sizeof(WiFi) == 54, "HardwareInterfaceConfigurationIR::WiFi size does not match the register map");

      struct UsbDeviceCdc {
        uint8_t common[sizeof(Common)];
      } usbDeviceCdc;
      static_assert(sizeof(UsbDeviceCdc) == 38, "HardwareInterfaceConfigurationIR::UsbDeviceCdc size does not match the register map");
    } hardwareInterfaceConfigurationIR;
    static_assert(sizeof(HardwareInterfaceConfigurationIR) == 54, "HardwareInterfaceConfigurationIR size does not match the register map");

    union ServerConfigurationHR {
      struct Common {
        uint16_t enabled:1;
        uint16_t :0;
        uint16_t clearStickyStatusBits;
      } common;
      static_assert(offsetof(Common, clearStickyStatusBits) == 2, "ServerConfigurationHR::Common::clearStickyStatusBits offset does not match the register map");
      static_assert(sizeof(Common) == 4, "ServerConfigurationHR::Common size does not match the register map");

      struct StreamServer {
        uint8_t common[sizeof(Common)];
      } streamServer;
      static_assert(sizeof(StreamServer) == 4, "ServerConfigurationHR::StreamServer size does not match the register map");

      struct NetworkServer {
        uint8_t common[sizeof(Common)];
        uint16_t port;
        uint16_t maxNumberOfClients;
      } networkServer;
      static_assert(offsetof(NetworkServer, port) == 4, "ServerConfigurationHR::NetworkServer::port offset does not match the register map");
      static_assert(offsetof(NetworkServer, maxNumberOfClients) == 6, "ServerConfigurationHR::NetworkServer::maxNumberOfClients offset does not match the register map");
      static_assert(sizeof(NetworkServer) == 8, "ServerConfigurationHR::NetworkServer size does not match the register map");

      struct ModbusServer {
        uint8_t common[sizeof(Common)];
        uint16_t protocol;
        uint16_t stationAddress;
      } modbusServer;
      static_assert(offsetof(ModbusServer, protocol) == 4, "ServerConfigurationHR::ModbusServer::protocol offset does not match the register map");
      static_assert(offsetof(ModbusServer, stationAddress) == 6, "ServerConfigurationHR::ModbusServer::stationAddress offset does not match the register map");
      static_assert(sizeof(ModbusServer) == 8, "ServerConfigurationHR::ModbusServer size does not match the register map");

      struct NetworkModbusServer {
        uint8_t modbusServer[sizeof(ModbusServer)];
        uint16_t port;
        uint16_t maxNumberOfClients;
      } networkModbusServer;
      static_assert(offsetof(NetworkModbusServer, port) == 8, "ServerConfigurationHR::NetworkModbusServer::port offset does not match the register map");
      static_assert(offsetof(NetworkModbusServer, maxNumberOfClients) == 10, "ServerConfigurationHR::NetworkModbusServer::maxNumberOfClients offset does not match the register map");
      static_assert(sizeof(NetworkModbusServer) == 12, "ServerConfigurationHR::NetworkModbusServer size does not match the register map");

      struct HttpServer {
        uint8_t networkServer[sizeof(NetworkServer)];
      } httpServer;
      static_assert(sizeof(HttpServer) == 8, "ServerConfigurationHR::HttpServer size does not match the register map");

      struct MdnsServer {
        uint8_t networkServer[sizeof(NetworkServer)];
      } mdnsServer;
      static_assert(sizeof(MdnsServer) == 8, "ServerConfigurationHR::MdnsServer size does not match the register map");
    } serverConfigurationHR;
    static_assert(sizeof(ServerConfigurationHR) == 12, "ServerConfigurationHR size does not match the register map");

    union ServerConfigurationIR {
      struct Common {
        uint16_t statusBits;
        uint16_t stickyStatusBits;
        uint16_t type;
        char name[maxNameSize];
      } common;
      static_assert(offsetof(Common, statusBits) == 0, "ServerConfigurationIR::Common::statusBits offset does not match the register map");
      static_assert(offsetof(Common, stickyStatusBits) == 2, "ServerConfigurationIR::Common::stickyStatusBits offset does not match the register map");
      static_assert(offsetof(Common, type) == 4, "ServerConfigurationIR::Common::type offset does not match the register map");
      static_assert(offsetof(Common, name) == 6, "ServerConfigurationIR::Common::name offset does not match the register map");
      static_assert(sizeof(Common) == 38, "ServerConfigurationIR::Common size does not match the register map");

      struct StreamServer {
        uint8_t common[sizeof(Common)];
      } streamServer;
      static_assert(sizeof(StreamServer) == 38, "ServerConfigurationIR::StreamServer size does not match the register map");

      struct NetworkServer {
        uint8_t common[sizeof(Common)];
      } networkServer;
      static_assert(sizeof(NetworkServer) == 38, "ServerConfigurationIR::NetworkServer size does not match the register map");

      struct ModbusServer {
        uint8_t common[sizeof(Common)];
      } modbusServer;
      static_assert(sizeof(ModbusServer) == 38, "ServerConfigurationIR::ModbusServer size does not match the register map");

      struct NetworkModbusServer {
        uint8_t common[sizeof(Common)];
      } networkModbusServer;
      static_assert(sizeof(NetworkModbusServer) == 38, "ServerConfigurationIR::NetworkModbusServer size does not match the register map");

      struct HttpServer {
        uint8_t networkServer[sizeof(NetworkServer)];
      } httpServer;
      static_assert(sizeof(HttpServer) == 38, "ServerConfigurationIR::HttpServer size does not match the register map");

      struct MdnsServer {
        uint8_t networkServer[sizeof(NetworkServer)];
      } mdnsServer;
      static_assert(sizeof(MdnsServer) == 38, "ServerConfigurationIR::MdnsServer size does not match the register map");
    } serverConfigurationIR;
    static_assert(sizeof(ServerConfigurationIR) == 38, "ServerConfigurationIR size does not match the register map");
  } memoryData;
  static_assert(sizeof(MemoryData) == registerMemoryAreaSize, "MemoryData size does not match the register map");
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