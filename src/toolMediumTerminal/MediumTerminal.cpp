#include "MediumTerminal.h"
#include "SdkWrapper.h"
#include <iostream>
#include <getopt.h>
#include <string>
#include <cstdlib>

namespace MediumTerminalUtils {
    void printUsage(const char* progName) {
        std::cout << "Usage: " << progName << " [-c COM_PORT | -p PID] [-v VID] [-x COMMAND] [-b BAUD] [--rts] [--dtr]\n"
                  << "  OpenBSC Medium Terminal is a USB and Serial communication CLI utilizing OPEN BSC PROTOCOL\n\n"
                  << "  Required config options:\n\n"
                  << "  -c <COM_PORT> | --com <COM_PORT>   Specify COM port (e.g., COM5)\n"
                  << "  -p <PID>      | --pid <PID>             Specify PID to search devices (VID optional)\n"
                  << "  -v <VID>      | --vid <VID>             Specify VID (default: 0x1ABD)\n"
                  << "                                          Use either -c <COM_PORT> or -p <PID>, but not both\n"
                  << "  -x <COMMAND>  | --command <COMMAND>     Command to send (e.g., V)\n\n"
                  << "  Optional config options:\n\n"
                  << "  -b <BAUD>     | --baudrate <BAUD>       Baudrate (default: 115200)\n"
                  << "  --rts                                  Enable RTS\n"
                  << "  --dtr                                  Enable DTR\n";
    }
}

int MediumTerminal::run(int argc, char *argv[])
{
    std::string comPort;
    uint16_t    vid = 0x1ABD;
    uint16_t    pid = 0;
    std::string command;
    bool        usePid   = false;
    bool        rts      = false;
    bool        dtr      = false;
    int         baudrate = 115200;

    const struct option long_options[] = {
        {    "help",       no_argument, nullptr, 'h'},
        {     "com", required_argument,       0, 'c'},
        {     "pid", required_argument,       0, 'p'},
        {     "vid", required_argument,       0, 'v'},
        { "command", required_argument,       0, 'x'},
        {"baudrate", required_argument,       0, 'b'},
        {     "rts",       no_argument, nullptr, 'r'},
        {     "dtr",       no_argument, nullptr, 'd'},
        {   nullptr,                 0, nullptr,   0}
    };

    int opt, long_index = 0;
    while ((opt = getopt_long(argc, argv, "c:p:v:x:b:rd", long_options, &long_index)) != -1)
    {
        switch (opt)
        {
            case 'h':
                MediumTerminalUtils::printUsage(argv[0]);
                return 0;
            case 'c':
                comPort = optarg;
                break;
            case 'p':
                pid    = static_cast<uint16_t>(strtoul(optarg, nullptr, 0));
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

    if ((!comPort.empty() && usePid) || (comPort.empty() && !usePid))
    {
        std::cerr << "Error: Use either -c <COM_PORT> or -p <PID>, but not both.\n";
        MediumTerminalUtils::printUsage(argv[0]);
        return 1;
    }

    if (command.empty())
    {
        std::cerr << "Error: Command (-x) is required.\n";
        MediumTerminalUtils::printUsage(argv[0]);
        return 1;
    }

    std::string serial;
    if (usePid)
    {
        ComPortList_s ports = list_ports_sdk(vid, pid);
        if (ports.ComPort[0].serial[0] == '\0')
        {
            std::cerr << "No COM port found for PID " << pid << " (VID " << vid << ").\n";
            return 1;
        }
        serial = ports.ComPort[0].serial;
    }
    else
    {
        serial = comPort;
    }

    if (!open_and_init_sdk(serial, baudrate, rts, dtr))
    {
        return 1;
    }

    CommandOutcome_s response = OpenBSCSDKSend(command.c_str());
    if (response.error != NONE)
    {
        std::cerr << "Command failed: " << response.error << "\n";
    }
    else
    {
        std::cout << response.answer << '\n';
    }

    OpenBSCSDKClose();
    return 0;
}