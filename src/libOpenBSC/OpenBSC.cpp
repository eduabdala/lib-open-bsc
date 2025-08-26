#include "OpenBSC.hpp"
#include <cstring>
#include <iostream>
#include <chrono>
#include <thread>

const uint16_t MAX_BUFF_SIZE = 1024;
const uint8_t STX = 0x02;
const uint8_t ETX = 0x03;

OpenBSC::OpenBSC() = default;

/**
 * @brief Initializes the OpenBSC instance with serial port settings.
 * @param portName Name of the serial port (e.g., "COM3" or "/dev/ttyUSB0")
 * @param baudRate Baud rate for communication
 * @param byte_size Number of data bits per byte
 * @param stop_bits Number of stop bits
 * @param parity Parity ('N' = none, 'E' = even, 'O' = odd)
 * @param use_rts Enable RTS flow control
 * @param use_dtr Enable DTR flow control
 * @return true if initialization succeeded, false otherwise
 */
bool OpenBSC::Init(const char* portName, uint32_t baudRate, uint8_t byte_size, uint8_t stop_bits, char parity, bool use_rts, bool use_dtr)
{
    serial = SerialCommunication::Create(
        std::string(portName),
        baudRate,
        byte_size,
        stop_bits,
        parity,
        use_rts,
        use_dtr
    );
    return serial != nullptr;
}

/**
 * @brief Opens the serial port.
 * @param comSerial Serial port name
 * @return true if the port was successfully opened, false otherwise
 */
bool OpenBSC::Open(const char* comSerial)
{
    if (!comSerial || !serial) return false;
    return serial->Open();
}

/**
 * @brief Calculates the BCC (Block Check Character) for the given data.
 * @param data Pointer to the data buffer
 * @param length Number of bytes in the buffer
 * @return Calculated BCC value
 */
uint8_t OpenBSC::CalculateBCC(const uint8_t* data, uint32_t length)
{
    uint8_t bcc = 0;
    for (uint32_t i = 0; i < length; ++i)
        bcc ^= data[i];
    return bcc;
}

/**
 * @brief Sends a command packet using OpenBSC protocol.
 * @param command Pointer to the command string
 * @param length Length of the command
 * @return true if the command was successfully sent, false otherwise
 */
bool OpenBSC::SendCommand(const char* command, uint32_t length)
{
    if (!serial || !command || length == 0) return false;

    uint8_t packet[MAX_BUFF_SIZE] = {0};
    uint32_t packetSize = 0;

    packet[packetSize++] = STX;
    std::memcpy(&packet[packetSize], command, length);
    packetSize += length;
    packet[packetSize++] = ETX;

    uint8_t bcc = CalculateBCC(&packet[1], length + 1);
    packet[packetSize++] = bcc;

    std::size_t written = serial->Write(packet, packetSize);
    if (written != packetSize) return false;

    return true;
}

/**
 * @brief Reads a response packet from the serial port using OpenBSC protocol.
 * @param buffer Buffer to store the received payload
 * @param maxLength Maximum number of bytes to read
 * @param timeout_ms Timeout in milliseconds to wait for response
 * @return Number of bytes read into the buffer, 0 on failure or timeout
 */
uint32_t OpenBSC::ReadResponse(char* buffer, uint32_t maxLength, uint32_t timeout_ms)
{
    if (!serial || !buffer || maxLength == 0) return 0;

    uint8_t raw[MAX_BUFF_SIZE] = {0};
    uint32_t idx = 0;
    auto start = std::chrono::steady_clock::now();

    while (true) {
        uint8_t byte;
        std::size_t read = serial->Read(&byte, 1, 10);
        if (read == 1 && idx < MAX_BUFF_SIZE) raw[idx++] = byte;

        auto now = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count() > timeout_ms)
            break;
    }

    uint8_t* stxPtr = static_cast<uint8_t*>(std::memchr(raw, STX, idx));
    uint8_t* etxPtr = stxPtr ? static_cast<uint8_t*>(std::memchr(stxPtr + 1, ETX, idx - (stxPtr - raw) - 1)) : nullptr;
    if (!stxPtr || !etxPtr || etxPtr <= stxPtr) return 0;

    uint8_t expectedBcc = *(etxPtr + 1);
    uint8_t calcBcc = CalculateBCC(stxPtr + 1, etxPtr - stxPtr);
    if (expectedBcc != calcBcc) return 0;

    uint32_t payloadLen = etxPtr - stxPtr - 1;
    if (payloadLen > maxLength) payloadLen = maxLength;
    std::memcpy(buffer, stxPtr + 1, payloadLen);
    buffer[payloadLen] = '\0';

    return payloadLen;
}

/**
 * @brief Disconnects and releases the serial port.
 * @return true if disconnection was successful, false if already disconnected
 */
bool OpenBSC::Disconnect()
{
    if (serial) {
        serial->Close();
        serial.reset();
        return true;
    }
    return false;
}
