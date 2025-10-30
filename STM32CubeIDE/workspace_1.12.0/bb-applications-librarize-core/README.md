# Blinky Block v2 application sample

This project contains a blank application as well as some samples in branches.

## How it works

Writing an application for a BB requires to edit the user_code.c file, in which several functions may be implemented, depending on your needs:

- `BBinit` is ran once at blinky block startup, you may use it to initialize various states on the blinky block.
- `BBloop` is called repeatedly in the blinky block main loop. You may use it for various updates independant from networking
- Network related functions are declared as weak in the blank program, so you may redefine their behaviour. There are three network functions:

  - `process_standard_packet`: it receives a packet as parameter, returns 0 if processing went well, 1 else. It is called when the blinky block received a packet on one of its interfaces. L3_packet structure contains the input UART port and the packet content as a uint8_t array. The later can be cast to a custom structure.
  - `standard_ack_handler`: this callback is called when a packet issued by the current BB, and requiring an acknowledgment from its destination, is acknowledged.
  - `standard_unack_handler`: this callback, also related to a packet sent and requiring an ACK was not acknoledged by its destination (internally, it is sent 3 times before standard_unack_handler is called).

Your user_code source file includes BB.h which provides you with 3 utility functions and network communications:

- `get_id`: returns current BB soft ID. It has a binding for old syntax: `getId`
- `sleep_msec`: this function or its counterpart `sleep_sec` shall be used instead of `HAL_Delay` (which will freeze network communications). It takes a parameter, _delay_, as milliseconds to wait. It returns at least after having waited for _delay_ milliseconds.
- `sleep_sec`: it operates like _sllep_msec_ but its _delay_ is in seconds

## Network

Network relies on a set of functions to be able to send various kind of transmissions (unicast, broadcast and multicast).

The core function is `send_message`, which sends a unicast message. It also has a binding for old syntax: `sendMessage`. It takes the following parameters:

  - **port**: the id of the UART output port (the one which the destination BB is connected to). It ranges from 0 to 5.
  - **data**: an array of uint8_t containing the data to be sent
  - **size**: the number of bytes in data
  - **has_ack**: a flag indicating whether the message must be acknoledged by its destination (has_ack=1) or not (has_ack=0)

Several functions are derived from `send_message`:

 - `send_delayed_message` sends a message after a period of time. Its parameters are the same as `send_message`, with one addition: there is a `delay` parameter as a uint16_t number of milliseconds before sending the message.
 - `send_broadcast_message` sends a message to all connected neighbors. Therefore, it has the same parameters as `send_message` except for the `port` which is absent from the broadcast since no particular port is targeted.
 - `send_delayed_broadcast_message` sends a delayed broadcast message. It has one additional parameter compared to `send_broadcast_message` which is the delay before sending the message (see `send_delayed_message`).
 - `send_multicast_message` sends a message to a list of output ports (it skips the ones not connected). The parameters are similar to `send_message` except there is no port but a mask of interfaces to send the message to. The mask is composed of ones for the ports that must transmit the message. (0th bit for first interface, 1st bit for second interface, and so on).
 - `send_delayed_multicast_message` sends a delayed multicast message. It adds the delay to the parameters of `send_multicast_message`.

## Example code

Let say you want to propagate a blue color through the ensemble from a leader whose soft identifier is _1_. You shall write the following code snippets in `user_code.c`.

```c
typedef enum __packed {INIT, LIGHT, OFF} Status;
Status status = INIT;

#define MESSAGE_LIGHT_NEIGHBORS 1
```

First, we define an enumeration to define the current blinky block status. We rely on three values:
- INIT is the initial value, the block's LED is off, it is waiting for a message
- LIGHT defines the LED on status, that will happen once for each block during a given execution
- OFF defines the end status, when the block propagated to its neighbors and switched off its LED. It is required to split status into INIT and OFF values to avoid the block to light twice or more in a given execution.

```c
void BBinit() {
        if (getId() == 1) {
                status = LIGHT;
        }
}
```

This initialization is simple: only the leader will update its status to LIGHT. Everything else is delegated to the BBloop function.

```c
void BBloop() {
        if (status == LIGHT) {
                setColor(DARK_BLUE);
                sleep_sec(1);
                uint8_t msg = MESSAGE_LIGHT_NEIGHBORS;
                for (uint8_t i=0; i<NB_SERIAL_PORT; ++i)
                        if (is_connected(i))
                                sendMessage(i, &msg, 1, 0);
                status = OFF;
                setColor(BLACK);
        }
}
```

In the BBloop function, the block will check if it is in LIGHT status. If so, it sets its LED to a blue color, then it sleeps for one second. After sleeping, the block sends a message to all its neighbors, then changes its status to OFF, and finally switches off its LED.

```c
uint8_t process_standard_packet(L3_packet *p) {
        if (status == INIT) {
                status = LIGHT;
        }
}
```

The message handler is simple: only a block in INIT mode has to check the message (thus avoiding blinking twice). When it happens, it changes its status to LIGHT (which will trigger the BBloop behavior described above).

Notice that the value transmitted in the packet is not used by its recipient. Another derived and fun example could be to choose a random color, then transmit its index instead of a value of _1_.

