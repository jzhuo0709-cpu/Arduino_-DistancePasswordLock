# Practical assessment — distance password lock

**Design brief:** Three measured distances form a three-number password. The HC-SR04 ultrasonic sensor converts echo time into centimetres. Press A to capture the current distance, B to undo one entry, and C to verify. The OLED displays the live measurement and entered values. When the sequence is approximately 15, 25 and 35 cm, the Arduino rotates the servo latch for five seconds.

**Sensor-to-actuator rule:** For each of three positions, `absolute(measured distance − target distance) <= 3 cm`. All three comparisons must pass before the servo moves from 0° to 90°. No echo means no number is captured. Three incorrect attempts cause a 30-second pause.

**Novelty:** The password is entered by moving an object through space. The measured sensor values are the password numbers themselves, replacing the usual numeric keypad.

**One-minute live demonstration:**

1. Move a flat card in front of the ultrasonic sensor and show the live centimetre reading on the OLED.
2. Hold it near 15, 25 and 35 cm in turn, pressing A once at each position. Explain that each press takes three readings and records the middle value.
3. Press C. The OLED says `OPEN 5s`, and the servo opens and then closes automatically.
4. Record 15, 15 and 15 cm, then press C. The OLED says `WRONG`; the servo stays locked. Press B to demonstrate undo if time allows.

**Code quality:** The 180-line sketch uses named constants, small functions, comments and a name/date/function header. Fill in the student's name before submission.

**Practical limit:** Distance measurement can be affected by target angle and surface. Use a flat card held still for the live demonstration. This is an educational prototype, not a secure access-control device.
