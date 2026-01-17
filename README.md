# Microcontroller-Based Smart Gate Automation System with Password Security

Project Overview The Problem: Manual gates are often inconvenient to operate and are frequently left open due to human error, compromising security. Furthermore, physical keys can be lost or copied.

The Solution: We have developed an automated solution that secures the entry point using digital authentication.

How it Works:

Secure Entry: The user is prompted to enter a 4-digit PIN on the keypad. The LCD provides visual feedback (masking the PIN for security).

Intelligent Control: Upon successful entry, the ATmega8 drives a servo motor to open the gate.

Smart Closing: Instead of using a simple timer, the system uses an Ultrasonic distance sensor to verify that the vehicle has left the gate area (distance > 17cm) before safely closing the gate. This prevents the gate from closing while a car is still passing through.

Entering pin code
<img width="1920" height="925" alt="Screenshot (12)" src="https://github.com/user-attachments/assets/dd575094-f902-4a6b-88cf-aca3d801894b" />

Gaining access and opening gate
<img width="1920" height="925" alt="Screenshot (13)" src="https://github.com/user-attachments/assets/55513cd6-90db-4e4b-b69f-a6338e169a2f" />

Current distance of object from the sensor
<img width="1920" height="925" alt="Screenshot (14)" src="https://github.com/user-attachments/assets/7cfa80e1-ca1a-4a61-8121-03bca33edf5e" />

Gate closed when the object was less than 17 cm
<img width="1920" height="925" alt="Screenshot (15)" src="https://github.com/user-attachments/assets/2596dcf5-9047-4727-9213-a3db3c19a0c6" />



# Implementation on Hardware and video recordings



https://github.com/user-attachments/assets/b6aa4d4a-9bc3-4c9f-b4f9-ce011eedd7a8





https://github.com/user-attachments/assets/64f7e95a-a6d6-4579-b71c-166419727426



https://github.com/user-attachments/assets/08c703ff-dc92-426a-910c-b47dc200122c


