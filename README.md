Cubo Project
Overview
Cubo is an autonomous, interactive companion robot designed for modern homes, built on a Raspberry Pi Zero 2 W running Raspberry Pi OS Bullseye Lite. Developed by Leena Kashyap and Suryasaradhi Balarkan, Cubo combines affordable hardware with open-source AI to provide video streaming, expressive display animations (e.g., robotic eyes), audio interaction, and sensor-based controls. It serves as a home automation hub, elderly care companion, educational assistant, and mobile surveillance system, requiring minimal user management. Cubo responds to touch and voice inputs with emotional expressions, making it engaging and relatable.
Purpose
Cubo addresses key challenges in smart home technology:

Limited Interaction: Unlike static assistants (e.g., Alexa, Google Home), Cubo offers emotional and physical interactions via animations and mobility.
Elderly Care: Provides companionship, reminders, and emergency support for seniors.
Affordability: Uses low-cost components like the Raspberry Pi Zero 2 W to make robotics accessible.
Unified Automation: Acts as a central hub for smart devices, schedules, and communication.

Features

Video Streaming: Streams 1920x1080 video at 30 FPS using libcamera-vid and ffmpeg over UDP for remote viewing, face tracking, and object detection.
Display: Renders expressive "RoboEyes" animations on a 3.5" SPI LCD via framebuffer (/dev/fb0).
Audio: Supports voice input (INMP411 microphone with Whisper/VOSK) and output (MAX98357A speaker via I2S).
Mobility: Uses 28BYJ-48 stepper motors for head or wheel movement, controlled by an Arduino Micro.
Sensors: Integrates a QMC5883P compass for orientation, VL53L3CX TOF sensor for distance detection, and touch sensors for gesture interaction (e.g., petting).
Automation: Manages smart devices and schedules via a Flask server, with self-charging and connectivity features.

Hardware Components



Component
Function



Raspberry Pi Zero 2 W
Main processing for AI, camera, audio, UI, and control logic


Arduino Micro
Real-time control for motors and sensors


OV5640 Camera
Video capture for streaming and detection


INMP411 Microphone
Voice input for speech recognition


3.5" SPI LCD
Displays UI (e.g., robotic eyes, alerts)


Touch Sensor
Detects user interactions (petting, taps)


MAX98357A Speaker
Audio output for responses and alerts


28BYJ-48 Stepper Motors
Drives head or wheel movement


ULN2003 Driver Board
Powers and controls stepper motors


VL53L3CX TOF Sensor
Measures distance for obstacle detection


QMC5883P Compass (HW-246 GY-271)
Detects orientation for navigation


3.7V Li-ion Battery
Main power source


TP4056 Charging Module
Manages battery charging and protection


5V Boost Converter
Provides stable 5V for components


Flask PC (via WiFi)
Remote server for web interface and communication


Software Stack

Raspberry Pi:
OS: Raspberry Pi OS Bullseye Lite.
Languages: Python (AI, backend), C++ (sensor control).
Frameworks:
ROS: Sensor and navigation control.
Flask: Web interface and remote control.
ORBSLAM: Planned for navigation (future).


AI:
Whisper/VOSK: Speech recognition for voice commands.
Ollama: Prompt-based AI (planned ChatGPT integration).


Libraries: numpy, pyserial, picamera, libcamera-apps, ffmpeg, U8glib, PIGPIO.
Networking: WiFi for streaming and server communication.


Arduino:
Libraries: TinyStepper_28BYJ_48, ezButton, Wire.
Custom Code: Manages compass, motors, and touch sensors via UART.


Design Tools:
Fusion 360: CAD modeling.
KiCad: PCB design.
Flutter: Minimal Android app.



Setup Instructions
1. Raspberry Pi Configuration

OS: Install Raspberry Pi OS Bullseye Lite.
Network:
Scan for Pi:nmap -p 22 --open -sV <subnet>.0/24
ping <scanned-ip>


Enable mDNS:sudo apt update
sudo apt install avahi-daemon
sudo systemctl enable avahi-daemon
sudo systemctl start avahi-daemon
sudo raspi-config # Set hostname
ping <hostname>.local


Install Bonjour on Windows for mDNS resolution.



2. Camera and Streaming

Enable camera in /boot/config.txt and raspi-config.
Install tools:sudo apt install ffmpeg -y
sudo apt install libcamera-apps
sudo apt-get install tio


Stream video:libcamera-vid -t 0 -g 10 --bitrate 4500000 --inline --width 1920 --height 1080 --framerate 30 --rotation 180 --codec libav --libav-format h264 --av-sync 200000 -n -o udp://<receiver-ip>:1234


Play stream:ffplay -fflags nobuffer -flags low_delay -framedrop udp://<receiver-ip>:1234


Autostart: Follow WiFi Camera Guide.

3. Audio Setup

Configure I2S in /boot/config.txt:dtparam=i2s=on
dtoverlay=googlevoicehat-soundcard


Test audio:aplay -l
speaker-test
arecord -d 5 test.wav
aplay test.wav



4. Display Setup

Connect 3.5" SPI LCD (guide).
Render to framebuffer:import numpy as np
h, w, c = 1024, 1280, 4
fb = np.memmap('/dev/fb0', dtype='uint8', mode='w+', shape=(h,w,c))
fb[:] = [255,0,0,255] # Blue screen



5. Sensor and Motor Control

Arduino:
Connect stepper motors, buttons (SW1–SW4), and QMC5883P compass.
Upload Arduino code for motor, sensor, and UART handling.


Serial Communication:
Uses /dev/ttyS0 or /dev/ttyUSB0.
Commands: sm;s1:<value>;s2:<value> (motors), swa (buttons), cmps (compass).


Gestures:
Buttons increment activityCount (threshold: 20 in 5 seconds) to trigger actions (e.g., HAPPY mood).



6. RoboEyes Animation

Displays expressive eyes on LCD:
Configurable: size, spacing, mood (e.g., HAPPY, DEFAULT), animations (blink, wink).
Features: Curiosity mode, flickering, prebuilt animations.
Updates at 7 FPS:robo = RoboEyes(lcd, 480, 320, frame_rate=7, on_show=robo_show)
robo.set_auto_blinker(True, 4, 2)
robo.set_idle_mode(True, 5, 2)
robo.mood = DEFAULT
robo.curious = True





Implementation

Design: CAD modeled in Fusion 360 with multiple revisions.
Assembly: Integrated hardware, addressing wiring complexity.
Software:
Python: Camera, mic, LCD, AI (Whisper/VOSK, Ollama).
Arduino: Motors, sensors, UART bridge.
Flask: Remote control and streaming.


Testing: Verified streaming, inputs, and animations.

Setbacks and Learnings

Challenges:
Component sourcing delays; built custom libraries.
Boost converter overheating and wiring issues.
I2C/SPI conflicts and logic level mismatches.
Whisper too slow; switched to VOSK.
Power consumption caused resets.


Learnings:
Modularize subsystems for testing.
Prioritize power planning.
Built custom level shifters.



Use Cases

Home Automation: Controls smart devices and schedules.
Elderly Care: Offers companionship, reminders, and alerts.
Education: Engages children with interactive learning.
Surveillance: Mobile camera for security.
Ambient: Plays music, displays calendar events.

Future Scope

Implement ORBSLAM for navigation.
Develop IoT backend for device integration.
Upgrade to Whisper or ChatGPT for AI.
Integrate apps (Skype, WhatsApp).
Connect to Google IoT for alarms and calendars.
Add native language speech recognition.
Design PCB for compact electronics.
Refine mechanical design and use faster motors.
Enhance gesture detection.

Repository

GitHub - thesunRider/cuBo

License
MIT License