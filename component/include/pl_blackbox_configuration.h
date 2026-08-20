#pragma once

//==============================================================================

namespace PL {

//==============================================================================

/// @brief Base class for a BlackBox configuration
class BlackBoxConfiguration {
public:
  BlackBoxConfiguration() = default;
  virtual ~BlackBoxConfiguration() = default;
  BlackBoxConfiguration(const BlackBoxConfiguration&) = delete;
  BlackBoxConfiguration& operator=(const BlackBoxConfiguration&) = delete;

  /// @brief Loads the configuration
  virtual void Load() = 0;

  /// @brief Saves the configuration
  virtual void Save() = 0;

  /// @brief Erases the configuration
  virtual void Erase() = 0;
};

//==============================================================================

}