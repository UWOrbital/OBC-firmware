**To try this example, follow these steps:** 

1. Change to the top-level directory (i.e. `OBC-firmware/`)
2. Run `make -f examples/sci_read_demo/Makefile` to build
    - To delete the build files, run `make -f examples/sci_read_demo/Makefile clean`
3. Start a serial terminal with the correct serial settings
    - For Windows: Try [Putty](https://www.putty.org/)
    - For Linux: Try [this](https://www.cyberciti.biz/faq/find-out-linux-serial-ports-with-setserial/)
4. Open Uniflash and flash `examples/sci_read_demo/build/sci_read_demo.out` onto the LaunchPad
5. Type something into the serial terminal and watch it get echoed back. You can only send 20 characters at once.