#include rectangle.h
#include <stdint.h>
#include <stdbool.h>

static rectangle_t rect;

void rectangle_init(uint16_t length, uint16_t width) {
    rect.r_length = length;
    rect.r_width = width;
}

uint16_t rectangle_get_area(void) {
    return rect.r_width * rect.r_length;
}

uint16_t rectangle_get_perimeter(void) {
    return (rect.r_length + rect.r_width) * 2;
}

bool rectangle_is_square(void) {
    return (rect.r_length == rect.r_width);
}