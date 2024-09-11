#include "pl_blackbox_wifi_station_configuration.h"
#include "esp_check.h"

//==============================================================================

namespace PL {

//==============================================================================

const std::string BlackBoxWiFiStationConfiguration::ssidNvsKey = "wifiSsid";
const std::string BlackBoxWiFiStationConfiguration::passwordNvsKey = "wifiPass";

//==============================================================================

BlackBoxWiFiStationConfiguration::BlackBoxWiFiStationConfiguration(std::shared_ptr<WiFiStation> wifiStation, std::string nvsNamespaceName) :
    BlackBoxNetworkInterfaceConfiguration(wifiStation, nvsNamespaceName), ssid(wifiStation->GetSsid()), password(wifiStation->GetPassword()), wifiStation(wifiStation) { }

//==============================================================================

void BlackBoxWiFiStationConfiguration::Load() {
  LockGuard lg(*this);
  NvsNamespace nvsNamespace(nvsNamespaceName, NvsAccessMode::readOnly);
  std::string stringValue;

  if (nvsNamespace.Read(ssidNvsKey, stringValue) == ESP_OK)
    ssid.SetValue(stringValue);
  if (nvsNamespace.Read(passwordNvsKey, stringValue) == ESP_OK)
    password.SetValue(stringValue);

  BlackBoxNetworkInterfaceConfiguration::Load();
}

//==============================================================================

void BlackBoxWiFiStationConfiguration::Save() {
  LockGuard lg(*this);
  NvsNamespace nvsNamespace(nvsNamespaceName, NvsAccessMode::readWrite);

  nvsNamespace.Write(ssidNvsKey, ssid.GetValue());
  nvsNamespace.Write(passwordNvsKey, password.GetValue());
  
  BlackBoxNetworkInterfaceConfiguration::Save();
}

//==============================================================================

void BlackBoxWiFiStationConfiguration::Apply() {
  LockGuard lg(*this, *wifiStation);
  
  wifiStation->SetSsid(ssid.GetValue());
  wifiStation->SetPassword(password.GetValue());

  BlackBoxNetworkInterfaceConfiguration::Apply();
}

//==============================================================================

}