#include "pl_blackbox_uart_configuration.h"
#include "esp_check.h"

//==============================================================================

namespace PL {

//==============================================================================

const std::string BlackBoxUartConfiguration::baudRateNvsKey = "baudRate";
const std::string BlackBoxUartConfiguration::dataBitsNvsKey = "dataBits";
const std::string BlackBoxUartConfiguration::parityNvsKey = "parity";
const std::string BlackBoxUartConfiguration::stopBitsNvsKey = "stopBits";
const std::string BlackBoxUartConfiguration::flowControlNvsKey = "flowControl";

//==============================================================================

BlackBoxUartConfiguration::BlackBoxUartConfiguration(std::shared_ptr<Uart> uart, std::string nvsNamespaceName) :
    BlackBoxHardwareInterfaceConfiguration(uart, nvsNamespaceName),
    baudRate(uart->GetBaudRate()), dataBits(uart->GetDataBits()), parity(uart->GetParity()), stopBits(uart->GetStopBits()), flowControl(uart->GetFlowControl()),
    uart(uart) { }


//==============================================================================

void BlackBoxUartConfiguration::Load() {
  LockGuard lg(*this);
  NvsNamespace nvsNamespace(nvsNamespaceName, NvsAccessMode::readOnly);
  uint8_t u8Value;
  uint16_t u16Value;
  uint32_t u32Value;

  if (nvsNamespace.Read(baudRateNvsKey, u32Value) == ESP_OK)
    baudRate.SetValue(u32Value);
  if (nvsNamespace.Read(dataBitsNvsKey, u16Value) == ESP_OK)
    dataBits.SetValue(u16Value);
  if (nvsNamespace.Read(parityNvsKey, u8Value) == ESP_OK)
    parity.SetValue((UartParity)u8Value);
  if (nvsNamespace.Read(stopBitsNvsKey, u8Value) == ESP_OK)
    stopBits.SetValue((UartStopBits)u8Value);
  if (nvsNamespace.Read(flowControlNvsKey, u8Value) == ESP_OK)
    flowControl.SetValue((UartFlowControl)u8Value);

  BlackBoxHardwareInterfaceConfiguration::Load();
}

//==============================================================================

void BlackBoxUartConfiguration::Save() {
  LockGuard lg(*this);
  NvsNamespace nvsNamespace(nvsNamespaceName, NvsAccessMode::readWrite);

  nvsNamespace.Write(baudRateNvsKey, baudRate.GetValue());
  nvsNamespace.Write(dataBitsNvsKey, dataBits.GetValue());
  nvsNamespace.Write(parityNvsKey, (uint8_t)parity.GetValue());
  nvsNamespace.Write(stopBitsNvsKey, (uint8_t)stopBits.GetValue());
  nvsNamespace.Write(flowControlNvsKey, (uint8_t)flowControl.GetValue());

  return BlackBoxHardwareInterfaceConfiguration::Save();
}

//==============================================================================

void BlackBoxUartConfiguration::Apply() {
  LockGuard lg(*this, *uart);
  
  uart->SetBaudRate(baudRate.GetValue());
  uart->SetDataBits(dataBits.GetValue());
  uart->SetParity(parity.GetValue());
  uart->SetStopBits(stopBits.GetValue());
  uart->SetFlowControl(flowControl.GetValue());
  
  BlackBoxHardwareInterfaceConfiguration::Apply();
}

//==============================================================================

}