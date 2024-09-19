#pragma once
#include "pl_blackbox_hardware_interface_configuration.h"

#if CONFIG_TINYUSB_CDC_ENABLED

#include "pl_usb.h"

//==============================================================================

namespace PL {

//==============================================================================

/// @brief BlackBox USB device CDC configuration
class BlackBoxUsbDeviceCdcConfiguration : public BlackBoxHardwareInterfaceConfiguration {
public:
  /// @brief Creates a BlackBox USB device CDC configuration
  /// @param usbDeviceCdc USB device CDC
  /// @param nvsNamespaceName NVS namespace name
  BlackBoxUsbDeviceCdcConfiguration(std::shared_ptr<UsbDeviceCdc> usbDeviceCdc, std::string nvsNamespaceName);

private:
  std::shared_ptr<UsbDeviceCdc> usbDeviceCdc;
};

//==============================================================================

}

#endif