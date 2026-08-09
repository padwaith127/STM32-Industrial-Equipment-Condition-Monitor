\# V1 Test Procedure



\## STM32 Industrial Equipment Condition Monitor



\---



\## 1. Document Information



| Item | Details |

|---|---|

| Project | STM32 Industrial Equipment Condition Monitor |

| Version | V1 |

| MCU | STM32F103C8T6 |

| Sensor | MPU6050 |

| Sensor Interface | I2C |

| Diagnostic Interface | USART1 UART |

| Firmware | Embedded C + STM32 HAL |

| Development Environment | STM32CubeIDE |

| Test Type | Hardware Functional and Validation Testing |



\---



\## 2. Test Objective



The objective of this procedure is to verify that the V1 embedded condition-monitoring system performs correctly at hardware and firmware level.



The validation covers:



\- STM32 hardware bring-up

\- MPU6050 communication

\- WHO\_AM\_I verification

\- Sensor initialization

\- Accelerometer data acquisition

\- Gyroscope data acquisition

\- Functional sensor read test

\- Baseline calibration

\- Normal condition detection

\- Elevated vibration detection

\- Warning condition detection

\- Critical condition detection

\- Recovery after disturbance

\- Sensor communication fault injection

\- UART diagnostic reporting



\---



\## 3. Required Hardware



The following hardware is required:



\- STM32F103C8T6 Blue Pill

\- MPU6050 module

\- USB-to-UART converter

\- Breadboard

\- Jumper wires

\- USB power/programming connection

\- PC with serial terminal

\- STM32 programmer/debugger as required



\---



\## 4. Firmware Configuration



\### MCU



STM32F103C8T6



\### I2C



I2C1



PB6 = SCL



PB7 = SDA



Clock speed:



100 kHz



\### UART



USART1



PA9 = TX



PA10 = RX



Configuration:



\- Baud rate: 115200

\- Data bits: 8

\- Stop bits: 1

\- Parity: None

\- Hardware flow control: None



\---



\## 5. MPU6050 Configuration



The MPU6050 is configured as follows.



\### I2C Address



7-bit address:



0x68



STM32 HAL address:



0xD0



\### Accelerometer



Range:



+/-2 g



Sensitivity:



16384 LSB/g



\### Gyroscope



Range:



+/-250 deg/s



\### Power Management



PWR\_MGMT\_1:



0x00



This wakes the MPU6050 from sleep mode.



\---



\## 6. Test Environment



Before beginning the test:



1\. Connect the STM32F103C8T6 to the MPU6050.

2\. Connect the USB-to-UART converter.

3\. Connect common ground between all required devices.

4\. Power the STM32 and MPU6050.

5\. Program the V1 firmware.

6\. Open a serial terminal.

7\. Configure the terminal for 115200 baud, 8-N-1.

8\. Keep the MPU6050 stationary during startup.

9\. Verify that UART output is visible.



\---



\# 7. Test Case V1-T01 — Hardware Bring-Up



\### Objective



Verify that the STM32 and connected MPU6050 hardware can start correctly.



\### Procedure



1\. Power the STM32F103C8T6.

2\. Observe the UART terminal.

3\. Allow the firmware startup sequence to complete.

4\. Verify that MPU6050 initialization messages are received.



\### Expected Result



The STM32 starts successfully and communicates with the MPU6050.



\### Result



PASS



\---



\# 8. Test Case V1-T02 — MPU6050 WHO\_AM\_I Verification



\### Objective



Verify the identity of the connected MPU6050.



\### Procedure



1\. Reset the STM32.

2\. Monitor the UART terminal.

3\. Observe the WHO\_AM\_I result.



\### Expected Result



WHO\_AM\_I register returns:



0x68



\### Result



PASS



\---



\# 9. Test Case V1-T03 — MPU6050 Initialization



\### Objective



Verify that the MPU6050 is correctly initialized.



\### Procedure



1\. Reset the STM32.

2\. Observe the initialization sequence.

3\. Verify PWR\_MGMT\_1 configuration.

4\. Confirm that the sensor exits sleep mode.



\### Expected Result



MPU6050 initialization succeeds.



\### Result



PASS



\---



\# 10. Test Case V1-T04 — Accelerometer Read Test



\### Objective



Verify that the STM32 can read accelerometer data through I2C.



\### Procedure



1\. Keep the sensor stationary.

2\. Allow the startup self-test to execute.

3\. Observe the accelerometer result.



\### Expected Result



Accelerometer read operation passes.



\### Result



PASS



\---



\# 11. Test Case V1-T05 — Gyroscope Read Test



\### Objective



Verify that the STM32 can read gyroscope data through I2C.



\### Procedure



1\. Keep the sensor stationary.

2\. Allow the startup self-test to execute.

3\. Observe the gyroscope result.



\### Expected Result



Gyroscope read operation passes.



\### Result



PASS



\---



\# 12. Test Case V1-T06 — Functional Sensor Self-Test



\### Objective



Verify that both accelerometer and gyroscope communication are functional.



\### Procedure



1\. Reset the STM32.

2\. Observe the complete self-test sequence.

3\. Verify accelerometer read status.

4\. Verify gyroscope read status.

5\. Verify the overall self-test result.



\### Expected Result



The accelerometer and gyroscope read tests pass.



\### Result



PASS



\---



\# 13. Test Case V1-T07 — Baseline Calibration



\### Objective



Verify that the firmware establishes a stationary acceleration baseline.



\### Procedure



1\. Place the MPU6050 on a stable surface.

2\. Reset the STM32.

3\. Do not disturb the sensor during calibration.

4\. Allow the firmware to collect the baseline samples.

5\. Observe the UART output.

6\. Verify the baseline-ready indication.



\### Expected Result



The firmware calculates the stationary baseline and begins normal monitoring.



\### Result



PASS



\---



\# 14. Test Case V1-T08 — Normal Condition



\### Objective



Verify that stable sensor operation is classified as NORMAL.



\### Procedure



1\. Complete baseline calibration.

2\. Keep the MPU6050 stationary.

3\. Observe the UART output.

4\. Verify that the vibration status remains NORMAL.



\### Example Observed Output



ACC X=   3 Y=  31 Z= 918 | MAG= 918 | DEV=   6 | VIB=NORMAL   | NORMAL



\### Expected Result



Small deviations from the baseline are classified as NORMAL.



\### Result



PASS



\---



\# 15. Test Case V1-T09 — Elevated Vibration



\### Objective



Verify detection of vibration above the normal threshold.



\### Procedure



1\. Complete baseline calibration.

2\. Physically disturb the MPU6050.

3\. Observe the deviation value.

4\. Verify the vibration classification.



\### Threshold



VIBRATION\_THRESHOLD\_MG = 150 mg



\### Expected Result



A deviation above the normal threshold is detected and reported as ELEVATED when applicable.



\### Result



PASS



\---



\# 16. Test Case V1-T10 — Warning Condition



\### Objective



Verify that significant vibration is classified as WARNING.



\### Procedure



1\. Complete baseline calibration.

2\. Apply a physical disturbance to the sensor.

3\. Generate a deviation between the warning and critical thresholds.

4\. Observe the UART output.



\### Warning Threshold



CONDITION\_WARNING\_MG = 300 mg



\### Expected Result



Deviation from 300 mg up to but below 500 mg is classified as WARNING.



\### Example Observed Condition



DEV=480 | VIB=WARNING



\### Result



PASS



\---



\# 17. Test Case V1-T11 — Critical Condition



\### Objective



Verify that severe vibration is classified as CRITICAL.



\### Procedure



1\. Complete baseline calibration.

2\. Apply a stronger physical disturbance.

3\. Observe the calculated deviation.

4\. Verify the critical classification.



\### Critical Threshold



CONDITION\_CRITICAL\_MG = 500 mg



\### Expected Result



Deviation equal to or greater than 500 mg is classified as CRITICAL.



\### Example Observed Condition



DEV=1103 | VIB=CRITICAL | CRITICAL



\### Result



PASS



\---



\# 18. Test Case V1-T12 — Recovery to Normal



\### Objective



Verify that the system returns to NORMAL after the physical disturbance is removed.



\### Procedure



1\. Operate the system under normal conditions.

2\. Apply a physical disturbance.

3\. Confirm WARNING or CRITICAL classification.

4\. Stop disturbing the sensor.

5\. Allow the sensor to return to a stationary state.

6\. Observe the UART output.



\### Expected Result



The measured deviation decreases and the condition returns to NORMAL.



\### Result



PASS



\---



\# 19. Test Case V1-T13 — I2C Sensor Failure Injection



\### Objective



Verify that the firmware detects loss of communication with the MPU6050.



\### Procedure



1\. Start the system normally.

2\. Allow baseline calibration to complete.

3\. Confirm normal sensor measurements are being reported.

4\. Deliberately interrupt the MPU6050 I2C connection.

5\. Observe the UART terminal.



\### Fault Introduced



MPU6050 I2C connection interrupted.



\### Expected Result



The accelerometer I2C transaction fails.



The firmware reports:



\[SENSOR] ACCEL READ FAIL



The firmware must not continue reporting fresh sensor measurements from the disconnected sensor.



\### Observed Result



Repeated sensor read failure messages were observed:



\[SENSOR] ACCEL READ FAIL

\[SENSOR] ACCEL READ FAIL

\[SENSOR] ACCEL READ FAIL



\### Result



PASS



\---



\# 20. Test Case V1-T14 — Sensor Failure Recovery



\### Objective



Verify system recovery after a sensor communication failure.



\### Procedure



1\. Perform the I2C fault-injection test.

2\. Restore the MPU6050 connection.

3\. Reset the STM32.

4\. Observe the startup sequence.

5\. Verify WHO\_AM\_I.

6\. Verify sensor initialization.

7\. Verify functional self-test.

8\. Allow baseline calibration to complete.

9\. Verify normal monitoring.



\### Expected Result



The system restarts successfully and returns to normal monitoring.



\### Result



PASS



\---



\# 21. UART Diagnostic Validation



The UART interface is used as the primary diagnostic interface during V1 validation.



The firmware reports:



\- Sensor identification

\- Sensor initialization

\- Accelerometer read status

\- Gyroscope read status

\- Self-test result

\- Baseline status

\- Acceleration values

\- Acceleration magnitude

\- Baseline deviation

\- Vibration status

\- Condition status

\- Sensor communication failures



\### UART Configuration



115200 baud



8 data bits



No parity



1 stop bit



No hardware flow control



\### Result



PASS



\---



\# 22. Validation Summary



| Test ID | Test | Result |

|---|---|---|

| V1-T01 | Hardware Bring-Up | PASS |

| V1-T02 | WHO\_AM\_I Verification | PASS |

| V1-T03 | MPU6050 Initialization | PASS |

| V1-T04 | Accelerometer Read | PASS |

| V1-T05 | Gyroscope Read | PASS |

| V1-T06 | Functional Self-Test | PASS |

| V1-T07 | Baseline Calibration | PASS |

| V1-T08 | Normal Condition | PASS |

| V1-T09 | Elevated Vibration | PASS |

| V1-T10 | Warning Condition | PASS |

| V1-T11 | Critical Condition | PASS |

| V1-T12 | Recovery to Normal | PASS |

| V1-T13 | I2C Sensor Failure | PASS |

| V1-T14 | Sensor Failure Recovery | PASS |

| V1-T15 | UART Diagnostics | PASS |



\---



\# 23. Acceptance Criteria



V1 is considered functionally validated when:



\- STM32 starts successfully.

\- MPU6050 identity is verified.

\- MPU6050 initialization succeeds.

\- Accelerometer reads succeed.

\- Gyroscope reads succeed.

\- Functional self-test passes.

\- Baseline calibration completes.

\- Normal operation is detected.

\- Elevated vibration can be detected.

\- Warning condition can be detected.

\- Critical condition can be detected.

\- Recovery to normal operation is demonstrated.

\- I2C communication failure is detected.

\- Sensor failure does not produce new valid measurements.

\- System can recover after reset.

\- Diagnostic information is clearly reported over UART.



\---



\# 24. Overall V1 Result



V1 FUNCTIONAL VALIDATION: PASS



Hardware Bring-Up       PASS

WHO\_AM\_I Verification   PASS

Sensor Initialization   PASS

Accelerometer Read      PASS

Gyroscope Read          PASS

Functional Self-Test    PASS

Baseline Calibration    PASS

Normal Detection        PASS

Elevated Detection      PASS

Warning Detection       PASS

Critical Detection      PASS

Recovery                PASS

I2C Fault Injection     PASS

Sensor Failure Detect   PASS

Sensor Recovery         PASS

UART Diagnostics        PASS



\---



\# 25. Test Limitations



This V1 validation is a prototype-level functional validation.



The tests demonstrate firmware behavior and hardware communication rather than production certification.



The vibration classification is based on acceleration-magnitude deviation from a stationary baseline.



The physical disturbance tests are not calibrated mechanical vibration tests.



For a production system, additional validation would be required using controlled vibration sources, defined acceleration levels, controlled frequency content, environmental testing, repeatability studies, and long-duration reliability testing.



\---



\# 26. Future Validation Improvements



Future versions can extend the validation framework with:



\- Automated UART test scripts

\- PC-side PASS/FAIL automation

\- Controlled vibration testing

\- Sampling-rate verification

\- Sensor noise characterization

\- RMS vibration measurement

\- FFT/frequency analysis

\- Temperature testing

\- Long-duration stability testing

\- Watchdog fault recovery

\- Automatic I2C recovery

\- CAN diagnostic validation

\- Data logging

\- Automated validation reports



\---



\# 27. Final Status



Project:



STM32 Industrial Equipment Condition Monitor



Version:



V1



Validation Status:



COMPLETE



Overall Result:



PASS



The V1 prototype successfully demonstrated sensor communication, firmware functionality, vibration-condition classification, UART diagnostics, fault injection, and recovery behavior on physical STM32 hardware.

