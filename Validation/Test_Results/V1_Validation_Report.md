\# STM32 Industrial Equipment Condition Monitor

\## V1 Validation Report



\---



\## 1. Document Information



| Item | Details |

|---|---|

| Project | STM32 Industrial Equipment Condition Monitor |

| Version | V1 |

| MCU | STM32F103C8T6 |

| Development Board | STM32F103C8T6 Blue Pill |

| Sensor | MPU6050 |

| Sensor Interface | I2C |

| UART Interface | USART1 |

| Firmware | Embedded C |

| Framework | STM32 HAL |

| IDE | STM32CubeIDE |

| Configuration Tool | STM32CubeMX |

| Validation Type | Functional + Fault-Injection Validation |

| Status | V1 Hardware Tested |



\---



\## 2. Validation Objective



The objective of V1 validation is to verify that the STM32-based condition monitoring firmware can:



\- Establish communication with the MPU6050.

\- Verify the MPU6050 device identity.

\- Initialize the sensor correctly.

\- Read accelerometer data.

\- Read gyroscope data.

\- Establish a stationary acceleration baseline.

\- Calculate acceleration magnitude.

\- Calculate deviation from the baseline.

\- Classify equipment condition.

\- Detect physical disturbances.

\- Recover to NORMAL after disturbance.

\- Detect MPU6050 I2C communication failure.

\- Report validation information through UART.



\---



\## 3. Validation Architecture



```text

STM32F103C8T6

&#x20;     |

&#x20;     | I2C

&#x20;     v

&#x20;  MPU6050

&#x20;     |

&#x20;     v

Accelerometer + Gyroscope Data

&#x20;     |

&#x20;     v

Acceleration Conversion

&#x20;     |

&#x20;     v

Magnitude Calculation

&#x20;     |

&#x20;     v

Baseline Calibration

&#x20;     |

&#x20;     v

Deviation Calculation

&#x20;     |

&#x20;     v

Condition Classification

&#x20;     |

&#x20;     +-------------------+

&#x20;     |                   |

&#x20;     v                   v

&#x20;  NORMAL             WARNING

&#x20;                         |

&#x20;                         v

&#x20;                      CRITICAL

&#x20;     |

&#x20;     v

UART Diagnostic Output

&#x20;     |

&#x20;     v

PC Serial Terminal

```



\---



\## 4. Test Environment



\### Hardware



\- STM32F103C8T6 Blue Pill

\- MPU6050 module

\- Breadboard

\- Jumper wires

\- USB-to-UART interface

\- PC



\### Firmware Configuration



\- MPU6050 I2C address: `0x68`

\- I2C speed: `100 kHz`

\- Accelerometer range: `±2g`

\- Gyroscope range: `±250 °/s`

\- UART baud rate: `115200`

\- UART format: `8-N-1`



\---



\# 5. Test Cases



\## TC-01 — MPU6050 WHO\_AM\_I Test



\### Objective



Verify that the STM32 can communicate with the MPU6050 and correctly read its device identity register.



\### Expected Result



The MPU6050 should respond with:



```text

WHO\_AM\_I = 0x68

```



\### Observed Result



```text

MPU6050 I2C PASS - WHO\_AM\_I = 0x68

```



\### Result



\*\*PASS\*\*



\---



\## TC-02 — MPU6050 Initialization Test



\### Objective



Verify that the MPU6050 can be initialized and taken out of sleep mode.



\### Expected Result



The power-management register should be configured correctly.



\### Observed Result



```text

MPU6050 INITIALIZATION PASS

```



\### Result



\*\*PASS\*\*



\---



\## TC-03 — Accelerometer Functional Read Test



\### Objective



Verify that the STM32 can continuously read the MPU6050 accelerometer registers.



\### Expected Result



Accelerometer register read should succeed.



\### Observed Result



```text

\[SELF-TEST] ACCEL READ : PASS

```



\### Result



\*\*PASS\*\*



\---



\## TC-04 — Gyroscope Functional Read Test



\### Objective



Verify that the STM32 can read the MPU6050 gyroscope registers.



\### Expected Result



Gyroscope register read should succeed.



\### Observed Result



```text

\[SELF-TEST] GYRO READ  : PASS

```



\### Result



\*\*PASS\*\*



\---



\## TC-05 — Integrated MPU6050 Functional Self-Test



\### Objective



Verify the combined accelerometer and gyroscope communication path.



\### Observed Result



```text

\[SELF-TEST] MPU6050 FUNCTIONAL TEST

\[SELF-TEST] ACCEL READ : PASS

\[SELF-TEST] GYRO READ  : PASS

\[SELF-TEST] RESULT     : PASS

```



\### Result



\*\*PASS\*\*



\---



\# 6. Baseline Calibration Test



\## TC-06 — Stationary Baseline Establishment



\### Objective



Verify that the firmware can establish an acceleration-magnitude baseline while the sensor is stationary.



\### Method



The MPU6050 was kept stationary during startup.



The firmware collected baseline acceleration-magnitude samples before enabling normal condition monitoring.



\### Observed Result



```text

\[BASELINE] READY

```



A baseline value around:



```text

BASE ≈ 924 mg

```



was observed during testing.



\### Result



\*\*PASS\*\*



\---



\# 7. Normal Condition Test



\## TC-07 — NORMAL Condition



\### Objective



Verify that the firmware reports NORMAL when the measured acceleration remains close to the established baseline.



\### Observed Examples



```text

ACC X=   3 Y=  31 Z= 918 | MAG= 918 | DEV=   6 | VIB=NORMAL   | NORMAL



ACC X=   2 Y=  26 Z= 925 | MAG= 925 | DEV=   1 | VIB=NORMAL   | NORMAL



ACC X=   6 Y=  32 Z= 923 | MAG= 923 | DEV=   1 | VIB=NORMAL   | NORMAL

```



\### Result



\*\*PASS\*\*



\---



\# 8. Physical Disturbance Test



\## TC-08 — Mechanical Disturbance



\### Objective



Verify that physical movement or disturbance of the MPU6050 produces a measurable change in acceleration magnitude and deviation.



\### Observed Examples



```text

ACC X=-154 Y=-114 Z= 399 | MAG= 442 | DEV= 480 | WARNING



ACC X= 261 Y=-305 Z= 369 | MAG= 545 | DEV= 377 | WARNING



ACC X= 273 Y= 178 Z=1999 | MAG=2025 | DEV=1103 | CRITICAL

```



The disturbance caused substantial deviation from the stationary baseline.



\### Result



\*\*PASS\*\*



\---



\# 9. WARNING Condition Test



\## TC-09 — WARNING Classification



\### Objective



Verify that the firmware classifies sufficiently large baseline deviation as WARNING.



\### Threshold



```text

WARNING threshold = 300 mg

```



\### Observed Example



```text

MAG=442 | DEV=480 | WARNING

```



\### Result



\*\*PASS\*\*



\---



\# 10. CRITICAL Condition Test



\## TC-10 — CRITICAL Classification



\### Objective



Verify that the firmware classifies severe acceleration deviation as CRITICAL.



\### Threshold



```text

CRITICAL threshold = 500 mg

```



\### Observed Example



```text

MAG=2025 | DEV=1103 | CRITICAL

```



Additional observed critical events were also recorded during physical disturbance testing.



\### Result



\*\*PASS\*\*



\---



\# 11. Recovery Test



\## TC-11 — Recovery to NORMAL



\### Objective



Verify that the firmware returns to NORMAL after the physical disturbance is removed and the sensor returns to a stable state.



\### Observed Result



After the disturbance, the acceleration magnitude returned close to the established baseline.



Example:



```text

ACC X=  10 Y=  30 Z= 926 | MAG= 926 | DEV=   4 | VIB=NORMAL   | NORMAL

```



\### Result



\*\*PASS\*\*



\---



\# 12. Sensor Failure / I2C Fault-Injection Test



\## TC-12 — MPU6050 Communication Failure



\### Objective



Verify that the firmware detects loss of MPU6050 I2C communication instead of continuing to process sensor data.



\### Method



The MPU6050 I2C connection was physically interrupted during operation.



\### Expected Result



The firmware should detect the failed accelerometer transaction and report a sensor read failure.



\### Observed Result



```text

\[SENSOR] ACCEL READ FAIL

\[SENSOR] ACCEL READ FAIL

\[SENSOR] ACCEL READ FAIL

\[SENSOR] ACCEL READ FAIL

...

```



The firmware repeatedly detected the failed I2C transaction.



\### Result



\*\*PASS\*\*



\### Observation



The system required a reset after the I2C connection was restored before normal operation resumed.



This behavior is recorded as a V1 limitation and provides a clear improvement target for a future firmware version.



\---



\# 13. Validation Summary



| Test ID | Validation | Result |

|---|---|---|

| TC-01 | MPU6050 WHO\_AM\_I | PASS |

| TC-02 | MPU6050 Initialization | PASS |

| TC-03 | Accelerometer Read | PASS |

| TC-04 | Gyroscope Read | PASS |

| TC-05 | Functional Self-Test | PASS |

| TC-06 | Baseline Calibration | PASS |

| TC-07 | NORMAL Condition | PASS |

| TC-08 | Physical Disturbance | PASS |

| TC-09 | WARNING Classification | PASS |

| TC-10 | CRITICAL Classification | PASS |

| TC-11 | Recovery to NORMAL | PASS |

| TC-12 | I2C Sensor Failure Detection | PASS |



\---



\# 14. V1 Validation Conclusion



V1 successfully demonstrated the fundamental functionality of the STM32 Industrial Equipment Condition Monitor.



The firmware successfully demonstrated:



\- MPU6050 device detection.

\- I2C communication.

\- Sensor initialization.

\- Accelerometer data acquisition.

\- Gyroscope data acquisition.

\- Functional sensor self-test.

\- Baseline calibration.

\- Acceleration magnitude calculation.

\- Baseline deviation calculation.

\- NORMAL condition detection.

\- WARNING condition detection.

\- CRITICAL condition detection.

\- Recovery to NORMAL.

\- I2C sensor failure detection.

\- UART-based diagnostic reporting.



The V1 system was therefore considered \*\*functionally validated on hardware for the implemented test cases\*\*.



\---



\# 15. Known V1 Limitations



The following limitations were observed during V1 testing:



1\. Sensor communication failure produces repeated read-failure messages.

2\. Normal operation requires a firmware reset after the interrupted I2C connection is restored.

3\. The current monitoring algorithm uses acceleration-magnitude deviation rather than a frequency-domain vibration analysis.

4\. No persistent fault storage is implemented.

5\. No dedicated alarm output is implemented.

6\. No timestamped data logging is implemented.

7\. No automated PC-side validation script is implemented.



These limitations are intentionally documented as potential future development areas.



\---



\# 16. Future V2 Improvements



Potential V2 improvements include:



\- Automatic I2C bus recovery.

\- Automatic MPU6050 reinitialization.

\- Sensor reconnection detection.

\- Persistent fault/event logging.

\- EEPROM or microSD event storage.

\- PC-side automated UART test logging.

\- Automated PASS/FAIL test reports.

\- Moving-average filtering.

\- RMS vibration measurement.

\- Peak acceleration detection.

\- Frequency-domain vibration analysis.

\- FFT-based condition monitoring.

\- Additional sensors.

\- CAN communication.

\- Industrial communication interface.

\- Hardware alarm output.

\- Watchdog-based fault recovery.



\---



\# 17. Validation Philosophy



The project follows a validation-oriented development approach:



```text

Requirement

&#x20;   ↓

Implementation

&#x20;   ↓

Hardware Bring-Up

&#x20;   ↓

Functional Test

&#x20;   ↓

Normal Operation Test

&#x20;   ↓

Fault Injection

&#x20;   ↓

Failure Detection

&#x20;   ↓

Recovery Verification

&#x20;   ↓

Validation Evidence

&#x20;   ↓

Documented Result

```



The objective is not only to demonstrate that the sensor works, but to verify how the complete embedded system behaves under both normal and abnormal operating conditions.



\---



\# 18. Evidence



The repository contains the firmware source and project configuration used for V1.



Additional validation evidence such as:



\- Hardware photographs

\- Wiring documentation

\- UART captures

\- Warning/critical captures

\- Sensor-failure captures

\- Demonstration video



will be added to the repository as the portfolio documentation is completed.



\---



\## Final V1 Status



\*\*V1 FUNCTIONALLY VALIDATED\*\*



```text

Hardware Bring-Up       PASS

Sensor Communication   PASS

Functional Read Test    PASS

Baseline Calibration    PASS

Normal Detection        PASS

Warning Detection       PASS

Critical Detection      PASS

Recovery                PASS

Fault Injection         PASS

UART Diagnostics        PASS

```



\*\*Project Version: V1\*\*



\*\*Validation Status: COMPLETE\*\*

