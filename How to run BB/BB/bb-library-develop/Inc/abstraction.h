#ifndef _ABSTRACTION_H_
#define _ABSTRACTION_H_

#include <stdint.h>

// Network
void init_physical_layer();
uint8_t is_connected(uint8_t uart_index);
void transmit_byte(uint8_t uart_idx, uint8_t byte);
void init_hello_protocol();

// Lights
void set_RGB(uint8_t red, uint8_t green, uint8_t blue);

// Sound
#define TONE_MULTIPLICATOR 2
#define NOTE_BLACK 330
#define NOTE_WHITE (NOTE_BLACK*2)

#define DO (523*TONE_MULTIPLICATOR)
#define RE (587*TONE_MULTIPLICATOR)
#define MI (659*TONE_MULTIPLICATOR)
#define FA (698*TONE_MULTIPLICATOR)
#define FA_DIESE (740*TONE_MULTIPLICATOR)
#define SOL (784*TONE_MULTIPLICATOR)
#define LA (880*TONE_MULTIPLICATOR)
#define SI (988*TONE_MULTIPLICATOR)

void make_sound(uint16_t freq, uint16_t duration);

// Microphone

// Accelerometer

#endif // _ABSTRACTION_H_
