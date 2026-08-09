# STM32 Industrial Equipment Condition Monitor

A STM32F103C8T6-based embedded condition monitoring and firmware validation system designed to detect abnormal equipment vibration using an MPU6050 inertial sensor.

The project combines embedded firmware development, I2C sensor interfacing, UART diagnostics, baseline calibration, vibration classification, sensor validation, fault-injection testing, and structured validation documentation.

---

## Project Status

| Item | Details |
|---|---|
| Version | V1 |
| Status | Hardware tested and functionally validated |
| Platform | STM32F103C8T6 Blue Pill |
| MCU Core | ARM Cortex-M3 |
| Firmware | Embedded C using STM32 HAL |
| Sensor | MPU6050 accelerometer + gyroscope |
| Communication | I2C + UART |
| I2C Speed | 100 kHz |
| UART | USART1, 115200 8-N-1 |
| IDE | STM32CubeIDE |
| Configuration | STM32CubeMX |
| Validation Interface | USB-to-TTL UART + Tera Term/equivalent |

---

# 1. Project Objective

The objective of V1 is to develop and validate a compact embedded condition-monitoring platform capable of:

- Initializing and validating an MPU6050 sensor
- Verifying I2C communication
- Performing functional accelerometer and gyroscope read tests
- Establishing a stationary acceleration baseline
- Measuring acceleration magnitude
- Calculating deviation from the baseline
- Classifying vibration conditions
- Detecting abnormal physical disturbances
- Detecting sensor communication failure
- Reporting real-time diagnostic information through UART
- Recording important validation events
- Supporting repeatable hardware-level firmware validation

The project is intentionally developed with a validation-engineering mindset rather than only demonstrating sensor data acquisition.

---

# 2. System Overview

```text
                  +---------------------------+
                  |      STM32F103C8T6        |
                  |         Blue Pill         |
                  |                           |
                  |     Embedded C + HAL      |
                  +-------------+-------------+
                                |
                          I2C / 100 kHz
                                |
                                v
                  +---------------------------+
                  |          MPU6050           |
                  |                           |
                  | Accelerometer + Gyroscope |
                  +-------------+-------------+
                                |
                                v
                       Sensor Data Acquisition
                                |
                                v
                       Raw Data Conversion
                                |
                                v
                      Acceleration Magnitude
                                |
                                v
                       Baseline Comparison
                                |
                                v
                 +-----------------------------+
                 |   Vibration Classification  |
                 +-----------------------------+
                 | NORMAL                      |
                 | ELEVATED                    |
                 | WARNING                     |
                 | CRITICAL                    |
                 +-------------+---------------+
                                |
                                v
                       UART Diagnostic Log
                                |
                                v
                       PC / Serial Terminal
```

---

# 3. Hardware

| Component | Purpose |
|---|---|
| STM32F103C8T6 Blue Pill | Main microcontroller |
| MPU6050 | Accelerometer and gyroscope |
| USB-to-TTL UART converter | Serial diagnostic interface |
| Breadboard | Hardware prototyping |
| Jumper wires | Interconnection |

## Main Hardware Interface

The STM32 communicates with the MPU6050 through I2C.

The STM32 communicates with the PC through USART1 using a USB-to-TTL converter.

---

# 4. Firmware Architecture

The V1 firmware follows a validation-oriented processing flow:

```text
STM32 Reset
    |
    v
HAL Initialization
    |
    v
System Clock Configuration
    |
    v
GPIO Initialization
    |
    v
USART1 Initialization
    |
    v
I2C1 Initialization
    |
    v
MPU6050 WHO_AM_I Verification
    |
    v
MPU6050 Initialization
    |
    v
Accelerometer Read Test
    |
    v
Gyroscope Read Test
    |
    v
Functional Self-Test
    |
    v
Baseline Acquisition
    |
    v
Continuous Sensor Acquisition
    |
    v
Acceleration Magnitude
    |
    v
Baseline Deviation
    |
    v
Vibration Classification
    |
    v
UART Diagnostic Output
```

---

# 5. Sensor Communication

The MPU6050 communicates with the STM32 through I2C.

## Configuration

- I2C peripheral: I2C1
- I2C speed: 100 kHz
- MPU6050 address: `0x68`
- STM32 HAL I2C memory read/write functions
- Sensor identification through `WHO_AM_I`

Expected identification:

```text
MPU6050 I2C PASS - WHO_AM_I = 0x68
```

---

# 6. MPU6050 Initialization

The firmware performs the following sequence:

1. Read `WHO_AM_I`
2. Verify sensor identity
3. Wake the MPU6050
4. Configure accelerometer
5. Configure gyroscope
6. Verify the power-management register
7. Perform functional sensor read tests

Example:

```text
MPU6050 I2C PASS - WHO_AM_I = 0x68
MPU6050 INITIALIZATION PASS

[SELF-TEST] MPU6050 FUNCTIONAL TEST
[SELF-TEST] ACCEL READ : PASS
[SELF-TEST] GYRO READ  : PASS
[SELF-TEST] RESULT     : PASS
```

---

# 7. Functional Self-Test

V1 performs a functional communication/read validation of the two major MPU6050 sensing blocks.

## Accelerometer

The firmware attempts to read the accelerometer output registers.

Expected:

```text
[SELF-TEST] ACCEL READ : PASS
```

## Gyroscope

The firmware attempts to read the gyroscope output registers.

Expected:

```text
[SELF-TEST] GYRO READ  : PASS
```

## Overall Result

```text
[SELF-TEST] RESULT     : PASS
```

This validates the basic sensor communication path before continuous monitoring begins.

---

# 8. Baseline Calibration

When the system starts, it collects stationary acceleration measurements to establish an initial reference.

V1 uses:

```text
20 samples
```

The acceleration magnitude is calculated for each sample.

The baseline is calculated as:

```text
Baseline Magnitude =
Sum of 20 Magnitude Samples / 20
```

Example:

```text
[BASELINE] READY
```

After baseline acquisition:

```text
Deviation = |Current Magnitude - Baseline Magnitude|
```

The deviation allows the system to detect changes relative to the initial stationary operating condition.

---

# 9. Acceleration Processing

The MPU6050 accelerometer is configured for the ±2 g range.

For this configuration:

```text
1 g ≈ 16384 raw counts
```

The firmware converts raw accelerometer readings into approximate milli-g values.

The acceleration magnitude is calculated as:

```text
Magnitude = sqrt(X² + Y² + Z²)
```

The resulting magnitude is compared with the established baseline.

---

# 10. Vibration Classification

V1 uses baseline deviation to classify equipment vibration.

| Baseline Deviation | Classification |
|---:|---|
| `< 150 mg` | NORMAL |
| `150–299 mg` | ELEVATED |
| `300–499 mg` | WARNING |
| `>= 500 mg` | CRITICAL |

The classification is based on deviation from the stationary baseline.

## Normal

```text
ACC X=   3 Y=  31 Z= 918 | MAG= 918 | DEV=   6 | VIB=NORMAL   | NORMAL
```

## Elevated

```text
ACC X=-287 Y= 126 Z=1122 | MAG=1164 | DEV= 239 | VIB=ELEVATED | NORMAL
```

## Warning

```text
ACC X=-154 Y=-114 Z= 399 | MAG= 442 | DEV= 480 | VIB=WARNING  | WARNING
```

## Critical

```text
ACC X= 273 Y= 178 Z=1999 | MAG=2025 | DEV=1103 | VIB=CRITICAL | CRITICAL
```

---

# 11. Real-Time UART Diagnostics

The firmware provides fixed-width diagnostic output through USART1 to make hardware validation easier.

Example:

```text
ACC X=   3 Y=  31 Z= 918 | MAG= 918 | DEV=   6 | VIB=NORMAL   | NORMAL
ACC X=   2 Y=  26 Z= 925 | MAG= 925 | DEV=   1 | VIB=NORMAL   | NORMAL
ACC X=   6 Y=  32 Z= 923 | MAG= 923 | DEV=   1 | VIB=NORMAL   | NORMAL
ACC X=  -1 Y=  25 Z= 926 | MAG= 926 | DEV=   2 | VIB=NORMAL   | NORMAL
```

The fixed-width formatting makes changing sensor values easier to inspect in a serial terminal.

---

# 12. Physical Disturbance Testing

The system was physically disturbed during validation to verify that the measured acceleration changes relative to the stationary baseline.

Observed transitions included:

```text
NORMAL
   |
   v
ELEVATED
   |
   v
WARNING
   |
   v
CRITICAL
```

depending on disturbance magnitude.

After the physical disturbance stopped, sensor measurements returned toward the stationary operating region.

This demonstrates that the classification is driven by actual MPU6050 sensor data rather than predefined status messages.

---

# 13. Sensor Failure Testing

A deliberate MPU6050 I2C connection interruption was performed during validation.

Before disconnecting the sensor:

```text
ACC X=   3 Y=  31 Z= 918 | MAG= 918 | DEV=   6 | VIB=NORMAL   | NORMAL
```

After interrupting the I2C connection:

```text
[SENSOR] ACCEL READ FAIL
[SENSOR] ACCEL READ FAIL
[SENSOR] ACCEL READ FAIL
...
```

The firmware therefore detects the failed sensor communication instead of continuing to process apparently valid sensor readings.

### V1 Behavior

After the physical I2C connection is restored, a reset is required to restart normal sensor operation.

This is documented as a V2 improvement area.

---

# 14. Validation Methodology

The project was validated through hardware-level tests covering:

- Communication
- Sensor identification
- Initialization
- Functional sensor reads
- Baseline calibration
- Normal operation
- Physical disturbance response
- Warning classification
- Critical classification
- Sensor communication failure

The validation approach emphasizes observable UART evidence and repeatable hardware actions.

---

# 15. Validation Test Results

## Test 1 — UART Communication

**Objective:** Verify serial diagnostic communication.

**Result:** PASS

---

## Test 2 — MPU6050 I2C Detection

**Objective:** Verify sensor presence and I2C communication.

Expected:

```text
MPU6050 I2C PASS - WHO_AM_I = 0x68
```

**Result:** PASS

---

## Test 3 — MPU6050 Initialization

**Objective:** Verify sensor configuration.

Expected:

```text
MPU6050 INITIALIZATION PASS
```

**Result:** PASS

---

## Test 4 — Functional Self-Test

**Objective:** Verify accelerometer and gyroscope register reads.

Expected:

```text
[SELF-TEST] ACCEL READ : PASS
[SELF-TEST] GYRO READ  : PASS
[SELF-TEST] RESULT     : PASS
```

**Result:** PASS

---

## Test 5 — Baseline Acquisition

**Objective:** Establish the stationary equipment reference magnitude.

Expected:

```text
[BASELINE] READY
```

**Result:** PASS

---

## Test 6 — Normal Condition

**Objective:** Verify stable stationary measurements.

Expected:

```text
DEV < 150 mg
VIB=NORMAL
CONDITION=NORMAL
```

**Result:** PASS

---

## Test 7 — Physical Disturbance

**Objective:** Verify that physical movement changes sensor output and condition classification.

Observed classifications included:

```text
NORMAL
ELEVATED
WARNING
CRITICAL
```

depending on disturbance magnitude.

**Result:** PASS

---

## Test 8 — Sensor Failure

**Objective:** Interrupt MPU6050 I2C communication and verify failure detection.

Observed:

```text
[SENSOR] ACCEL READ FAIL
```

The firmware stopped processing valid sensor samples while the I2C connection was interrupted.

**Result:** PASS — failure detected

---

# 16. Validation Summary

| Validation Item | Result |
|---|---|
| UART communication | PASS |
| MPU6050 WHO_AM_I | PASS |
| MPU6050 initialization | PASS |
| Accelerometer functional read | PASS |
| Gyroscope functional read | PASS |
| Baseline acquisition | PASS |
| Normal condition detection | PASS |
| Physical disturbance detection | PASS |
| Warning classification | PASS |
| Critical classification | PASS |
| Sensor communication failure detection | PASS |

---

# 17. Example Complete Startup Log

```text
MPU6050 I2C PASS - WHO_AM_I = 0x68
MPU6050 INITIALIZATION PASS

[SELF-TEST] MPU6050 FUNCTIONAL TEST
[SELF-TEST] ACCEL READ : PASS
[SELF-TEST] GYRO READ  : PASS
[SELF-TEST] RESULT     : PASS

ACC X=  17 Y=  32 Z= 926 | MAG= 926 | BASELINE
ACC X=  10 Y=  28 Z= 934 | MAG= 934 | BASELINE
ACC X=   7 Y=  30 Z= 924 | MAG= 924 | BASELINE
...
[BASELINE] READY

ACC X=   3 Y=  31 Z= 918 | MAG= 918 | DEV=   6 | VIB=NORMAL   | NORMAL
ACC X=   2 Y=  26 Z= 925 | MAG= 925 | DEV=   1 | VIB=NORMAL   | NORMAL
```

---

# 18. Engineering Skills Demonstrated

## Embedded Firmware

- Embedded C
- STM32 HAL
- STM32CubeIDE
- STM32CubeMX
- ARM Cortex-M3
- Peripheral initialization
- Firmware debugging

## Hardware Interfaces

- I2C
- UART
- GPIO
- MPU6050 sensor interfacing

## Sensor Processing

- Raw sensor data conversion
- Acceleration magnitude calculation
- Baseline calibration
- Threshold-based classification
- Sensor validity checking

## Validation Engineering

- Functional self-test
- Hardware bring-up
- Positive testing
- Disturbance testing
- Fault injection
- Sensor communication failure testing
- UART diagnostic logging
- PASS/FAIL validation methodology

## Development Practices

- STM32CubeMX-generated configuration
- Structured firmware organization
- Git version control
- GitHub portfolio documentation
- Structured validation documentation
- Reproducible test procedures

---

# 19. Repository Structure

```text
STM32-Industrial-Equipment-Condition-Monitor/
│
├── Firmware/
│   └── STM32_Equipment_Condition_Monitor_V1/
│       ├── Core/
│       │   ├── Inc/
│       │   └── Src/
│       ├── Drivers/
│       │   ├── CMSIS/
│       │   └── STM32F1xx_HAL_Driver/
│       ├── .ioc
│       └── STM32F103C8TX_FLASH.ld
│
├── Hardware/
│   ├── Datasheets/
│   └── Wiring/
│
├── Documentation/
│
├── Validation/
│   ├── Test_Procedures/
│   └── Test_Results/
│
├── Images/
│
├── Demo/
│
├── .gitignore
└── README.md
```

---

# 20. Build Environment

## Hardware

```text
STM32F103C8T6 Blue Pill
MPU6050
USB-to-TTL UART converter
Breadboard
Jumper wires
```

## Software

```text
STM32CubeIDE
STM32CubeMX
STM32 HAL
Embedded C
Git
GitHub
Tera Term or equivalent serial terminal
```

## UART Configuration

```text
Baud Rate : 115200
Data      : 8 bit
Parity    : None
Stop Bits : 1
Flow Ctrl : None
```

---

# 21. How to Build and Run

1. Clone the repository.
2. Open STM32CubeIDE.
3. Import the project located at:

```text
Firmware/STM32_Equipment_Condition_Monitor_V1/
```

4. Open the STM32CubeIDE project.
5. Build the project.
6. Program the STM32F103C8T6.
7. Connect the UART interface.
8. Open a serial terminal.
9. Configure the terminal as:

```text
115200 baud
8 data bits
No parity
1 stop bit
No flow control
```

10. Reset the STM32.
11. Keep the MPU6050 stationary during the initial baseline acquisition.
12. Observe the initialization and self-test messages.
13. Wait for:

```text
[BASELINE] READY
```

14. Observe normal monitoring output.
15. Physically disturb the sensor/equipment to test vibration classification.
16. For sensor-failure validation, interrupt the MPU6050 I2C connection and observe the failure message.
17. Restore the connection and reset the system for V1 recovery.

---

# 22. V1 Limitations

V1 intentionally focuses on the core sensing, classification, and validation pipeline.

Current limitations include:

- Threshold-based vibration classification
- Static baseline established only at startup
- No persistent baseline storage
- No automatic sensor recovery after I2C failure
- No moving-average or digital filtering stage
- No RMS vibration calculation
- No frequency-domain analysis
- No SD-card data logging
- No CAN communication
- No graphical user interface
- No remote monitoring
- No automated PC-side validation framework yet

These limitations define the development path for future versions.

---

# 23. V2 Roadmap

## Sensor Reliability

- Automatic MPU6050 reinitialization
- I2C bus recovery
- Sensor reconnection detection
- Sensor health monitoring
- Timeout and retry handling
- Automatic recovery after sensor reconnection

## Signal Processing

- Moving-average filtering
- Low-pass filtering
- RMS vibration measurement
- Peak detection
- Improved vibration classification
- Noise reduction

## Data Logging

- MicroSD logging
- Timestamped measurements
- CSV export
- Fault/event logging

## Industrial Communication

- CAN interface
- Modbus RTU
- RS-485
- Industrial gateway integration

## Validation Automation

- Python-based UART log parser
- Automatic PASS/FAIL test evaluation
- Automated test result generation
- Regression testing
- Automated firmware validation reports

## Monitoring Interface

- PC dashboard
- Real-time vibration plots
- Historical trends
- Equipment health indicator
- Fault/event visualization

---

# 24. Project Evolution

```text
V1
 |
 +-- STM32F103C8T6 bring-up
 |
 +-- UART diagnostics
 |
 +-- MPU6050 I2C communication
 |
 +-- Sensor functional self-test
 |
 +-- Baseline calibration
 |
 +-- Acceleration magnitude calculation
 |
 +-- Baseline deviation calculation
 |
 +-- NORMAL / ELEVATED / WARNING / CRITICAL
 |
 +-- Physical disturbance validation
 |
 +-- Sensor communication fault testing
 |
 v
V2
 |
 +-- Automatic sensor recovery
 +-- I2C bus recovery
 +-- Filtering
 +-- RMS vibration
 +-- Data logging
 +-- CAN / Modbus
 +-- Python validation automation
 +-- Automated test reports
 +-- Monitoring dashboard
```

---

# 25. Why This Project Matters

Industrial equipment condition monitoring requires reliable sensing, meaningful signal interpretation, and robust fault handling.

This project focuses on the embedded validation layer required before developing a larger industrial monitoring system.

The V1 implementation demonstrates the path from:

```text
Physical Sensor
      |
      v
I2C Communication
      |
      v
STM32 Firmware
      |
      v
Sensor Validation
      |
      v
Signal Processing
      |
      v
Condition Classification
      |
      v
Fault Detection
      |
      v
UART Diagnostic Evidence
```

The project is therefore intended as a practical demonstration of embedded firmware development combined with hardware validation and test engineering.

---

# 26. Recruiter-Relevant Highlights

This project demonstrates hands-on experience with:

- STM32F103C8T6 hardware bring-up
- ARM Cortex-M3 firmware
- Embedded C
- STM32 HAL
- I2C peripheral validation
- UART diagnostics
- Inertial sensor interfacing
- Sensor initialization and identification
- Functional self-testing
- Baseline calibration
- Real-time sensor processing
- Threshold-based condition classification
- Physical disturbance testing
- Fault injection
- I2C communication failure detection
- PASS/FAIL validation
- Git/GitHub version control
- Hardware-oriented debugging

The project is particularly relevant to embedded firmware, hardware validation, electronics test, product validation, characterization, and embedded systems engineering roles.

---

# 27. Current V1 Validation Status

```text
SYSTEM BRING-UP              : PASS
UART COMMUNICATION           : PASS
I2C COMMUNICATION            : PASS
MPU6050 IDENTIFICATION       : PASS
MPU6050 INITIALIZATION       : PASS
ACCELEROMETER READ           : PASS
GYROSCOPE READ               : PASS
FUNCTIONAL SELF-TEST         : PASS
BASELINE ACQUISITION         : PASS
NORMAL CONDITION             : PASS
PHYSICAL DISTURBANCE         : PASS
ELEVATED CLASSIFICATION      : PASS
WARNING CLASSIFICATION       : PASS
CRITICAL CLASSIFICATION      : PASS
SENSOR FAILURE DETECTION     : PASS

OVERALL V1 STATUS             : FUNCTIONALLY VALIDATED
```

---

# 28. Future Validation Philosophy

Future versions will extend the project from manual hardware validation toward automated validation.

The intended progression is:

```text
V1
Manual Hardware Validation
        |
        v
V2
Automated UART Log Validation
        |
        v
V3
Automated Regression Testing
        |
        v
V4
Hardware-in-the-Loop Validation
        |
        v
Industrial Condition Monitoring Platform
```

The long-term goal is to combine embedded firmware, hardware validation, automated testing, sensor diagnostics, industrial communication, and data analysis into a reusable engineering platform.

---

# 29. Author

**Adwaith P**

B.Tech Electrical & Electronics Engineering

Embedded Systems | Hardware Validation | Electronics Test & Validation

---

# 30. License

This project is intended primarily as an educational and engineering portfolio project.

The firmware and documentation are provided for learning, experimentation, hardware validation, and portfolio demonstration.
