#pragma once

typedef double julian_date_t;
typedef float position_t;

/**
 * @struct	position_data_t
 * @brief	Holds a single position data point
*/
typedef struct position_data_t {
    julian_date_t julianDate;
    position_t x;
    position_t y;
    position_t z;
} position_data_t;

int equalsDataPoint(const position_data_t a, const position_data_t b);
