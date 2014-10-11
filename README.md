STM32_18B20_SIM900_GPRS
=======================
This is a c based code.

Use STM32(32bit MCU) to control 18b20 (temperature sensor), 5110 (LCD), SIM900 (GPRS).

Function.
18b20 check temperatue every 2 second.
When temperature > 30 C. Alter SMS will be sent to pre-defined cell phone.
If predfiened cell phone call SIM900, current temperature will be sent to the cell phone too.
