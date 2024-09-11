#pragma once
#include "pl_blackbox_hardware_interface_configuration.h"
#include "pl_uart.h"

//==============================================================================

namespace PL {

//==============================================================================

/// @brief BlackBox UART configuration
class BlackBoxUartConfiguration : public BlackBoxHardwareInterfaceConfiguration {
public:
  /// @brief baud rate parameter NVS key
  static const std::string baudRateNvsKey;
  /// @brief data bits parameter NVS key
  static const std::string dataBitsNvsKey;
  /// @brief parity parameter NVS key
  static const std::string parityNvsKey;
  /// @brief stop bits parameter NVS key
  static const std::string stopBitsNvsKey;
  /// @brief flow control parameter NVS key
  static const std::string flowControlNvsKey;

  /// @brief Creates a BlackBox UART configuration
  /// @param uart UART
  /// @param nvsNamespaceName NVS namespace name
  BlackBoxUartConfiguration(std::shared_ptr<Uart> uart, std::string nvsNamespaceName);

  /// @brief baud rate parameter
  BlackBoxConfigurationParameter<uint32_t> baudRate;

  /// @brief data bits parameter
  BlackBoxConfigurationParameter<uint16_t> dataBits;

  /// @brief parity parameter
  BlackBoxConfigurationParameter<UartParity> parity;

  /// @brief stop bits parameter
  BlackBoxConfigurationParameter<UartStopBits> stopBits;

  /// @brief flow control parameter
  BlackBoxConfigurationParameter<UartFlowControl> flowControl;
  
  void Load() override;
  void Save() override;
  void Apply() override;

private:
  std::shared_ptr<Uart> uart;
};

//==============================================================================

}