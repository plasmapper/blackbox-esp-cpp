# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]
### Changed
- Lock timeout handling.
- Static const members to constexpr.
- BlackBoxModbusServerConfiguration port and maxNumberOfClients initialization to avoid SetValidValues/SetValue detour.
- BlackBoxConfigurationParameter rejected value log message to hint at missing validator configuration.
- BlackBoxServerConfiguration to implement Lockable, matching BlackBoxHardwareInterfaceConfiguration.
- BlackBoxHardwareInterfaceConfiguration and BlackBoxServerConfiguration mutex member to private.

### Removed
- Unused hardware interface and server members from BlackBoxEthernetConfiguration, BlackBoxUsbDeviceCdcConfiguration, BlackBoxStreamServerConfiguration, BlackBoxHttpServerConfiguration and BlackBoxMdnsServerConfiguration.

### Fixed
- BlackBoxHardwareInterfaceConfiguration unlock log tag.
- Hardware info cache invalidation on NVS namespace change.
- Missing mutex locking in BlackBox configuration adder methods.
- BlackBoxNetworkServerConfiguration maxNumberOfClients NVS type mismatch on load.
- BlackBoxModbusServer Wi-Fi password validity loop condition.
- Missing mutex locking in BlackBoxConfigurationParameter::DisableValueValidation.
- BlackBoxHardwareInterfaceConfiguration and BlackBoxServerConfiguration copy constructor deletion targeting the wrong type.

## [2.0.2] - 2024-09-26
### Fixed
- BlackBoxConfigurationParameter value comparison before validation.

## [2.0.1] - 2024-09-19
### Fixed
- USB preprocessor directives.

## [2.0.0] - 2024-09-19
### Added
- USB, HTTP server and mDNS server types.
- Configuration and configuration parameter classes.

### Changed
- Separated configuration from actual interface and server parameters.
- UART server to stream server.

## [1.0.0] - 2024-06-12
Initial release.