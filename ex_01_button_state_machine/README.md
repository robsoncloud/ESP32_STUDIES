# ESP32 Button Control Project

This project demonstrates advanced button control using an ESP32 microcontroller. It showcases how to detect different types of button presses (single, double, and long press) and control an LED based on these inputs.

## Features

- Single press detection
- Double press detection
- Long press detection
- LED control based on button press type
- Debounce mechanism for reliable button press detection

## Hardware Requirements

- ESP32 development board
- Push button
- LED
- Resistors (as needed for your specific setup)

## Pin Configuration

- Button: GPIO 33
- LED: GPIO 23

## Software Dependencies

- ESP-IDF (Espressif IoT Development Framework)

## How It Works

The program continuously monitors the state of the button connected to GPIO 33. It can distinguish between three types of button presses:

1. Single Press: A quick press and release of the button.
2. Double Press: Two quick presses within 300ms.
3. Long Press: Holding the button for more than 1 second.

Based on the type of press detected, the program controls an LED connected to GPIO 23:

- Single Press: Turns the LED on
- Double Press: Turns the LED off
- Long Press: Toggles the LED state

## Building and Flashing

1. Set up your ESP-IDF environment.
2. Navigate to the project directory.
3. Build the project: `idf.py build`
4. Flash to your ESP32: `idf.py -p (PORT) flash`
   Replace (PORT) with your ESP32's port (e.g., COM3 on Windows or /dev/ttyUSB0 on Linux)

## TODO
- Implement debounce logic to prevent unexpected button press due to noise