#ifndef _INITIALIZATION_H_
#define _INITIALIZATION_H_

#include <stdint.h>

void copy_interrupt_vectors();
void SystemClock_Config(void);

void init_application();
// Later: void init_bootloader();

#endif // _INITIALIZATION_H_
