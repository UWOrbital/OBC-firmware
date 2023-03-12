**To try this example, follow these steps:** 

1. Change to the top-level directory (i.e. `OBC-firmware/`)

2. Run `make -f examples/sci_print_demo/Makefile` to build
    - To delete the build files, run `make -f examples/sci_print_demo/Makefile clean`
3. Open a serial terminal
    - For Windows: Try [Putty](https://www.putty.org/)
    - For Linux: Try [this](https://www.cyberciti.biz/faq/find-out-linux-serial-ports-with-setserial/)
4. Find the COM port identified as `XDS Class Application/User UART`
5. Set the serial port settings:
    - COM Port: What you found above
    - Baud Rate: 115200
    - Data Bits: 8
    - Stop Bits: 2
    - Parity: None
6. Start the serial terminal
7. Open Uniflash and flash `examples/sci_print_demo/build/hello-world.out` onto the LaunchPad
8. View User LED A blinking on the board and `Hello World!` being printed to the serial terminal