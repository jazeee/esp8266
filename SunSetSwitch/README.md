## ESP-8266 SunSet Switch

This turns on at sunset and off later. Hard coded to RWC CA.

This works well on the LoL1n board, with ESP8266 chip.

Be aware that the board comes as 9600 baud, but this code then will upload at 
115200 * 2 baud. (230400)

In other words, the initial upload will take a while, but subsequent uploads are quick.

Use Arduino IDE plus ESP8266 packages.

