#ifndef _BFS_STP_H_
#define _BFS_STP_H_

#include <bb_global.h>
#include <stdbool.h>

typedef struct {
	uint16_t depth;
	uint8_t children;
	uint8_t parent;
} bfs_stp_properties_t;

typedef enum __packed {
	MESSAGE_TYPE_ACTIVATE,
	MESSAGE_TYPE_ACCEPT_PARENT,
	MESSAGE_TYPE_REJECT_PARENT,
} bfs_stp_msg_t;

typedef struct _packed {
	bfs_stp_msg_t msg_type;
	union {
		uint16_t activated_depth;
		bool has_reached_end;
	} payload;
} bfs_stp_packet_t;

void init_bfs_stp_process();
void start_bfs_stp();

struct _L3_packet;

uint8_t process_bfs_stp_packet(struct _L3_packet *p);
void bfs_stp_unack_handler(struct _L3_packet *p);
void bfs_stp_ack_handler(struct _L3_packet *p);

void process_activation(uint16_t depth, uint8_t source_uart);
void process_parent_acceptance(bool cont, uint8_t source_uart);
void process_parent_rejection(bool cont, uint8_t source_uart);

void flood_activation(uint16_t depth);

#endif // _BFS_STP_H_
