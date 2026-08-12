#pragma once
#include "pl_blackbox_configuration.h"
#include "pl_blackbox_configuration_parameter.h"
#include "pl_nvs.h"

//==============================================================================

namespace PL {

//==============================================================================

/// @brief BlackBox server configuration
class BlackBoxServerConfiguration : public Lockable, public BlackBoxConfiguration {
public:
  /// @brief enabled parameter NVS key
  static const std::string enabledNvsKey;

  /// @brief Creates a BlackBox server configuration
  /// @param server server
  /// @param nvsNamespaceName NVS namespace name
  BlackBoxServerConfiguration(std::shared_ptr<Server> server, std::string nvsNamespaceName);
  ~BlackBoxServerConfiguration() {}
  BlackBoxServerConfiguration(const BlackBoxServerConfiguration&) = delete;
  BlackBoxServerConfiguration& operator=(const BlackBoxServerConfiguration&) = delete;

  /// @brief enabled parameter
  BlackBoxConfigurationParameter<bool> enabled = BlackBoxConfigurationParameter<bool>(true);

  /// @brief Gets the server
  /// @return server
  std::shared_ptr<Server> GetServer();

  esp_err_t Lock(TickType_t timeout = portMAX_DELAY) override;
  esp_err_t Unlock() override;
  void Load() override;
  void Save() override;
  void Erase() override;

  /// @brief Applies the configuration to the server
  virtual void Apply();

protected:
  std::string nvsNamespaceName;

private:
  Mutex mutex;
  std::shared_ptr<Server> server;
};

//==============================================================================

}