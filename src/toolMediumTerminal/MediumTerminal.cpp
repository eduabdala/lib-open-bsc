#include "MediumTerminal.h"
#include <iostream>
#include <getopt.h>
#include <string>
#include <cstdlib>
#include "SdkWrapper.h"
#include <OpenBSC.hpp>

namespace MediumTerminalUtils {
    /**
     * @brief Prints the usage/help message for MediumTerminal CLI
     * @param progName Name of the executable
     */
    void printUsage(const char* progName) {
        std::cout << "Usage: " << progName << " [-c COM_PORT | -p PID] [-v VID] [-x COMMAND] [-b BAUD] [--rts] [--dtr]\n"
                  << "  OpenBSC Medium Terminal is a USB and Serial communication CLI utilizing OPEN BSC PROTOCOL\n\n"
                  << "  Required config options:\n\n"
                  << "  -c <COM_PORT> | --com <COM_PORT>   Specify COM port (e.g., COM5)\n"
                  << "  -p <PID>      | --pid <PID>        Specify PID to search devices (VID optional)\n"
                  << "  -v <VID>      | --vid <VID>        Specify VID (default: 0x1ABD)\n"
                  << "                                      Use either -c <COM_PORT> or -p <PID>, but not both\n"
                  << "  -x <COMMAND>  | --command <COMMAND> Command to send (e.g., V)\n\n"
                  << "  Optional config options:\n\n"
                  << "  -b <BAUD>     | --baudrate <BAUD>  Baudrate (default: 115200)\n"
                  << "  --rts                               Enable RTS\n"
                  << "  --dtr                               Enable DTR\n";
    }
}

int MediumTerminal::run(int argc, char *argv[])
{
    std::string comPort;
    uint16_t vid = 0x1ABD;            // Default vendor ID
    uint16_t pid = 0;                  // Product ID
    std::string command;               // Command to send
    bool usePid = false;               // Flag if PID search is used
    bool rts = false;                  // RTS control
    bool dtr = false;                  // DTR control
    int baudrate = 115200;             // Default baudrate

    // Define long options for getopt
    const struct option long_options[] = {
        {"help", no_argument, nullptr, 'h'},
        {"com", required_argument, nullptr, 'c'},
        {"pid", required_argument, nullptr, 'p'},
        {"vid", required_argument, nullptr, 'v'},
        {"command", required_argument, nullptr, 'x'},
        {"baudrate", required_argument, nullptr, 'b'},
        {"rts", no_argument, nullptr, 'r'},
        {"dtr", no_argument, nullptr, 'd'},
        {nullptr, 0, nullptr, 0}
    };

    // Parse command-line arguments
    int opt, long_index = 0;
    while ((opt = getopt_long(argc, argv, "c:p:v:x:b:rd", long_options, &long_index)) != -1) {
        switch (opt) {
            case 'h': 
                MediumTerminalUtils::printUsage(argv[0]); 
                return 0;
            case 'c': 
                comPort = optarg; 
                break;
            case 'p': 
                pid = static_cast<uint16_t>(strtoul(optarg, nullptr, 0)); 
                usePid = true; 
                break;
            case 'v': 
                vid = static_cast<uint16_t>(strtoul(optarg, nullptr, 0)); 
                break;
            case 'x': 
                command = optarg; 
                break;
            case 'b': 
                baudrate = std::stoi(optarg); 
                break;
            case 'r': 
                rts = true; 
                break;
            case 'd': 
                dtr = true; 
                break;
            default: 
                MediumTerminalUtils::printUsage(argv[0]); 
                return 1;
        }
    }

    // Validate COM/PID options
    if ((!comPort.empty() && usePid) || (comPort.empty() && !usePid)) {
        std::cerr << "Error: Use either -c <COM_PORT> or -p <PID>, but not both.\n";
        MediumTerminalUtils::printUsage(argv[0]);
        return 1;
    }

    // Command is required
    if (command.empty()) {
        std::cerr << "Error: Command (-x) is required.\n";
        MediumTerminalUtils::printUsage(argv[0]);
        return 1;
    }

    // Resolve serial port
    std::string serial;
    if (usePid) {
        ComPortList_s ports = list_ports_sdk(vid, pid);
        if (ports.ComPort[0].serial[0] == '\0') {
            std::cerr << "No COM port found for PID " << pid << " (VID " << vid << ").\n";
            return 1;
        }
        serial = ports.ComPort[0].serial;
    } else {
        serial = comPort;
    }

    // Initialize OpenBSC instance
    OpenBSC bsc;
    if (!bsc.Init(serial.c_str(), baudrate, 8, 1, 'N', rts, dtr) || !bsc.Open(serial.c_str())) {
        std::cerr << "Failed to open serial port " << serial << "\n";
        return 1;
    }

    // Send the command
    if (!bsc.SendCommand(command.c_str(), static_cast<uint32_t>(command.length()))) {
        std::cerr << "Failed to send command.\n";
        bsc.Disconnect();
        return 1;
    }

    // Read response from device
    char response[1024] = {0};
    uint32_t len = bsc.ReadResponse(response, sizeof(response), 1000);

    if (len == 0) {
        std::cerr << "No response or invalid BCC.\n";
    } else {
        std::cout << response << "\n";
    }

    bsc.Disconnect();
    return 0;
}
