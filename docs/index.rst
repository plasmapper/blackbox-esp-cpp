BlackBox Component
==================

.. |COMPONENT| replace:: blackbox

.. |VERSION| replace:: 1.0.0

.. include:: ../../../installation.rst

.. include:: ../../../sdkconfig_network.rst

Features
--------

This is an implementation of `this specification <https://github.com/plasmapper/blackbox>`_ for ESP-IDF.

1. :cpp:class:`PL::BlackBox` - a base class for a `BlackBox <https://github.com/plasmapper/blackbox>`_ device.
   The descendant class should override :cpp:func:`PL::BlackBox::GetFirmwareInfo`.
   :cpp:func:`PL::BlackBox::LoadConfiguration`, :cpp:func:`PL::BlackBox::SaveConfiguration` and :cpp:func:`PL::BlackBox::EraseSavedConfiguration`
   load, save and erase the device configuration (default: using the NVS).
   :cpp:func:`PL::BlackBox::GetHardwareInfo` and :cpp:func:`PL::BlackBox::GetFirmwareInfo` return the device information.
   :cpp:func:`PL::BlackBox::AddHardwareInterface` and :cpp:func:`PL::BlackBox::GetHardwareInterfaces` are used to add and configure hardware interfaces.
   :cpp:func:`PL::BlackBox::AddServer` and :cpp:func:`PL::BlackBox::GetServers` are used to add and configure servers.
2. :cpp:class:`PL::BlackModbusServer` - a base class for a `BlackBox Modbus <https://github.com/plasmapper/blackbox/tree/main/modbus.md>`_ server.

Examples
--------
| `BlackBox firmware and software examples <https://github.com/plasmapper/blackbox/tree/main/example>`_

API reference
-------------

.. toctree::
  
  api/types      
  api/blackbox_base
  api/blackbox_modbus_server