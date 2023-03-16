**To run this utility, follow these steps:** 

1. Change to the top-level directory (i.e. `OBC-firmware/`)
2. Run `make -f util/bringup/interface_tests/Makefile` to build
    - To delete the build files, run `make -f util/bringup/interface_tests/Makefile clean`
3. Start a serial terminal with the correct serial settings
    - For Windows: Try [Putty](https://www.putty.org/)
4. Open Uniflash and flash `util/bringup/interface_tests/build/interface_test_utility.out` onto the LaunchPad
5. Execute a test by sending its command character. These characters are defined in `util\bringup\interface_tests\include\op_codes.h`.