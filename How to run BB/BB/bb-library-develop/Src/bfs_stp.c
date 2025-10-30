/*
 * bfs_stp.c
 *
 *  Created on: 6 janv. 2022
 *      Author: flassabe
 */

#include <bfs_stp.h>
#include <abstraction.h>
#include <layer3_generic.h>

L3_functions bfs_stp_functions = {
		.process_packet = &process_bfs_stp_packet,
		.ack_handler = &bfs_stp_ack_handler,
		.unack_handler = &bfs_stp_unack_handler,
};

bfs_stp_properties_t bfs_stp_properties ={
		.depth = 0xffff,
		.children = 0,
		.parent = NB_SERIAL_PORT,
};

uint8_t pending_responses;  ///< Waiting all these responses before replying
uint8_t potential_children; ///< neighbors who might be children: 0 = I'm a leaf
bool continue_process;

void init_bfs_stp_process() {
	for (uint8_t i=0; i<NB_SERIAL_PORT; ++i)
		if (is_connected(i))
			SET_FLAG(potential_children, i);
}

void flood_activation(uint16_t depth) {
	pending_responses = 0;
	bfs_stp_packet_t packet = {
			.msg_type = MESSAGE_TYPE_ACTIVATE,
			.payload.activated_depth = depth,
	};
	for (uint8_t i=0; i<NB_SERIAL_PORT; ++i) {
		if (IS_FLAG(potential_children, i)) {
			SET_FLAG(pending_responses, i);
			send_data((uint8_t *) &packet, sizeof(bfs_stp_packet_t), L3_BFS_STP, i, 1);
		}
	}
}

void start_bfs_stp() {
	bfs_stp_properties.depth = 0;
	continue_process = false;
	flood_activation(1);
}

uint8_t process_bfs_stp_packet(L3_packet *p) {
	bfs_stp_packet_t *pkt = (bfs_stp_packet_t *) p->packet_content;
	switch (pkt->msg_type) {
	case MESSAGE_TYPE_ACTIVATE:
		process_activation(pkt->payload.activated_depth, p->io_port);
		break;
	case MESSAGE_TYPE_ACCEPT_PARENT:
		process_parent_acceptance(pkt->payload.has_reached_end, p->io_port);
		break;
	case MESSAGE_TYPE_REJECT_PARENT:
		process_parent_rejection(pkt->payload.has_reached_end, p->io_port);
		break;
	}
	return 0;
}

void bfs_stp_unack_handler(L3_packet *p) {
}

void bfs_stp_ack_handler(L3_packet *p) {
}

void process_activation(uint16_t depth, uint8_t source_uart) {
	if (bfs_stp_properties.depth < depth) {
		// Envoi à tous les enfants - wait for response
	} else if (bfs_stp_properties.depth == depth) {
		// Propagate my depth to every potential child - wait for response
	} else {
		bfs_stp_properties.depth = depth;
		bfs_stp_properties.parent = source_uart;
		//
	}
}

void process_parent_acceptance(bool cont, uint8_t source_uart) {
}

void process_parent_rejection(bool cont, uint8_t source_uart) {
}
