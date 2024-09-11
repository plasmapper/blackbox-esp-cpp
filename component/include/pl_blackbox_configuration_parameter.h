#pragma once
#include "pl_common.h"
#include <functional>
#include "esp_check.h"

//==============================================================================

static const char* CONFIG_PARAM_TAG = "pl_blackbox_configuration_parameter";

//==============================================================================

namespace PL {

//==============================================================================

/// @brief BlackBox configuration parameter
template <class T>
class BlackBoxConfigurationParameter : public Lockable {
public:
  /// @brief Creates a BlackBox configuration parameter
  BlackBoxConfigurationParameter(T value) {
    this->value = value;
    this->valueValidator = [](T value) { return false; };
  }
  ~BlackBoxConfigurationParameter() {}
  BlackBoxConfigurationParameter(const BlackBoxConfigurationParameter&) = delete;
  BlackBoxConfigurationParameter& operator=(const BlackBoxConfigurationParameter&) = delete;

  esp_err_t Lock(TickType_t timeout = portMAX_DELAY) override {
    ESP_RETURN_ON_ERROR(mutex.Lock(timeout), CONFIG_PARAM_TAG, "mutex lock failed");
    return ESP_OK;
  }
  
  esp_err_t Unlock() override {
    ESP_RETURN_ON_ERROR(mutex.Unlock(), CONFIG_PARAM_TAG, "mutex unlock failed");
    return ESP_OK;
  }

  /// @brief Gets the parameter value
  T GetValue() {
    LockGuard lg(*this);
    return value;
  }
  
  /// @brief Sets the parameter value
  /// @param value parameter value
  /// @return error code
  esp_err_t SetValue(T value) {
    LockGuard lg(*this);
    ESP_RETURN_ON_FALSE(valueValidator(value), ESP_ERR_INVALID_ARG, CONFIG_PARAM_TAG, "parameter value validation failed");
    this->value = value;
    return ESP_OK;
  }

  /// @brief Sets the parameter value validator
  /// @param valueValidator parameter value validator
  void SetValueValidator(std::function<bool(T)> valueValidator) {
    LockGuard lg(*this);
    this->valueValidator = valueValidator;
  }

  /// @brief Sets the valid parameter values
  /// @param value valid parameter values
  void SetValidValues(std::vector<T> validValues) {
    LockGuard lg(*this);
    this->valueValidator = [validValues](T value){ return std::find(validValues.begin(), validValues.end(), value) != validValues.end(); };
  }

  /// @brief Disables the parameter value validation
  void DisableValueValidation() {
    this->valueValidator = [](T value) { return true; };
  }

private:
  Mutex mutex;
  T value;
  std::function<bool(T)> valueValidator;
};

//==============================================================================

}