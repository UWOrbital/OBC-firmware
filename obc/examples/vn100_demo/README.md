# Instructions on how to setup a test for vn-100

1. Connect 5V and GND pins to the RM46. </br >
2. Connect the TTL pins of the VN-100 (Shown as Rx2 and Tx2) to the sci register (if you are using OBC REV 1 or REV 2, use scilinReg, otherwise if using RM46 launchpad use sciReg). </br >
3. Then depending on what you would like to test, copy the code from main.c into obc/main.c, build and flash. </br >
4. Open the serial monitor to read from the serial outputs of the RM46 and observe the following outputs:
