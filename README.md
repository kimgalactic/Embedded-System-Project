# Microcontroller-Based Smart Gate Automation System with Password Security

Project Overview The Problem: Manual gates are often inconvenient to operate and are frequently left open due to human error, compromising security. Furthermore, physical keys can be lost or copied.

The Solution: We have developed an automated solution that secures the entry point using digital authentication.

How it Works:

Secure Entry: The user is prompted to enter a 4-digit PIN on the keypad. The LCD provides visual feedback (masking the PIN for security).

Intelligent Control: Upon successful entry, the ATmega8 drives a servo motor to open the gate.

Smart Closing: Instead of using a simple timer, the system uses an Ultrasonic distance sensor to verify that the vehicle has left the gate area (distance > 17cm) before safely closing the gate. This prevents the gate from closing while a car is still passing through.

Entering pin code
<img width="1920" height="925" alt="Screenshot (12)" src="https://github.com/user-attachments/assets/44609a2d-abd8-4d18-8cf8-c19c18bb04b9" />

Gaining access and opening gate
<img width="1920" height="925" alt="Screenshot (13)" src="https://github.com/user-attachments/assets/59e495ef-9cf2-452e-a6fe-9889d11b2e96" />

Current distance of object from the sensor
<img width="1920" height="925" alt="Screenshot (14)" src="https://github.com/user-attachments/assets/a7781a57-7742-4294-831f-25ffec86fb19" />

Gate closed when the object was less than 17 cm
<img width="1920" height="925" alt="Screenshot (15)" src="https://github.com/user-attachments/assets/2290219e-6688-4401-ac97-732a26a4dc38" />



# Implementation on Hardware and video recordings



https://github.com/user-attachments/assets/b6aa4d4a-9bc3-4c9f-b4f9-ce011eedd7a8





https://github.com/user-attachments/assets/64f7e95a-a6d6-4579-b71c-166419727426


