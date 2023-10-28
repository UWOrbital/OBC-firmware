#pragma once

/**
 * @brief Feed the software watchdog in privileged mode
 */
void feedDigitalWatchdog(void);

/**
 * @brief initialize the Digital Windowed Watchdog on the RM46 in privileged mode
 */
void initDigitalWatchdog(void);
