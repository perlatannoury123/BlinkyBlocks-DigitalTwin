#ifndef _BB_H_
#define _BB_H_

#include <stdint.h>
#include <layer3_generic.h>

// Accessing configuration
uint32_t get_id();
uint32_t getId();

// Network functions
void send_message(uint8_t port, uint8_t data[], uint16_t size, uint8_t has_ack);
void send_delayed_message(uint8_t port, uint8_t data[], uint16_t size, uint8_t has_ack, uint16_t delay);
uint8_t send_multicast_message(uint8_t data[], uint16_t size, uint8_t dest_mask, uint8_t has_ack);
uint8_t send_delayed_multicast_message(uint8_t data[], uint16_t size, uint8_t dest_mask, uint8_t has_ack, uint16_t delay);
uint8_t send_broadcast_message(uint8_t data[], uint16_t size, uint8_t has_ack);
uint8_t send_delayed_broadcast_message(uint8_t data[], uint16_t size, uint8_t has_ack, uint16_t delay);

// Active wait functions (still processes network events)
void sleep_msec(uint32_t delay_msec);
void sleep_sec(uint32_t delay_sec);

// For backward compatibility:
void sendMessage(uint8_t port, uint8_t data[], uint16_t size, uint8_t has_ack);

#endif // _BB_H_
