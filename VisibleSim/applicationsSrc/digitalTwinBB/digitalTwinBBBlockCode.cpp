#include "digitalTwinBBBlockCode.hpp"
#include <sstream>
#include <vector>
#include <cstdint>
#include <iomanip>
#include <fcntl.h>
#include <unistd.h>
#include <thread>
#include <cstdio>
#include <memory>
#include <regex>
using namespace BlinkyBlocks;

/*
Reads data from /tmp/bbfifo, which contains messages from STM32CubeIDE (physical side).

Parses each message as hex bytes → extracts coordinates and color.

Creates blocks in VisibleSim (digital twin) at those positions and with those colors.

Implements a small broadcast algorithm (distance propagation) between virtual blocks.

Handles events and console commands for debugging or control.
*/

DigitalTwinBBBlockCode::DigitalTwinBBBlockCode(BlinkyBlocksBlock *host) : BlinkyBlocksBlockCode(host) {
    // @warning Do not remove block below, as a blockcode with a NULL host might be created
    //  for command line parsing
    if (not host) return;

    // Registers a callback (handleSampleMessage) to the message of type SAMPLE_MSG_ID
    addMessageEventFunc2(SAMPLE_MSG_ID,
                         std::bind(&DigitalTwinBBBlockCode::handleSampleMessage, this,
                                   std::placeholders::_1, std::placeholders::_2));

    // Set the module pointer
    module = static_cast<BlinkyBlocksBlock*>(hostBlock);
  }

void DigitalTwinBBBlockCode::startup() {
    console << "start";
    cout << "test\n";
    // Sample distance coloring algorithm below
    if (module->blockId == 1) { // Master ID is 1
        // runCommand("blinkyApploaderCLI -t -j 0x8010000 -s /dev/ttyUSB0 -q");
        // cout << "-j done rinning -t...\n";
        // runCommand("stdbuf -oL blinkyApploaderCLI -t -s /dev/ttyUSB0");
        readFifo();
        module->setColor(RED);
        distance = 0;
        sendMessageToAllNeighbors("Sample Broadcast",
                                  new MessageOf<int>(SAMPLE_MSG_ID,distance),100,200,0);
    } else {
        distance = -1; // Unknown distance
        hostBlock->setColor(LIGHTGREY);
    }

    // Additional initialization and algorithm start below
    // ...
}




void DigitalTwinBBBlockCode::readFifo() {
    const char * fifo_path = "/tmp/bbfifo";
    int fd = open(fifo_path, O_RDONLY);
    if (fd == -1) {
        cerr << "Error opening FIFO\n";
        return;
    }

    char ch;
    string line;
    while (true) {
        ssize_t bytes_read = read(fd, &ch, 1);
        if (bytes_read <= 0) break;

        if (ch == '\n') {
            if (!line.empty()) {
                cout << "Received: " << line << endl;

                // --- Only process lines that look like hex bytes ---
                static const regex hex_line("^[0-9A-Fa-f ]+$");
                if (regex_match(line, hex_line)) {
                    istringstream iss(line);
                    vector<int8_t> bytes;
                    string byteStr;
                    while (iss >> byteStr) {
                        try {
                            int value = stoi(byteStr, nullptr, 16);
                            bytes.push_back(static_cast<int8_t>(value));
                        } catch (const invalid_argument&) {
                            // ignore non-hex tokens
                        }
                    }

                    cout << "Parsed bytes: ";
                    for (auto b : bytes)
                        cout << setw(2) << setfill('0') << hex
                             << static_cast<int>(static_cast<uint8_t>(b)) << " ";
                    cout << dec << endl;

                    if (bytes.size() >= 5 && bytes[1] == 0) {
                        Cell3DPosition pos;
                        pos.pt[0] = bytes[2];
                        pos.pt[1] = bytes[3];
                        pos.pt[2] = bytes[4];
                        int color = bytes[5];
                        cout << "Cell3dPosition: " << pos << endl;
                        BaseSimulator::getWorld()->addBlock(0, DigitalTwinBBBlockCode::buildNewBlockCode, pos, BBColors[color]);
                    }
                } else {
                    // Non-hex message → just log it
                    cout << "(Non-hex message ignored)" << endl;
                }

                cout.flush();
                line.clear();
            }
        } else {
            line += ch;
        }
    }

    close(fd);
}


void DigitalTwinBBBlockCode::handleSampleMessage(std::shared_ptr<Message> _msg,
                                               P2PNetworkInterface* sender) {
    MessageOf<int>* msg = static_cast<MessageOf<int>*>(_msg.get());

    int d = *msg->getData() + 1;
    console << " received d =" << d << " from " << sender->getConnectedBlockId() << "\n";
                                        
    if (distance == -1 || distance > d) {
        console << " updated distance = " << d << "\n";
        distance = d;
        module->setColor(Colors[distance % NB_COLORS]);

        // Broadcast to all neighbors but ignore sender
        sendMessageToAllNeighbors("Sample Broadcast",
                                  new MessageOf<int>(SAMPLE_MSG_ID,distance),100,200,1,sender);
    }
}

void DigitalTwinBBBlockCode::onMotionEnd() {
    console << " has reached its destination" << "\n";

    // do stuff
    // ...
}

void DigitalTwinBBBlockCode::processLocalEvent(EventPtr pev) {
    std::shared_ptr<Message> message;
    stringstream info;

    // Do not remove line below
    BlinkyBlocksBlockCode::processLocalEvent(pev);

    switch (pev->eventType) {
        case EVENT_ADD_NEIGHBOR: {
            // Do something when a neighbor is added to an interface of the module
            break;
        }

        case EVENT_REMOVE_NEIGHBOR: {
            // Do something when a neighbor is removed from an interface of the module
            break;
        }
    }
}

/// ADVANCED BLOCKCODE FUNCTIONS BELOW

void DigitalTwinBBBlockCode::onBlockSelected() {
    // Debug stuff:
    cerr << endl << "--- PRINT MODULE " << *module << "---" << endl;
}

void DigitalTwinBBBlockCode::onAssertTriggered() {
    console << " has triggered an assert" << "\n";

    // Print debugging some info if needed below
    // ...
}

bool DigitalTwinBBBlockCode::parseUserCommandLineArgument(int &argc, char **argv[]) {
    /* Reading the command line */
 
    if ((argc > 0) && ((*argv)[0][0] == '-')) {
        switch((*argv)[0][1]) {

            // Single character example: -b
            case 'b':   {
                cout << "-b option provided" << endl;
                return true;
            } break;

            // Composite argument example: --foo 13
            case '-': {
                string varg = string((*argv)[0] + 2); // argv[0] without "--"
                if (varg == string("foo")) { //
                    int fooArg;
                    try {
                        fooArg = stoi((*argv)[1]);
                        argc--;
                        (*argv)++;
                    } catch(std::logic_error&) {
                        stringstream err;
                        err << "foo must be an integer. Found foo = " << argv[1] << endl;
                        throw CLIParsingError(err.str());
                    }

                    cout << "--foo option provided with value: " << fooArg << endl;
                } else return false;

                return true;
            }

            default: cerr << "Unrecognized command line argument: " << (*argv)[0] << endl;
        }
    }

    return false;
}

string DigitalTwinBBBlockCode::onInterfaceDraw() {
    stringstream trace;
    trace << "Some value " << 123;
    return trace.str();
}

void DigitalTwinBBBlockCode::runCommandAsync(const string& cmd) {
    thread([cmd]() {
        unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
        if (!pipe) {
            cerr << "Failed to start command: " << cmd << endl;
            return;
        }

        char buffer[256];
        string line;
        static const regex hex_line("^[0-9A-Fa-f ]+$");

        while (fgets(buffer, sizeof(buffer), pipe.get()) != nullptr) {
            line = buffer;
            if (!line.empty() && line.back() == '\n') line.pop_back();

            cout << "Received: " << line << endl;

            // Parse only hex lines
            if (regex_match(line, hex_line)) {
                istringstream iss(line);
                vector<int8_t> bytes;
                string byteStr;
                while (iss >> byteStr) {
                    try {
                        int value = stoi(byteStr, nullptr, 16);
                        bytes.push_back(static_cast<int8_t>(value));
                    } catch (...) {}
                }

                cout << "Parsed bytes: ";
                for (auto b : bytes)
                    cout << hex << static_cast<int>(static_cast<uint8_t>(b)) << " ";
                cout << dec << endl;
            }
        }
    }).detach();  // Detach so it runs independently
}

void DigitalTwinBBBlockCode::runCommand(const string &cmd) {
    cout << ">>> Running command: " << cmd << endl;

    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe) {
        cerr << "Failed to start command" << endl;
        return;
    }

    char ch;
    string line;
    static const regex hex_line("^[0-9A-Fa-f ]+$");

    while (fread(&ch, 1, 1, pipe) == 1) { // read one char at a time
        if (ch == '\n') {
            if (!line.empty()) {
                cout << "Received: " << line << endl;

                // --- If line contains only hex bytes ---
                if (regex_match(line, hex_line)) {
                    istringstream iss(line);
                    vector<int8_t> bytes;
                    string byteStr;
                    while (iss >> byteStr) {
                        try {
                            int value = stoi(byteStr, nullptr, 16);
                            bytes.push_back(static_cast<int8_t>(value));
                        } catch (...) {}
                    }

                    cout << "Parsed bytes: ";
                    for (auto b : bytes)
                        cout << setw(2) << setfill('0') << hex
                             << static_cast<int>(static_cast<uint8_t>(b)) << " ";
                    cout << dec << endl;

                    // Example: replicate readFifo logic
                    if (bytes.size() >= 5 && bytes[1] == 0) {
                        Cell3DPosition pos;
                        pos.pt[0] = bytes[2];
                        pos.pt[1] = bytes[3];
                        pos.pt[2] = bytes[4];
                        cout << "Cell3dPosition: " << pos << endl;
                        int color = bytes[5];
                        cout << "Cell3dPosition: " << pos << endl;
                        BaseSimulator::getWorld()->addBlock(0, DigitalTwinBBBlockCode::buildNewBlockCode, pos, BBColors[color]);
                    }
                } else {
                    cout << "(Non-hex message ignored)" << endl;
                }

                line.clear();
            }
        } else {
            line += ch;
        }
    }

    pclose(pipe);
    cout << ">>> Command finished" << endl;
}