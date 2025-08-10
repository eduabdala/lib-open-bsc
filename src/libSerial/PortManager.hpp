#ifndef PORT_MANAGER_HPP
#define PORT_MANAGER_HPP

#include <vector>
#include <string>

/**
 * @brief Lists available serial ports on the system.
 * 
 * On Windows, uses SetupAPI to find COM ports (e.g., "COM1", "COM3").
 * On Linux, scans /dev for serial devices (e.g., "/dev/ttyS0", "/dev/ttyUSB0").
 * 
 * @return Vector of serial port names as strings.
 */
std::vector<std::string> FindSerialPorts(uint16_t VID, uint16_t PID);

#endif // PORT_MANAGER_HPP
