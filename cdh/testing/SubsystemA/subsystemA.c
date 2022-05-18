#include "subsystemA.h"

static int c = 0;

void get_data(int *x) {
    *x = c;
    ++c;
}
