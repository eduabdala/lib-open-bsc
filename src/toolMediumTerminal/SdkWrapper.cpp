#include "SdkWrapper.h"
#include <iostream>

bool open_and_init_sdk(const std::string& serial, int baudrate, bool rts, bool dtr) {
    errorList_e initError = OpenBSCSDKInit(serial.c_str(), baudrate, 8, 1, 'N', rts, dtr);
    if (initError != NONE) {
        std::cerr << "Error initializing SDK: " << initError << "\n";
        return false;
    }
    
    errorList_e openError = OpenBSCSDKOpen(serial.c_str());
    if (openError != NONE) {
        std::cerr << "Failed to open port: " << openError << "\n";
        return false;
    }

    return true;
}

ComPortList_s list_ports_sdk(uint16_t vid, uint16_t pid) {
    return listPortSDK(vid, pid);
}
