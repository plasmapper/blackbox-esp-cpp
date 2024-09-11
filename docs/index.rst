BlackBox Component
==================

.. |COMPONENT| replace:: blackbox

.. |ESP_IDF_VERSION| replace:: 5.3
   
.. |VERSION| replace:: 1.0.0

.. include:: ../../../installation.rst

.. include:: ../../../sdkconfig_network.rst

Features
--------

This is an implementation of `this specification <https://github.com/plasmapper/blackbox>`_ for ESP-IDF.

:cpp:class:`PL::BlackBox` is a base class for a `BlackBox <https://github.com/plasmapper/blackbox>`_ device.
The descendant class should override :cpp:func:`PL::BlackBox::GetFirmwareInfo`.

Configuration
^^^^^^^^^^^^^

:cpp:class:`PL::BlackBoxConfiguration` is a base class for configurations that can be saved, loaded and erased from NVS.
:cpp:class:`PL::BlackBoxConfigurationParameter` objects can be used to provide configuration parameter validation.

:cpp:func:`PL::BlackBox::AddConfiguration` adds a device configuration. A number of Add methods add hardware interface and server configurations.
By default all added hardware interfaces and servers are enabled and their parameters are fixed.
To make parameters configurable :cpp:func:`PL::BlackBoxConfigurationParameter::SetValueValidator`, :cpp:func:`PL::BlackBoxConfigurationParameter::SetValidValues`
or :cpp:func:`PL::BlackBoxConfigurationParameter::DisableValueValidation` should be used.

:cpp:func:`PL::BlackBox::LoadAllConfigurations` loads all configurations from NVS.
:cpp:func:`PL::BlackBox::SaveAllConfigurations` saves all configurations to NVS.
:cpp:func:`PL::BlackBox::EraseAllConfigurations` erases all configurations from NVS.

:cpp:func:`PL::BlackBox::ApplyHardwareInterfaceConfigurations` and :cpp:func:`PL::BlackBox::ApplyServerConfigurations`
apply the correspondent configurations to the hardware interfaces and servers.

Modbus Server
^^^^^^^^^^^^^

:cpp:class:`PL::BlackModbusServer` is a :cpp:class:`PL::ModbusServer` class extension that contains memory areas specified
in `BlackBox Modbus <https://github.com/plasmapper/blackbox/tree/main/modbus.md>`_ description.

Thread safety
-------------

Class method thread safety is implemented by having the :cpp:class:`PL::Lockable` as a base class and creating the class object lock guard at the beginning of the methods.

Examples
--------
| `BlackBox firmware and software examples <https://github.com/plasmapper/blackbox/tree/main/example>`_

API reference
-------------

.. toctree::
  
  api/types      
  api/blackbox_base
  api/blackbox_configuration_parameter
  api/blackbox_configuration
  api/blackbox_hardware_interface_configuration
  api/blackbox_uart_configuration
  api/blackbox_network_interface_configuration
  api/blackbox_ethernet_configuration
  api/blackbox_wifi_station_configuration
  api/blackbox_usb_device_cdc_configuration
  api/blackbox_server_configuration
  api/blackbox_stream_server_configuration
  api/blackbox_network_server_configuration
  api/blackbox_modbus_server_configuration
  api/blackbox_http_server_configuration
  api/blackbox_mdns_server_configuration
  api/blackbox_modbus_server
