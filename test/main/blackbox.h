#include "pl_blackbox.h"

//==============================================================================

class BlackBox : public PL::BlackBox {
public:
  static const PL::BlackBoxHardwareInfo hardwareInfo;
  static const PL::BlackBoxFirmwareInfo firmwareInfo;

  PL::BlackBoxFirmwareInfo GetFirmwareInfo() override; 
};

//==============================================================================

extern std::shared_ptr<PL::Uart> uart;
extern std::shared_ptr<PL::EspWiFiStation> wifi;

extern std::shared_ptr<PL::ModbusServer> uartModbusServer;
extern std::shared_ptr<PL::ModbusServer> networkModbusServer;

//==============================================================================

void TestBlackBox();