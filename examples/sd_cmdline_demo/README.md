**To try this example, follow these steps:** 

1. Connect the LaunchPad to a microSD breakout board. Connect to the SPI3 pins (and use SPI3 CS1)
2. Change to the top-level directory (i.e. `OBC-firmware/`)
3. Run `make -f examples/sd_cmdline_demo/Makefile` to build
    - To delete the build files, run `make -f examples/sd_cmdline_demo/Makefile clean`
4. Open a serial terminal
    - For Windows: Try [Putty](https://www.putty.org/)
    - For Linux: Try [this](https://www.cyberciti.biz/faq/find-out-linux-serial-ports-with-setserial/)
5. Find the COM port identified as `XDS Class Application/User UART`
6. Set the serial port settings:
    - COM Port: What you found above
    - Baud Rate: 115200
    - Data Bits: 8
    - Stop Bits: 2
    - Parity: None
7. Start the serial terminal
8. Open Uniflash and flash `examples/sd_cmdline_demo/build/sd_demo.out` onto the LaunchPad