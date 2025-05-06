To test:

Build firmware via cmake with EXAMPLE_TYPE=ADC
Flash to MCU
Attach voltage to ADIN1 and ADIN2 (please note that ADIN1 and ADIN2 are the only active pins right now for testing, and both are in Group 1)
Connect to serial port
It should read the voltage and display on the console
