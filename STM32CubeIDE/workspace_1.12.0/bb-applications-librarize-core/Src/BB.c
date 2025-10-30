/*
 * BB.c
 *
 *  Created on: Jul 10, 2020
 *      Author: flassabe
 */

#include <BB.h>
#include <flash_storage.h>
#include <layer2.h>
#include <layer3_generic.h>
#include <abstraction.h>
#include <serial.h>

extern configuration *my_configuration;

/*!
 * @brief get_id returns current BB software ID
 * @return the software ID (a 4-bytes unsigned integer)
 */
uint32_t get_id() {
	if (my_configuration)
		return my_configuration->soft_id;
	return 0;
}

/*!
 * @brief getId is a binding for get_id for compatibility with old sources
 * @return the BB software ID
 */
uint32_t getId() {
	return get_id();
}

/*!
 * \brief send_message send a message to a given UART interface
 * \param uart_idx the index of the UART interface (from 0 to 5) to send the packet to
 * \param data a pointer to the array of bytes to send
 * \param size the number of bytes in data
 * \param has_ack to specify if the encapsulating frame must be ack'd (0: no, 1: yes)
 *
 * */
void send_message(uint8_t port, uint8_t data[], uint16_t size, uint8_t has_ack) {
	if (port >= NB_SERIAL_PORT) return;
	L3_packet *packet = get_free_L3_packet(L3_NORMAL);
	if (packet) {
		packet->io_port = port;
		for (uint16_t i=0; i<size; ++i)
			packet->packet_content[i] = data[i];
		send_layer3_packet(packet, L3_NORMAL, size, has_ack);
	}
}

/*!
 * \brief send_delayed_message send a message to a given UART interface after a delay
 * \param uart_idx the index of the UART interface (from 0 to 5) to send the packet to
 * \param data a pointer to the array of bytes to send
 * \param size the number of bytes in data
 * \param has_ack to specify if the encapsulating frame must be ack'd (0: no, 1: yes)
 * \param delay the delay in milliseconds before the actual sending
 *
 * */
void send_delayed_message(uint8_t port, uint8_t data[], uint16_t size, uint8_t has_ack, uint16_t delay) {
	if (port >= NB_SERIAL_PORT) return;
	L3_packet *packet = get_free_L3_packet(L3_NORMAL);
	if (packet) {
		packet->io_port = port;
		for (uint16_t i=0; i<size; ++i)
			packet->packet_content[i] = data[i];
		send_delayed_layer3_packet(packet, L3_NORMAL, size, delay, has_ack);
	}
}

/*!
 * \brief send_multicast_message send a message to a given set of UART interfaces defined by a mask of flags
 * \param uart_idx the index of the UART interface (from 0 to 5) to send the packet to
 * \param data a pointer to the array of bytes to send
 * \param size the number of bytes in data
 * \param dest_mask the interfaces mask: if bit i is set, then data will be sent to interface i, else it won't
 * \param has_ack to specify if the encapsulating frame must be ack'd (0: no, 1: yes)
 * \return a mask of flags indicating all interfaces the data was addressed to (is_connected state is checked)
 *
 * */
uint8_t send_multicast_message(uint8_t data[], uint16_t size, uint8_t dest_mask, uint8_t has_ack) {
	uint8_t res = 0;
	for (uint8_t i=0; i<NB_SERIAL_PORT; ++i)
		if (is_connected(i) && (((dest_mask >> i) & 1) == 1)) {
			SET_FLAG(res, i);
			send_message(i, data, size, has_ack);
		}
	return res;
}

/*!
 * \brief send_delayed_multicast_message sends after a delay a message to a given set of UART interfaces defined by a mask of flags
 * \param uart_idx the index of the UART interface (from 0 to 5) to send the packet to
 * \param data a pointer to the array of bytes to send
 * \param size the number of bytes in data
 * \param dest_mask the interfaces mask: if bit i is set, then data will be sent to interface i, else it won't
 * \param has_ack to specify if the encapsulating frame must be ack'd (0: no, 1: yes)
 * \param delay the delay in milliseconds before the actual sending
 * \return a mask of flags indicating all interfaces the data was addressed to (is_connected state is checked)
 *
 * */
uint8_t send_delayed_multicast_message(uint8_t data[], uint16_t size, uint8_t dest_mask, uint8_t has_ack, uint16_t delay) {
	uint8_t res = 0;
	for (uint8_t i=0; i<NB_SERIAL_PORT; ++i)
		if (is_connected(i) && (((dest_mask >> i) & 1) == 1)) {
			SET_FLAG(res, i);
			send_delayed_message(i, data, size, has_ack, delay);
		}
	return res;
}

/*!
 * \brief send_broadcast_message sends a message to all interfaces
 * \param uart_idx the index of the UART interface (from 0 to 5) to send the packet to
 * \param data a pointer to the array of bytes to send
 * \param size the number of bytes in data
 * \param has_ack to specify if the encapsulating frame must be ack'd (0: no, 1: yes)
 * \return a mask of flags indicating all interfaces the data was addressed to (is_connected state is checked)
 *
 * */
uint8_t send_broadcast_message(uint8_t data[], uint16_t size, uint8_t has_ack) {
	uint8_t res = 0;
	for (uint8_t i=0; i<NB_SERIAL_PORT; ++i)
		if (is_connected(i)) {
			SET_FLAG(res, i);
			send_message(i, data, size, has_ack);
		}
	return res;
}

/*!
 * \brief send_delayed_broadcast_message sends a message to all interfaces after a delay
 * \param uart_idx the index of the UART interface (from 0 to 5) to send the packet to
 * \param data a pointer to the array of bytes to send
 * \param size the number of bytes in data
 * \param has_ack to specify if the encapsulating frame must be ack'd (0: no, 1: yes)
 * \param delay the delay in milliseconds before the actual sending
 * \return a mask of flags indicating all interfaces the data was addressed to (is_connected state is checked)
 *
 * */
uint8_t send_delayed_broadcast_message(uint8_t data[], uint16_t size, uint8_t has_ack, uint16_t delay) {
	uint8_t res = 0;
	for (uint8_t i=0; i<NB_SERIAL_PORT; ++i)
		if (is_connected(i)) {
			SET_FLAG(res, i);
			send_delayed_message(i, data, size, has_ack, delay);
		}
	return res;
}

/*!
 * @brief sleep_msec waits for a parameterized time while processing events
 * @param delay_msec the waiting delay in milliseconds
 */
void sleep_msec(uint32_t delay_msec) {
	uint32_t end_time = HAL_GetTick() + delay_msec;
	while (HAL_GetTick() < end_time) {
		  check_reemitted_frames();
		  check_pending_frames();
		  check_last_sent();
		  extern uint8_t must_reboot;
		  extern uint32_t reboot_time;
		  if (must_reboot && HAL_GetTick() >= reboot_time)
			  NVIC_SystemReset();
	}
}

/*!
 * @brief sleep_sec is a front for sleep_msec, but with a delay in seconds
 * @param delay_sec the delay to wait for in seconds
 */
void sleep_sec(uint32_t delay_sec) {
	sleep_msec(delay_sec * 1000);
}

/*!
 * @brief sendMessage is a binding for send_message for backward compatibility with old sources
 * @param port the output port (the one to send the message through)
 * @param data a pointer/an array of uint8_t holding the data to send
 * @param size the size of the data pointed by the data array/pointer
 * @param has_ack a flag to request an ack frame from the destination upon reception.
 */
void sendMessage(uint8_t port, uint8_t data[], uint16_t size, uint8_t has_ack) {
	send_message(port, data, size, has_ack);
}
