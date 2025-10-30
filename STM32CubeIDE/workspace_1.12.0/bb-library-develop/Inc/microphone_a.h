#ifndef MICROPHONE_A_H_
#define MICROPHONE_A_H_

#include <stdint.h>

float get_microphone_frequency();
uint16_t get_microphone_level();
uint16_t get_mic_upper_bound();
uint16_t get_mic_lower_bound();
uint8_t are_bounds_initialized();

#endif // MICROPHONE_A_H_
