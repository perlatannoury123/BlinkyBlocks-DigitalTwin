#ifndef _DEBUG_MESSAGE_H_
#define _DEBUG_MESSAGE_H_

#include <bb_global.h>

#define DEBUG_MESSAGE_MAX_LENGTH 64

#define DEBUG_MESSAGE_TO_CHILDREN 0
#define DEBUG_MESSAGE_TO_ROOT 1

typedef struct {
	uint8_t packet_type;
	uint8_t packet_size;
	uint8_t content[DEBUG_MESSAGE_MAX_LENGTH];
} debug_message_packet;

void send_debug_message(uint8_t *message, uint8_t len);

struct _L3_packet;

uint8_t process_debug_message(struct _L3_packet *packet);

#endif
