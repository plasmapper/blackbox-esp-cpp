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
class BlackBoxConfigurationParameter {
public:
  /// @brief Creates a BlackBox configuration parameter
  BlackBoxConfigurationParameter(T value) {
    this->value = value;
    this->valueValidator = [](T value) { return false; };
  }
  ~BlackBoxConfigurationParameter() {}
  BlackBoxConfigurationParameter(const BlackBoxConfigurationParameter&) = delete;
  BlackBoxConfigurationParameter& operator=(const BlackBoxConfigurationParameter&) = delete;

  /// @brief Gets the parameter value
  T GetValue() {
    LockGuard lg(mutex);
    return value;
  }
  
  /// @brief Sets the parameter value
  /// @param value parameter value
  /// @return error code
  esp_err_t SetValue(T value) {
    LockGuard lg(mutex);
    ESP_RETURN_ON_FALSE(valueValidator(value), ESP_ERR_INVALID_ARG, CONFIG_PARAM_TAG, "parameter value validation failed");
    this->value = value;
    return ESP_OK;
  }

  /// @brief Sets the parameter value validator
  /// @param valueValidator parameter value validator
  void SetValueValidator(std::function<bool(T)> valueValidator) {
    LockGuard lg(mutex);
    this->valueValidator = valueValidator;
  }

  /// @brief Sets the valid parameter values
  /// @param validValues valid parameter values
  void SetValidValues(std::vector<T> validValues) {
    LockGuard lg(mutex);
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
