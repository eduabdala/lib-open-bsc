/**
 * @file OpenBSC.hpp
 * @author Eduardo Abdala
 * @brief Header file of OpenBSC
 * @version 0.1
 * @date 2025-08-09
 * @copyright Copyright (c) 2025
 */
#ifndef OPENBSC_H
#define OPENBSC_H

#include "Serial.hpp"
#include <cstdint>
#include <memory>
#include <cstring>

/**
 * @brief Class for managing Open BSC protocol with a device via a serial interface.
 */
class OpenBSC
{
  public:
    /**
     * @brief Constructs a new OpenBSC object.
     */
    OpenBSC();

    /**
     * @brief Initializes the serial communication settings.
     * 
     * This method internally creates a configured SerialCommunication instance.
     * The port is not opened yet — use Open() afterwards.
     *
     * @param[in] portName: The port name (e.g., "COM3" or "/dev/ttyUSB0").
     * @param[in] baudRate: The baud rate for communication.
     * @param[in] byte_size: The number of data bits per character.
     * @param[in] stop_bits: The number of stop bits used in the communication.
     * @param[in] parity: The parity setting ('N' for none, 'E' for even, 'O' for odd).
     * @param[in] use_rts: Flag indicating whether to enable the RTS signal.
     * @param[in] use_dtr: Flag indicating whether to enable the DTR signal.
     * @return true if the initialization was successful;
     *         false otherwise.
     */
    bool Init(const char* portName, uint32_t baudRate, uint8_t byte_size, uint8_t stop_bits, char parity, bool use_rts, bool use_dtr);

    /**
     * @brief Opens a serial communication port.
     * @param[in] comSerial: The port name of the serial port.
     * @return true if the port was successfully opened;
     *         false otherwise.
     */
    bool Open(const char* comSerial);

    /**
     * @brief Sends a command to the connected device.
     * @param[in] command: The command string to be sent.
     * @param[in] length: The length of the command string.
     * @return true if the command was successfully sent;
     *         false otherwise.
     */
    bool SendCommand(const char* command, uint32_t length);

    /**
     * @brief Reads the response from the connected device.
     * 
     * Reads bytes from the serial port and extracts the payload according to the OpenBSC protocol.
     * If `filtered` is true, the STX, ETX, and BCC bytes are removed and only the payload is returned.
     * 
     * @param[out] buffer The buffer to store the received payload.
     * @param[in] maxLength Maximum number of bytes that can be stored in buffer.
     * @param[in] timeout_ms Timeout in milliseconds to wait for the response.
     * @return uint32_t Number of bytes successfully read into buffer. Returns 0 if timeout occurs or BCC is invalid.
     */
    uint32_t ReadResponse(char* buffer, uint32_t maxLength, uint32_t timeout_ms);

    /**
     * @brief Disconnects the serial communication.
     * @return true if the port was successfully closed;
     *         false otherwise.
     */
    bool Disconnect();

  private:
    /**
     * @brief Calculates the Block Check Character (BCC).
     * @param[in] data: Pointer to the data array for which the BCC is to be calculated.
     * @param[in] length: The number of bytes in the data array.
     * @return The calculated BCC value.
     */
    uint8_t CalculateBCC(const uint8_t* data, uint32_t length);

    std::shared_ptr<SerialCommunication> serial; ///< Smart pointer to SerialCommunication object.
};

#endif // OPENBSC_HPP
