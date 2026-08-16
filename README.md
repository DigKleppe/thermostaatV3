# Thermostat using Waveshare ESP32-s3-touch-lcd-4 and LVGL 8
uses SCD30 sensor 
idf 6.02

### Dependencies

| Component | Version |
| --- | --- |
| `waveshare/esp32_s3_touch_lcd_4` | `3.0.0` |
| `lvgl/lvgl` | `8.4.0` |


uses RS485 outputsconnected tp optocouplers for heating and cooling valve
Modification board (revision 4): T6 and U6 removed , pin 2/3 U7 connected to pin4 ESP32 module
Pads U6: 1 - 7 and 4 -7 connected for aux i2c for SCD30 module. Problems using board i2c combined with touchscreen
CAN L SDA CAN H SCL


