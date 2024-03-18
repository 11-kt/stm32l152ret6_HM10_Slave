# HM-10 Slave mode. Based on stm32l152ret6

## Logic of messaging:
* Request for the current temperature is generated every 10 seconds
* Request for the current RSSI value is generated every 10 seconds
* Message is received from the connected device every 2 seconds
* Response is generated for each message received from the connected device
