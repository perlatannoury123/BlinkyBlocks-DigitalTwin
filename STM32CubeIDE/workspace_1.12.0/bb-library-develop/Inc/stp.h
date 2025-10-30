#ifndef _STP_H_
#define _STP_H_


#include <bb_global.h>

#ifdef STP

#define STP_COMMAND_CLEAR_TREE 0x00
#define STP_COMMAND_PARENT_REQUEST 0x40
#define STP_COMMAND_CHILD_ACCEPT 0x80
#define STP_COMMAND_CHILD_REJECT 0xc0

/*!
 * \brief stp_command is the only byte required to be transmitted by the STP protocol
 * It is decomposed into two parts:
 *  - bits 7 and 6 define the command type:
 *    * 00 (0x00) -> (request) clear tree
 *    * 01 (0x40) -> (request) source tries to be parent of destination
 *    * 10 (0x80) -> (response) destination accepts being child of source
 *    * 11 (0xc0) -> (response) destination refuses to be child of source
 *
 * STP operations:
 * (1) If hello protocol is enabled
 *  - Upon reception of a parent request on a port:
 *    If current BB already has a different parent: rejects the parent request
 *    Else:
 *    - sends a parent request to each connected neighbor
 *    - If none connected, sends an accept with count 1 (counting only itself)
 *  - Upon reception of an accept along with a BB count:
 *    Sums all BB counts and adds 1 for itself
 *    If all neighbors replied, sends an accept parent to the issuer of the request
 * (2) If hello protocol is not enabled (legacy STP)
 * 	- Upon receiving a parent request on one port, current BB looks whether it already has
 * 	  a parent.
 * 	- If not, or if its parent is the same than the request one, it replies positively to the
 * 	  requester BB and requests each port to become its neighbors parent
 * 	- Else, it rejects the request
 */
typedef struct _packed {
	uint8_t packet_type;
	uint32_t bb_count;
} stp_command;

struct _L3_packet;

uint8_t process_STP_packet(struct _L3_packet *p);
void stp_unack_handler(struct _L3_packet *p);

void copy_packet_to_children(struct _L3_packet *p, uint8_t ptype, uint16_t size);

void start_stp();

uint8_t is_child(uint8_t uart_index);

#endif // STP

#endif // _STP_H_
