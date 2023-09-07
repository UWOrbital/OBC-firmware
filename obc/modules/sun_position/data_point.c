#include "data_point.h"

int equalsPositionData(const position_data_t data1, const position_data_t data2) {
    return data1.julianDate == data2.julianDate && data1.x == data2.x && data1.y == data2.y && data1.z == data2.z;
}