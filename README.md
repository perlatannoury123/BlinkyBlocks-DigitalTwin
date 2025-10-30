# BlinkyBlocks Digital Twin

This project implements a **Digital Twin** of the Blinky Blocks modular robotic system.

## Structure
- `stm32_code/`: Firmware for physical Blinky Blocks programmed in **STM32CubeIDE (C)**.
- `visiblesim/`: The **digital twin simulator** implemented in **C++**.
- `fifo_communication/`: The **bridge** between the physical and virtual systems, using a FIFO file (`/tmp/bbfifo`).

## How it works
1. STM32 sends Blinky Block states (positions, colors) via serial.
2. Data is redirected to `/tmp/bbfifo`.
3. VisibleSim reads the FIFO to visualize the blocks in real-time.

## Run
```bash
make run
