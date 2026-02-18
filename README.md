# ESP32 Relay-Based DC Motor Control with RPM Feedback

Embedded systems project focused on deterministic motor control, safety validation, and hardware–software integration.

This system uses an ESP32 to control a 12V DC motor through a dual-relay configuration, allowing forward and reverse rotation. Operator inputs are validated before execution to prevent unsafe states, while an optical encoder provides real-time RPM estimation displayed on an OLED screen.

Designed with an engineering mindset: predictable behavior, command verification, and fail-safe operation.

---

## Features

- Forward and reverse motor control using relays  
- Emergency stop with priority over all commands  
- Optical encoder for real-time RPM calculation  
- Detection of conflicting operator inputs  
- OLED display for system status  
- LED indicators for quick diagnostics  
- Structured C++ firmware  

---

## Hardware

| Component | Function |
|------------|------------|
| ESP32 | Main microcontroller |
| 12V DC Motor | Mechanical actuator |
| Dual Relay Module | Direction control |
| Slotted Encoder Disk | Shaft rotation tracking |
| IR Phototransistor Sensor | Pulse detection |
| OLED (I2C) | Status display |
| Push Buttons | Operator commands |
| Red / Green LEDs | Visual state indicators |

---

## System Behavior

The controller evaluates every operator request before energizing the motor.

### Motor States
- **Stopped**
- **Running Forward**
- **Running Reverse**
- **Emergency Stop**

Conflicting commands (e.g., forward and reverse pressed simultaneously) trigger a safe shutdown.

This approach mirrors basic industrial interlocking principles.

---

## Relay Logic

| KM1 | KM2 | Motor State |
|--------|--------|---------------|
| 0 | 0 | Stopped |
| 0 | 1 | Forward |
| 1 | 0 | Reverse |

The firmware avoids unsafe relay combinations to reduce electrical stress and mechanical shock.

---

## RPM Measurement

Motor speed is estimated using a slotted optical encoder attached to the shaft.

Each light interruption generates a pulse detected by the ESP32.  
One full revolution is computed after the configured number of slots is reached.

RPM = 1 / (time_for_one_revolution_in_minutes)


### Current Implementation
- Edge detection via polling  
- Timing measured with `millis()`  
- Floating-point RPM calculation  

The method favors reliability and simplicity while maintaining stable readings.

---

## Safety Strategy

- Emergency stop overrides every system state  
- Invalid command combinations prevent motor activation  
- Relay outputs default to a safe stop condition  
- Direction changes require state reevaluation  

The design prioritizes predictable behavior over aggressive responsiveness.

---

## Project Structure

The repository is organized to separate firmware, documentation, and media:

ESP32-Motor-Control/
├── firmware/
│ └── main.cpp ← ESP32 source code
├── docs/
│ ├── wiring.md ← Pinout, relay, LED and motor wiring
│ ├── control-logic.md ← Motor control logic explanation
│ └── encoder.md ← RPM measurement details
├── media/
│ ├── oled_demo.png ← OLED status screenshots
│ └── wiring_diagram.png ← Wiring and layout diagram
└── README.md ← Project overview

Keeping documentation concise improves readability and reflects professional engineering practices.

---

## Getting Started

### Requirements
- ESP32  
- Arduino framework or PlatformIO  
- Adafruit SSD1306 + GFX libraries  
- 12V power supply  

### Flashing
1. Clone the repository  
2. Install dependencies  
3. Build and upload the firmware  

---

## Engineering Notes

- `INPUT_PULLUP` is used to improve signal stability.  
- Logical inversion is handled in software.  
- Static variables preserve encoder timing data between cycles.  
- The firmware favors clarity and deterministic behavior over premature optimization.

---

## Possible Future Improvements

- Interrupt-based encoder reading  
- Software debouncing  
- Non-blocking display updates  
- Explicit finite-state machine  
- PWM speed control  
- Closed-loop regulation (PID)  

These enhancements would move the system closer to production-style embedded design.

---

## License

MIT License.
