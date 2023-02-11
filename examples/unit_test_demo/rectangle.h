#ifndef EXAMPLES_UNIT_TEST_DEMO_RECTANGLE_H_
#define EXAMPLES_UNIT_TEST_DEMO_RECTANGLE_H_

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint16_t r_length;
    uint16_t r_width;
} rectangle_t;

void rectangle_init(uint16_t length, uint16_t width);

uint16_t rectangle_get_area(void);

uint16_t rectangle_get_perimeter(void);

bool rectangle_is_square(void);

#endif /* EXAMPLES_UNIT_TEST_DEMO_RECTANGLE_H_ */
