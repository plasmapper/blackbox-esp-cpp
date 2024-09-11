#include "unity.h"
#include "nvs_flash.h"
#include "pl_nvs.h"
#include "blackbox.h"
#include "blackbox_modbus.h"

//==============================================================================

extern "C" void app_main(void) {
  ESP_ERROR_CHECK(esp_event_loop_create_default());
  ESP_ERROR_CHECK(esp_netif_init());
  esp_err_t err = nvs_flash_init();
  if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      err = nvs_flash_init();
  }
  ESP_ERROR_CHECK(err);
  
  {
    PL::NvsNamespace nvsNamespace(PL::BlackBox::defaultHardwareInfoNvsNamespaceName, PL::NvsAccessMode::readWrite);
    ESP_ERROR_CHECK(nvsNamespace.Write(PL::BlackBox::hardwareInfoNameNvsKey, BlackBox::hardwareInfo.name));
    ESP_ERROR_CHECK(nvsNamespace.Write(PL::BlackBox::hardwareInfoMajorVersionNvsKey, BlackBox::hardwareInfo.version.major));
    ESP_ERROR_CHECK(nvsNamespace.Write(PL::BlackBox::hardwareInfoMinorVersionNvsKey, BlackBox::hardwareInfo.version.minor));
    ESP_ERROR_CHECK(nvsNamespace.Write(PL::BlackBox::hardwareInfoPatchVersionNvsKey, BlackBox::hardwareInfo.version.patch));
    ESP_ERROR_CHECK(nvsNamespace.Write(PL::BlackBox::hardwareInfoUidNvsKey, BlackBox::hardwareInfo.uid));
  }

  UNITY_BEGIN();
  RUN_TEST(TestBlackBox);
  RUN_TEST(TestBlackBoxModbus);
  UNITY_END();
}