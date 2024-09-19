#include "pl_blackbox_usb_device_cdc_configuration.h"
#include "esp_check.h"

#if CONFIG_TINYUSB_CDC_ENABLED

//==============================================================================

namespace PL {

//==============================================================================

BlackBoxUsbDeviceCdcConfiguration::BlackBoxUsbDeviceCdcConfiguration(std::shared_ptr<UsbDeviceCdc> usbDeviceCdc, std::string nvsNamespaceName) :
    BlackBoxHardwareInterfaceConfiguration(usbDeviceCdc, nvsNamespaceName), usbDeviceCdc(usbDeviceCdc) { }

//==============================================================================

}

#endif