Autres tutos et infos à cette adresse:
https://benoitpiranda.fr/#/education


# Pre-requisite

In order to follow this guide, you will need a Ubuntu distribution (current tutorial has been tested on Ubuntu 22.04). If you don't have Ubuntu, you may try to use another distribution, or to rely on an Ubuntu Virtual Machine.

You shall download sources of the following packages:

- [libblinkyApploader](https://github.com/flassabe/modular-robots/releases/download/1.0/bb-apploader-develop.zip) (library with BB network stack support for your computer)
- [blinkyApploaderCLI](https://github.com/flassabe/modular-robots/releases/download/1.0/bb-cli-develop.zip) (CLI application to interact with BBs from your computer)
- [bb-library](https://github.com/flassabe/modular-robots/releases/download/1.0/bb-library-develop.zip) (the system and network stack for BB embedded applications)
- [bb-applications](https://github.com/flassabe/modular-robots/releases/download/1.0/bb-applications-librarize-core.zip) (a template BB application for developping a new application)

# Technical setup

Blinky Blocks are ARM-based robots. A computer is required to interact with BB software. On the computer side, a Qt application is used to communicate with the BB through a serial interface. BBs execute two programs: a bootloader and an application. Both have the same network stack. Users develop BB applications to be deployed and run by the bootloader.

Being able to use BBs requires to build the required softwares on the computer, as well as developping and building BB applications.

This tutorial will describe all the steps required to enable writing an application for Blinky Blocks, building it, then deploying and running it on a set of BBs.

# Computer applications

## Preparation: Ubuntu installation of the QtCreator IDE

To program the Blinky Blocks, we will use the QtCreator IDE, to do that you must first install some packages:

```bash
sudo apt-get install libqt5serialport5-dev qtcreator build-essential
```

Now two projects have to be compiled:

- first the library that contains the code,
- then, the apploader-CLI software which provides the CLI interface to the BBs.

Extract both zip files into the same directory. You shall get this kind of layout, with `.` being your project directory:
```
.
├── bb-apploader-develop
│   ├── bb-logo-pict.png
│   ├── bb-logo-text.png
│   ├── bbpp.cpp
│   ├── bbpp.h
│   ├── bf21codec.cpp
│   ├── bf21codec.h
│   ├── blinkyapploader.cpp
│   ├── blinkyapploader_global.h
│   ├── blinkyapploader.h
│   ├── blinkyApploader.pro
│   ├── blinkyApploader.pro.user
│   ├── debug_messages.cpp
│   ├── debug_messages.h
│   ├── device_command_task.cpp
│   ├── device_command_task.h
│   ├── doc
│   │   └── doc_styles.css
│   ├── DOXYFILE
│   ├── ihexparser.cpp
│   ├── ihexparser.h
│   ├── layer2.cpp
│   ├── layer2.h
│   ├── layer3.cpp
│   ├── layer3.h
│   ├── observer.hpp
│   ├── physicalinterface.cpp
│   ├── physicalinterface.h
│   ├── README.md
│   ├── recovery_jumper.cpp
│   ├── recovery_jumper.h
│   ├── soft_id.cpp
│   ├── soft_id.h
│   ├── stp.cpp
│   ├── stp.h
│   ├── sublayer21.cpp
│   └── sublayer21.h
└── bb-cli-develop
    ├── blinkyApploaderCLI.pro
    └── main.cpp
```

## Building blinkyApploader library

The code is in the file `bb-apploader-develop.zip` which extracts to `bb-apploader-develop` (please check your path, and adapt configurations to any difference). Open `blinkyApploader.pro` from QtCreator.

A panel will open, with a button labeled `Configure project`. Click this button. You are now in the project and can build it. First, you shall select the `Release` target (lower left of the screen, with a computer screen icon, click on it and choose `Release`). Then click on the hammer to build the library. As a result, a new folder is created, named `build_bb-apploader`, that contains the library.

To help further steps, copy the library to your system library folder:

```
sudo cp ../build_bb-apploader/release/libblinkyApploader.so.1.0.0 /usr/lib
sudo ln -s /usr/lib/libblinkyApploader.so.1.0.0 /usr/lib/libblinkyApploader.so.1.0
sudo ln -s /usr/lib/libblinkyApploader.so.1.0.0 /usr/lib/libblinkyApploader.so.1
sudo ln -s /usr/lib/libblinkyApploader.so.1.0.0 /usr/lib/libblinkyApploader.so
```

### Building the CLI

The CLI application uses the library. Therefore, you need to set your .pro file accordingly to your own configuration. Path in this tutorial are supposed to be used, provided you didn't rename anything, and followed the layout described in the pre-requisite section.

Open `bb-cli-develop/blinkyApploaderCLI.pro` with QtCreator, then click the `Configure project` button. Choose the `Release` build target. In the `blinkyApploaderCLI.pro` file, replace lines 30 to 35 by:
```
INCLUDEPATH += $$PWD/../bb-apploader-develop
DEPENDPATH += $$PWD/../bb-apploader-develop
```

Hit the build button (the hammer) and wait for the compilation to be done. A new directory named `build-bb-apploader-cli/release` shall now have been created, with a binary file named `blinkyApploaderCLI`. Copy this file to your system's binary directory:

```
sudo cp ../build-bb-apploader-cli/release/blinkyApploaderCLI /usr/bin
```

You can now check if everything is correct by trying to run `blinkyApploaderCLI -h` from a terminal. You shall get the following result:

```
layer2: ctor
layer3::ctor
Usage: blinkyApploaderCLI
                         -h display this message
                         -s <SERIAL> sets serial interface name
                         -c <R,G,B> sets connected blinky to color R,G,B
                         -t applies command to blocks ensemble (builds a spanning tree)
                         -e erase configuration
                         -p <HEX file> starts sending HEX file to connected blinky ensemble
                         -j 0xAAAAAAAA jumps to application address AAAAAAAA
                         -k <ID min> sets blinky blocks software ID, starting with <ID min>
                         -g requests blinky block to send its configuration
                         -v displays current apploader version date
                         -V displays blinky blocks version and hardware IDs
                         -r reboots BB ensemble
                         -a <1|0> enables (1) or disables (0) the full parallel programming feature
                         -o clears spanning tree
                         -q enables application autoclose
                         -H <hex string with even symbols count> sends a L3_NORMAL packet with content equal to hex string
                         -w <VAR ID,VAR VALUE> sets blinky block variable <VAR ID> to value <VAR VALUE>
                            Application autostart -> 1
                            Application autostart delay -> 2
                            Commit configuration -> 128
                            Soft ID -> 3
layer3::dtor
layer2: dtor
```

## Allowing the user to access the serial port

The current user must have access to the serial port in order to use `blinkyApploaderCLI` with its communication with BBs. You simply have to add your user to the `dialout` group:

```bash
sudo usermod -aG dialout $USER
```

Then, reboot your machine or logout then login again, so you shall have access to `/dev/ttyUSB0` (or any other value it is given by your OS) with your user.

## Commands to test the setup

To test your setup, you need:

- a Blinky Blocks power supply,
- a USB/jack serial cable
- a programming block (same shape as a BB, with one face reserved for pluging the power supply and one face reserved for the jack serial connection)
- at least two Blinky Blocks with the latest bootloader.

Plus the power supply to an outlet and to the programming block. Plus the serial cable to a USB port of your computer and to the programming block. Plug the Blinky Blocks together. Finally, plug the programming block **on top** of one of the Blinky Blocks (only the bottom connector of the programming block carries the serial signal). Open a terminal and try the following command:

```bash
# Making a spanning tree
blinkyApploaderCLI -t -s /dev/ttyUSB0 -q
# Shall output something like:
Blinky block accepted me with BB count: 4
All tasks are processed
Exiting...
Quitting
```

BB count shall be equal to the number of Blinky Blocks you plugged. The `-s` option is not required if your serial port is `/dev/ttyUSB0` (default value in the application). For other values, you shall specify `-s` option.

Another way to check the spanning tree is to order the connected Blinky Blocks to set a color:

```bash
blinkyApploaderCLI -t -c 0,0,5 -s /dev/ttyUSB0 -q
# Shall output:
Blinky block accepted me with BB count: 4
All tasks are processed
Exiting...
Quitting
```

The output doesn't change, but all connected Blinky Blocks shall turn blue (a dark blue).

It is possible to set unique software identifiers (unique in one command execution) with the option `-k`. It takes as argument the first ID to be allocated to the Blinky Blocks. This ID will be allocated to the Blinky Block directly connected to the computer. The other IDs will be allocated according to the layout of the BB ensemble.

```bash
# Setting IDs starting from 1000
blinkyApploaderCLI -t -k 1000 -s /dev/ttyUSB0 -q
# Shall output:
Blinky block accepted me with BB count: 4
BB ensemble assigned ID's 1000 to 1003
All tasks are processed
Exiting...
Quitting
```

The result will indicate the value of the last (highest) ID allocated, which amounts to `starting_id + bb_count - 1`.

If all of the above commands provide results, you may proceed to the application writing, building and deployment.

# Setting up the ARM development tools

In this part, we will first install STM32CubeIDE to get all tools required to write and build code for ARM targets. Then, we'll compile the BB support library and use it with the template application. The last step described will be the deployment and execution of the application on a set of BBs.

## Installing STM32CubeIDE

Two solutions are possible to get the IDE: downloading and archive from STMicroelectronics website (or one provided by your teachers), or, for Linux, using Flatpak to get a packaged version of STM32CubeIDE.

### With the archive

### With `flatpak`

For Ubuntu, you will need to:

- Install Flatpak: `sudo apt install flatpak`
- Add Flathub to your Flatpak sources: `sudo flatpak remote-add --if-not-exists flathub https://flathub.org/repo/flathub.flatpakrepo`
- Install STM32CubeIDE: `sudo flatpak install com.st.STM32CubeIDE`

Flatpak generates launchers for some desktop environments (such as Gnome). In this case, you'll find a menu entry to launch STM32CubeIDE. If your desktop is not supported, you may launch a flatpak software with the following command: `flatpak run ID`. For STM32CubeIDE, it shall be `flatpak run com.st.STM32CubeIDE`.

Start the IDE once to set its workspace directory. It will create a workspace directory: `$HOME/STM32CubeIDE/workspace-xxxx` where `xxxx` is the installed version. Check the option to always use this as workspace.

## Writing an application

Blinky Blocks applications use a library providing all the basic BB stack (network, commands). This library must be compiled to be linked against applications' code.

The subsequent sections require to download (on Moodle; TODO: permanent links) files `bb-library-develop.zip` and `bb-applications-librarize-core.zip`.

### Compiling the library

Extract the file `bb-library-develop.zip` into the workspace directory. Then, use File -> Import to import the library. You shall select the option General -> Existing Projects into Workspace, then select the bb-library-develop folder and validate. A project appears in the left panel of the IDE.

In the toolbar, next to the hammer icon (used to build your project), click on the downwards arrow and choose **Release** as build target. Then hit the **Build** icon. After a couple second and a bunch of warnings, the library shall have been built.

If you encounter compilation errors about `caddr_t` being not defined, you shall switch your STM toolchain to version 10.3. You do this from your project's properties. Right click on the project, then click on _Properties_ (last entry of the menu), then go to _C/C++ Build_, then _Settings_. In settings, go to _MCU toolchain_, and click on button titled _Open Toolchain Manager_. It will lookup on remote sources, and list available versions. Choose and install _GNU Tools for STM32 (10.3-2021.10)_

You are now ready to proceeed to the next step.

### Compiling the application

Extract file `bb-applications-librarize-core.zip` into the workspace, and import it to the IDE like you did for the library.

The project must be parametered for accessing the library and generating the HEX file: right click on the project, and go to the properties:

- In **C/C++ Build**, then **Settings**, add to the MCU Compiler include paths the path to the `Inc` folder of the library.
- In the linker libraries, add the path to the `Release` folder of the library.
- Then, in the linker miscellanious, add the file `Release/libbb-library.a` to the additional object files.
- In the MCU post build outputs, check the *Convert to Intel Hex file* option,
- Select **Release** target (like you did for the library) in the toolbar.

You are now ready to write your application for the Blinky Blocks. For now, just add to the `bb_init` function in file `user_code.c` the following line:

```c
setColor(GREEN);
```

and compile your application. You are ready to go further to the next section.

### Uploading the application

Plug your BBs ensemble as described in the step following compilation of `blinkyApploaderCLI`, then send your program:

```bash
blinkyApploaderCLI -t -p /path/to/your/program.hex -s /dev/ttyUSB0 -q
# Final output:
Blinky block accepted me with BB count: 4
Chunks map ok
Starting programming at address 8010000 with program size 32156 in 252 chunks
99,60%
Programming finished!
iHexParser: dtor
All tasks are processed
Exiting...
Quitting
```

The Blinky Blocks shall turn purple, then green (with a growing lightness), then dark blue. Values from the output may vary according to the program size. You are now ready to run the application.

### Running the application

Running the application is simple:

```bash
blinkyApploaderCLI -t -j 0x8010000 -s /dev/ttyUSB0 -q
# Output:
Blinky block accepted me with BB count: 4
All tasks are processed
Exiting...
Quitting
```

Blinky Blocks shall turn green after startup.

If you managed to reach this stage, you are now ready to write your own applications. Feel free to read the README pages of the application to get information about Blinky Blocks API.
