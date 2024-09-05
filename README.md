# Smart Medibox

## Overview

The Smart Medibox is a project developed using an ESP32 microcontroller designed to remind users to take their medicine on time. It includes several functionalities such as setting time zones, configuring alarms, fetching time from an NTP server, and monitoring temperature and humidity levels.

## Features

1. **Time Zone Setting**: Set the time zone by specifying the UTC offset.
2. **Alarm Configuration**: Set up to three alarms. Alarm times can be adjusted and disabled.
3. **NTP Time Fetching**: Retrieve the current time from an NTP server and display it on the OLED screen.
4. **Alarm Indication**: Ring an alarm with a buzzer and LED indication when the set alarm times are reached.
5. **Alarm Cancellation**: Stop the alarm using a push button.
6. **Temperature and Humidity Monitoring**: Monitor temperature and humidity levels, with warnings displayed if they exceed or fall below healthy limits.
7. **OLED Display**: Display current time, alarm settings, and warning messages on an OLED screen.

## Components

- ESP32 Microcontroller
- OLED Display (SSD1306)
- DHT22 Temperature and Humidity Sensor
- Buzzer
- LEDs
- Push Buttons
- Various resistors and wiring

## Installation

1. **Hardware Setup**:
   - Connect the ESP32 to the OLED display, DHT22 sensor, buzzer, LEDs, and push buttons as specified in the code.

2. **Software Setup**:
   - Ensure you have the required libraries installed:
     - `Wire`
     - `Adafruit_GFX`
     - `Adafruit_SSD1306`
     - `DHTesp`
     - `WiFi`
   - Upload the provided code to the ESP32 using the Arduino IDE or another compatible development environment.

## Code Explanation

- **Libraries**: The code uses several libraries for OLED display, DHT sensor, and Wi-Fi connectivity.
- **Global Variables**: Variables for time, alarm settings, and hardware pin configurations.
- **Setup Function**: Initializes hardware components, connects to Wi-Fi, and synchronizes time with the NTP server.
- **Loop Function**: Regularly updates time, checks alarms, and monitors temperature and humidity.
- **Alarm Management**: Functions to set, check, and ring alarms.
- **Menu System**: Allows users to navigate through different settings and configure alarms.

## Usage

1. **Set Time Zone**: Navigate to the menu option to set the UTC offset.
2. **Configure Alarms**: Set or disable alarms through the menu system.
3. **Monitor Temperature and Humidity**: View warnings on the OLED display if levels go out of the healthy range.

## Notes

- The `Wokwi` simulation environment is used for testing and demonstrating the functionality of the Smart Medibox.
- Make sure the ESP32 is connected to the internet for NTP time fetching.


## Contact

For any questions or issues, please contact [Your Name](tharakadidd456@gmail.com).

