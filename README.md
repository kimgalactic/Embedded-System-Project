# 🚪 Smart Gate Automation System
### Microcontroller-Based Security & Access Control

![C](https://img.shields.io/badge/Language-Embedded%20C-blue)
![Hardware](https://img.shields.io/badge/Hardware-ATmega8%20|%20LCD%20|%20Servo-orange)
![Simulation](https://img.shields.io/badge/Simulation-Proteus-green)

## 📝 Overview
This project implements a secure, automated gate control system using an **ATmega8 microcontroller**. It addresses the security risks of manual gates by enforcing digital authentication (PIN) and ensures safety using ultrasonic proximity sensors for auto-closing functionality.

## 🛠️ Hardware & Tech Stack
* **Microcontroller:** ATmega8 (AVR Architecture)
* **Input:** 4x3 Keypad (for PIN entry)
* **Output:** 16x2 LCD Display (User feedback & masking)
* **Actuator:** Servo Motor (Gate mechanism simulation)
* **Sensor:** Ultrasonic Distance Sensor (HC-SR04 equivalent)
* **Simulation:** Proteus Design Suite

## ⚙️ How It Works
The system operates as a **Finite State Machine (FSM)** with three main stages:

### 1. Secure Authentication
* User enters a 4-digit PIN via the keypad.
* The system compares input against stored hash/value.
* **LCD Feedback:** Displays `*` for each digit to prevent "shoulder surfing."

### 2. Intelligent Actuation
* **If PIN is Correct:** The ATmega8 sends a PWM signal to the Servo Motor, rotating it 90° (Gate Open).
* **If PIN is Incorrect:** LCD displays "Access Denied" and resets.

### 3. Safety Auto-Closing
* Instead of a simple timer, the system uses an **Ultrasonic Sensor**.
* It continuously monitors the distance of the vehicle.
* **Logic:** The gate remains open until the vehicle distance > 17cm (indicating the car has fully passed).
* Once cleared, the Servo rotates back to 0° (Gate Closed).

## 📂 File Structure
* `main.c`: The core C code containing the keypad scanning logic, PWM servo control, and sensor interrupt handling.
* `smart_gate_simulation.pdsprj`: The Proteus project file including the full schematic and wiring.

## 🚀 Getting Started
1.  Clone the repository.
2.  Open the `.pdsprj` file in Proteus 8 (or later).
3.  Compile `main.c` using AVR-GCC or Microchip Studio to generate the `.hex` file.
4.  Load the `.hex` file into the ATmega8 component in Proteus.
5.  Run the simulation.

---
*Created by [Abdullakim](https://github.com/Abdullakim1)*
