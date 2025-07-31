
# Secret Knock Drawer Lock 🔐

This project implements a **Secret Knock-Activated Drawer Lock** using an STM32 microcontroller. The system detects a predefined knock pattern by the user and activates a solenoid to unlock a drawer only when the correct pattern is recognized.

---

## 🔍 Project Overview

This embedded system was developed for educational and practical purposes. It offers a cost-effective solution to physical security using:
- **Piezoelectric sensor** for knock detection
- **STM32F100RB** MCU for signal processing and pattern recognition
- **Solenoid lock** controlled via transistor


---

## ⚙️ Components Used

| Component | Description |
|----------|-------------|
| STM32F100RB | Main controller (via STM32CubeIDE) |
| Piezoelectric Sensor | Converts physical knocks into analog signals |
| Solenoid Lock (12V) | Electromechanical actuator for the drawer |
| TIP120 Transistor | Controls solenoid from STM32 output |
| 1N4001 Diode | Protects against back EMF |
| Resistors (1MΩ, 10KΩ, 2.2KΩ) | Signal filtering and biasing |
| Tactile Button | Reset or program new pattern |
| AMS1117-3.3V Regulator | Provides 3.3V power to STM32 |
| 12V Power Supply | Powers solenoid and regulator |
| Perforated Board | For permanent circuit setup |

---

## 🛠️ How It Works

1. User knocks on the surface with a predefined pattern.
2. Piezo sensor reads the signal and sends it to the STM32 ADC.
3. STM32 processes timing intervals between knocks.
4. If the pattern matches the stored sequence in EEPROM:
   - The STM32 outputs a signal to the TIP120 transistor
   - TIP120 triggers the solenoid to unlock the drawer.
5. Wrong patterns result in no action.

---

## 📂 Project Files

- `main.c`, `stm32f1xx_it.c`, etc. — STM32 application source files
- `.ioc` file — STM32CubeMX configuration
- `.launch`, `.project` — STM32CubeIDE project setup
- `startup_stm32f100rbtx.s` — Bootloader code
- `README.md` — This file

---

## 🎓 Educational Value

This project demonstrates:
- ADC signal processing
- PWM/IO control
- Basic security via pattern recognition

Ideal for microcontroller, signal processing, and embedded systems learning.

---

## 👥 Authors

- **Yusuf Elbiz** 
