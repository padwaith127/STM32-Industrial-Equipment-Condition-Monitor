\# V1 Wiring Documentation



\## STM32 Industrial Equipment Condition Monitor



\### Version

V1



\### Hardware Platform

STM32F103C8T6 Blue Pill



\### Sensor

MPU6050 6-Axis Accelerometer + Gyroscope



\### Communication

I2C



\### Diagnostic Interface

USART1 UART



\### UART Configuration

\- Baud Rate: 115200

\- Data Bits: 8

\- Stop Bits: 1

\- Parity: None

\- Hardware Flow Control: None



\---



\# 1. System Hardware



The V1 prototype consists of:



\- STM32F103C8T6 Blue Pill

\- MPU6050 sensor module

\- USB-to-UART converter

\- Breadboard

\- Jumper wires

\- USB power/programming connection



The STM32F103C8T6 performs sensor initialization, data acquisition, baseline calibration, vibration classification, and diagnostic reporting.



\---



\# 2. MPU6050 I2C Connection



The MPU6050 communicates with the STM32 through the I2C1 peripheral.



| MPU6050 Pin | STM32F103C8T6 | Function |

|-------------|---------------|----------|

| VCC | 3.3V | Sensor supply |

| GND | GND | Common ground |

| SCL | PB6 | I2C1 Clock |

| SDA | PB7 | I2C1 Data |

| INT | Not used in V1 | Interrupt |

| AD0 | GND / default | I2C address selection |



\### I2C Address



7-bit address: 0x68



HAL address used by the firmware: 0xD0



The firmware verifies the device using:



WHO\_AM\_I = 0x68



\---



\# 3. USART1 UART Connection



USART1 is used to transmit firmware diagnostics and sensor measurements to a PC serial terminal.



| STM32F103C8T6 | USB-UART Converter | Function |

|---------------|---------------------|----------|

| PA9 | RX | STM32 UART TX -> Converter RX |

| PA10 | TX | Converter TX -> STM32 UART RX |

| GND | GND | Common ground |



For the V1 diagnostic output, only the STM32 transmit path is required for normal monitoring.



\### UART Settings



Baud Rate: 115200

Data Bits: 8

Parity: None

Stop Bits: 1

Flow Control: None



\---



\# 4. STM32F103C8T6 Interfaces Used



STM32F103C8T6



I2C1

\- PB6 -> SCL

\- PB7 -> SDA



USART1

\- PA9 -> UART TX

\- PA10 -> UART RX



GPIO

\- Status LED



\---



\# 5. Sensor Configuration



The MPU6050 is configured during firmware startup.



\### Power Management



The firmware writes:



PWR\_MGMT\_1 = 0x00



This wakes the MPU6050 from sleep mode.



\### Accelerometer Range



The accelerometer is configured for:



+/-2 g



Sensitivity used by the firmware:



16384 LSB/g



\### Gyroscope Range



The gyroscope is configured for:



+/-250 deg/s



\---



\# 6. Sensor Initialization Sequence



STM32 Startup

&#x20;   |

&#x20;   v

I2C Initialization

&#x20;   |

&#x20;   v

Read MPU6050 WHO\_AM\_I

&#x20;   |

&#x20;   v

Verify WHO\_AM\_I = 0x68

&#x20;   |

&#x20;   v

Wake MPU6050

&#x20;   |

&#x20;   v

Configure Accelerometer

&#x20;   |

&#x20;   v

Configure Gyroscope

&#x20;   |

&#x20;   v

Verify PWR\_MGMT\_1

&#x20;   |

&#x20;   v

Perform Functional Read Test

&#x20;   |

&#x20;   v

Start Continuous Monitoring



\---



\# 7. Functional Sensor Validation



During startup, the firmware performs a functional communication test.



The following are checked:



Accelerometer read -> PASS / FAIL

Gyroscope read -> PASS / FAIL



A successful test produces:



\[SELF-TEST] MPU6050 FUNCTIONAL TEST

\[SELF-TEST] ACCEL READ : PASS

\[SELF-TEST] GYRO READ  : PASS

\[SELF-TEST] RESULT     : PASS



This confirms that the STM32 can successfully communicate with and read data from the MPU6050.



\---



\# 8. Acceleration Processing



Raw accelerometer data is converted into milli-g units.



For the +/-2 g configuration:



Acceleration (mg) = Raw ADC value \* 1000 / 16384



The firmware calculates acceleration magnitude:



MAG = sqrt(X^2 + Y^2 + Z^2)



The magnitude represents the overall acceleration vector measured by the sensor.



\---



\# 9. Baseline Calibration



When the system starts, it collects 20 acceleration-magnitude samples.



Baseline = Sum of 20 samples / 20



During this period the UART output reports:



BASELINE



After 20 samples:



\[BASELINE] READY



The calculated baseline is then used for subsequent deviation measurements.



\---



\# 10. Vibration Deviation



After baseline calibration:



DEV = |Current Magnitude - Baseline|



The deviation is used to classify the equipment condition.



\---



\# 11. Vibration Classification



The V1 firmware uses the following thresholds:



VIBRATION\_THRESHOLD\_MG = 150 mg

CONDITION\_WARNING\_MG   = 300 mg

CONDITION\_CRITICAL\_MG  = 500 mg



Classification:



DEV < 150 mg

&#x20;   NORMAL



150 mg <= DEV < 300 mg

&#x20;   ELEVATED



300 mg <= DEV < 500 mg

&#x20;   WARNING



DEV >= 500 mg

&#x20;   CRITICAL



The vibration status and overall condition are reported through UART.



\---



\# 12. Example Normal Output



Example:



ACC X=   3 Y=  31 Z= 918 | MAG= 918 | DEV=   6 | VIB=NORMAL   | NORMAL



This indicates:



Acceleration magnitude: 918 mg

Baseline deviation: 6 mg

Vibration status: NORMAL

Condition: NORMAL



\---



\# 13. Example Warning Output



Example:



ACC X=-154 Y=-114 Z=399 | MAG=442 | DEV=480 | VIB=WARNING  | WARNING



This represents a significant deviation from the established baseline.



\---



\# 14. Example Critical Output



Example:



ACC X=273 Y=178 Z=1999 | MAG=2025 | DEV=1103 | VIB=CRITICAL | CRITICAL



This represents a large acceleration disturbance exceeding the critical threshold.



\---



\# 15. Fault Injection Test



The V1 system was tested by deliberately interrupting the MPU6050 I2C connection.



Expected behavior:



MPU6050 disconnected

&#x20;       |

&#x20;       v

I2C transaction fails

&#x20;       |

&#x20;       v

Accelerometer read failure detected

&#x20;       |

&#x20;       v

\[SENSOR] ACCEL READ FAIL



Observed behavior during validation:



\[SENSOR] ACCEL READ FAIL

\[SENSOR] ACCEL READ FAIL

\[SENSOR] ACCEL READ FAIL

...



This demonstrates that the firmware does not continue producing new sensor measurements when the I2C transaction fails.



\---



\# 16. Recovery



After restoring the sensor connection and resetting the STM32, the system performs the startup sequence again:



Reset

&#x20;|

&#x20;v

I2C initialization

&#x20;|

&#x20;v

WHO\_AM\_I validation

&#x20;|

&#x20;v

MPU6050 initialization

&#x20;|

&#x20;v

Functional self-test

&#x20;|

&#x20;v

Baseline acquisition

&#x20;|

&#x20;v

Normal monitoring



This provides a repeatable recovery path following a sensor communication failure.



\---



\# 17. Hardware Validation Status



STM32F103C8T6 bring-up          PASS

MPU6050 I2C communication       PASS

WHO\_AM\_I verification           PASS

MPU6050 initialization          PASS

Accelerometer read              PASS

Gyroscope read                  PASS

Functional self-test            PASS

Baseline calibration            PASS

Normal condition detection      PASS

Warning detection               PASS

Critical detection              PASS

Recovery                        PASS

I2C fault injection             PASS

UART diagnostic output          PASS



\---



\# 18. Important V1 Design Notes



V1 is a prototype validation platform.



The vibration classification is based on acceleration-magnitude deviation from a stationary baseline.



The system is intended to demonstrate:



\- Embedded sensor interfacing

\- Firmware bring-up

\- Peripheral validation

\- Sensor communication validation

\- Real-time data acquisition

\- Threshold-based condition classification

\- Fault detection

\- Fault injection

\- Recovery validation

\- UART-based diagnostics



The V1 implementation is not intended to represent a production-grade industrial vibration-monitoring algorithm.



\---



\# 19. Future Improvements



Potential V2 improvements include:



\- Moving-average filtering

\- RMS vibration calculation

\- Sampling-rate control

\- Frequency-domain analysis using FFT

\- More robust vibration metrics

\- Gyroscope-based diagnostics

\- Persistent event logging

\- SD-card data logging

\- CAN communication

\- OLED/TFT local display

\- Automatic sensor recovery without reset

\- Watchdog-based fault recovery

\- Structured machine-readable diagnostic packets

\- Automated PC-side validation scripts

\- Python-based UART test automation

\- Automated PASS/FAIL report generation



\---



\# 20. Related Repository Sections



Firmware/

&#x20;   STM32\_Equipment\_Condition\_Monitor\_V1/



Hardware/

&#x20;   Wiring/

&#x20;   Datasheets/



Validation/

&#x20;   Test\_Procedures/

&#x20;   Test\_Results/



Images/



Demo/



Documentation/



\---



\# 21. Project Validation Philosophy



The project follows a validation-oriented engineering workflow:



Hardware Bring-Up

&#x20;       |

&#x20;       v

Peripheral Initialization

&#x20;       |

&#x20;       v

Communication Verification

&#x20;       |

&#x20;       v

Functional Testing

&#x20;       |

&#x20;       v

Baseline Calibration

&#x20;       |

&#x20;       v

Normal Operation Testing

&#x20;       |

&#x20;       v

Abnormal Condition Testing

&#x20;       |

&#x20;       v

Fault Injection

&#x20;       |

&#x20;       v

Recovery Testing

&#x20;       |

&#x20;       v

Validation Documentation



The objective is not only to make the sensor work, but to demonstrate that the firmware can detect valid operation, abnormal conditions, and communication failures in a repeatable manner.



\---



\# 22. V1 Status



PROJECT VERSION       : V1

PLATFORM              : STM32F103C8T6

SENSOR                : MPU6050

COMMUNICATION         : I2C

DIAGNOSTIC INTERFACE  : USART1 UART

FIRMWARE              : Embedded C + STM32 HAL



VALIDATION STATUS     : COMPLETE



\---



\## Author



Adwaith P.



STM32 | Embedded Systems | Hardware Validation | Electronics Test



GitHub:

https://github.com/padwaith127



