#include "unity.h"
#include "nvs_flash.h"
#include "blackbox.h"
#include "blackbox_modbus.h"

//==============================================================================

extern "C" void app_main(void) {
  ESP_ERROR_CHECK (esp_event_loop_create_default());
  ESP_ERROR_CHECK (esp_netif_init());
  esp_err_t err = nvs_flash_init();
  if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK (nvs_flash_erase());
      err = nvs_flash_init();
  }
  ESP_ERROR_CHECK (err);

  nvs_handle_t handle;
  ESP_ERROR_CHECK (nvs_open (PL::BlackBox::hardwareInfoNvsNamespace.c_str(), NVS_READWRITE, &handle));
  ESP_ERROR_CHECK (nvs_set_str (handle, PL::BlackBox::hardwareInfoNameNvsKey.c_str(), BlackBox::hardwareInfo.name.c_str()));
  ESP_ERROR_CHECK (nvs_set_u16 (handle, PL::BlackBox::hardwareInfoMajorVersionNvsKey.c_str(), BlackBox::hardwareInfo.version.major));
  ESP_ERROR_CHECK (nvs_set_u16 (handle, PL::BlackBox::hardwareInfoMinorVersionNvsKey.c_str(), BlackBox::hardwareInfo.version.minor));
  ESP_ERROR_CHECK (nvs_set_u16 (handle, PL::BlackBox::hardwareInfoPatchVersionNvsKey.c_str(), BlackBox::hardwareInfo.version.patch));
  ESP_ERROR_CHECK (nvs_set_str (handle, PL::BlackBox::hardwareInfoUidNvsKey.c_str(), BlackBox::hardwareInfo.uid.c_str()));
  ESP_ERROR_CHECK (nvs_commit (handle));
  nvs_close (handle);

  UNITY_BEGIN();
  RUN_TEST (TestBlackBox);
  RUN_TEST (TestBlackBoxModbus);
  UNITY_END();
}