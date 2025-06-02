# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build and Development Commands

### Build
```bash
pio run
```

### Upload to Arduino UNO
```bash
pio run -t upload
```

### Monitor Serial Output
```bash
pio device monitor
```

### Clean Build
```bash
pio run -t clean
```

## Architecture Overview

This is an Arduino UNO project for controlling a robotic car with a mechanical arm. The system combines line tracking, obstacle avoidance, and pick-and-place operations.

### Key Components

1. **Motor Control System**
   - Uses manual PWM for DC motor speed control
   - Left motor: pins A5 (forward), 8 (backward)
   - Right motor: pins A4 (forward), 12 (backward)
   - Implements differential drive for movement and turning

2. **Servo Control System**
   - 4 servos for robotic arm: base (pin 11), rear arm (pin 5), front arm (pin 3), claw (pin 6)
   - Smooth movement implemented via `slowWrite()` function
   - Pre-defined positions for 3 pick-and-place operations

3. **Sensor System**
   - Line tracking sensors: A2 (left), A3 (right)
   - Obstacle detection sensors: A1 (left), A0 (right)
   - All sensors use digital input mode

### Main Logic Flow

1. Continuously reads tracking sensors for line following
2. Adjusts motor speeds based on sensor readings
3. When both obstacle sensors detect an object:
   - Stops the car
   - Performs servo calibration routine
   - Executes 3 pre-programmed pick-and-place sequences
   - Enters infinite loop (halt state)

### Important Notes

- The project uses PlatformIO with Arduino framework
- Main source file is `src/final.cpp`
- Servo library is included as a dependency in `platformio.ini`
- Manual PWM implementation suggests hardware PWM pins may be limited or reserved