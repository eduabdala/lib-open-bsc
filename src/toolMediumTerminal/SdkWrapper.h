#ifndef SDK_WRAPPER_H
#define SDK_WRAPPER_H

#include <string>
#include "libOpenBSC.h"

bool open_and_init_sdk(const std::string& serial, int baudrate, bool rts, bool dtr);
ComPortList_s list_ports_sdk(uint16_t vid, uint16_t pid);

#endif
