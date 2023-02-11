#include "unity.h" // The testing framework

#include "shape_container.h" // The header for the code we are testing
#include "mock_rectangle.h" // A mock header

void test_shape_container_init(void) {
    // Set up known values
    uint16_t length = 4;
    uint16_t width = 3;

    //State, in order of call, what expectations we have, and the expected values to be returned, if any
    rectangle_init_Expect(length, width);

    //Run Actual Function Under Test
    shape_container_init(length, width);
}
