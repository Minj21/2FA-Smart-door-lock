# 2FA Smart Door Lock

A two-factor smart door lock prototype using an Arduino UNO keypad passcode as the first factor and ESP32-CAM face recognition as the second factor.

## Project Overview

This project combines keypad authentication with camera-based face verification. The Arduino UNO controls the keypad, relay, buzzer, and status LEDs. The ESP32-CAM hosts the camera web server and sends a face-detected signal to the Arduino. The door unlocks only after the correct passcode is entered and a fresh face signal is received.

## Features

- Keypad passcode authentication
- ESP32-CAM camera web server
- Face-recognition second factor
- Relay-controlled door lock output
- Red and green LED status feedback
- Buzzer feedback for keypresses and access result
- Automatic relock after unlock timeout
- Manual relay test using the D key

## Hardware Used

- Arduino UNO
- ESP32-CAM AI Thinker module
- 4x4 matrix keypad
- Relay module
- Electric door lock or solenoid lock
- Active-low buzzer
- Red LED and green LED
- Jumper wires and breadboard or soldered circuit
- Suitable external power supply for the lock and relay

## Repository Structure

- Arduino_UNO_2FA/Arduino_UNO_2FA.ino - Arduino keypad, relay, buzzer, LED, and 2FA logic
- ESP32_CameraWebServer/CameraWebServer.ino - ESP32-CAM Wi-Fi and camera server setup
- ESP32_CameraWebServer/app_httpd.cpp - Camera web server and face recognition handlers
- ESP32_CameraWebServer/camera_index.h - Web interface assets
- ESP32_CameraWebServer/camera_pins.h - Supported ESP32-CAM board pin definitions

## Arduino UNO Wiring

- ESP32-CAM face signal to Arduino A0
- Relay IN to Arduino D8
- Buzzer signal to Arduino D12
- Red LED to Arduino A4
- Green LED to Arduino A3
- Keypad rows to D3, D4, D5, D6
- Keypad columns to D7, D9, D10, D11

## Setup

1. Open Arduino_UNO_2FA/Arduino_UNO_2FA.ino in the Arduino IDE.
2. Install the Keypad library if it is not already installed.
3. Upload the Arduino sketch to the Arduino UNO.
4. Open ESP32_CameraWebServer/CameraWebServer.ino in the Arduino IDE.
5. Select the AI Thinker ESP32-CAM board or adjust the camera model in the sketch.
6. Replace YOUR_WIFI_SSID and YOUR_WIFI_PASSWORD with your own Wi-Fi details.
7. Upload the ESP32-CAM sketch.
8. Open http://doorlockcam.local/ or use the IP address printed in Serial Monitor.
9. Enroll the face in the ESP32-CAM web interface before testing the full lock flow.

## Normal Operation

1. Enter the passcode 1234 on the keypad and press #.
2. The Arduino waits for a fresh face signal from the ESP32-CAM.
3. Show the enrolled face to the ESP32-CAM.
4. If both factors pass, the relay unlocks the door for 5 seconds.
5. The relay turns off automatically and the door locks again.

## Test Mode

Press D on the keypad to trigger a manual relay unlock test. This is useful while wiring and debugging the relay, but should be removed or disabled for a real deployment.

## Security Notes

- Wi-Fi credentials are intentionally replaced with placeholders before publishing.
- Change the default passcode before using this in a real environment.
- Do not rely on this prototype as a production security system without stronger authentication, tamper protection, and fail-safe power design.
- Keep high-current lock wiring separate from the Arduino logic pins.

## Status

Final working prototype code prepared for GitHub upload.
