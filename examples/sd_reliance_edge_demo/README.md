**To try this example, follow these steps:** 

1. Connect the LaunchPad to a microSD breakout board. Connect to the SPI3 pins (and use SPI3 CS1)
2. Change to the top-level directory (i.e. `OBC-firmware/`)
3. Run `make -f examples/sd_reliance_edge_demo/Makefile` to build
    - To delete the build files, run `make -f examples/sd_reliance_edge_demo/Makefile clean`
4. Open a serial terminal
5. Open Uniflash and flash `make -f examples/sd_reliance_edge_demo/build/sd_reliance_edge_demo.out` onto the LaunchPad