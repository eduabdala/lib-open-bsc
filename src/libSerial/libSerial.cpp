#include "libSerial.h"
#include "Serial.hpp"

#include <vector>
#include <memory>
#include <cstring>
#include <stdexcept>

// Static vector holding instances of SerialCommunication
static std::vector<std::shared_ptr<SerialCommunication>> s_instances;

extern "C"
{

/**
 * @brief Lists available serial ports, optionally filtered by USB Vendor ID and Product ID.
 *
 * Currently not implemented (TODO).
 *
 * @param vendorId USB Vendor ID filter (0 disables filtering).
 * @param productId USB Product ID filter (0 disables filtering).
 * @return SerialCommPortList Structure containing the list of ports found.
 */
BSC_SDK_EXPORT SerialCommPortList SerialCommListPorts(uint16_t vendorId, uint16_t productId)
{
    SerialCommPortList list{};
    // TODO: implement port enumeration
    (void)vendorId; // to avoid unused parameter warning
    (void)productId;
    return list;
}

/**
 * @brief Initializes and creates a configured SerialCommunication instance.
 *
 * Adds the created instance to the internal vector for instance management.
 *
 * @param baudRate Communication baud rate.
 * @param dataBits Number of data bits.
 * @param stopBits Number of stop bits.
 * @param parity Parity mode: 'N' (None), 'E' (Even), 'O' (Odd).
 * @param enableRts Enable RTS line.
 * @param enableDtr Enable DTR line.
 * @param portSerial Serial port name (e.g., "COM3" or "/dev/ttyUSB0").
 * @param outError Optional pointer to receive error code.
 * @return int Instance ID (index in vector) or -1 on failure.
 */
BSC_SDK_EXPORT int SerialCommInit(uint32_t baudRate, uint8_t dataBits, uint8_t stopBits, char parity,
                                 bool enableRts, bool enableDtr, const char *portSerial, SerialCommError *outError)
{
    if (!portSerial)
    {
        if (outError)
            *outError = SCErrorInvalidFormat;
        return -1;
    }

    auto inst = SerialCommunication::Create(portSerial, baudRate, dataBits, stopBits, parity, enableRts, enableDtr);
    if (!inst)
    {
        if (outError)
            *outError = SCErrorPortNotFound;
        return -1;
    }

    // Reuse empty slots if any
    for (size_t i = 0; i < s_instances.size(); ++i)
    {
        if (!s_instances[i])
        {
            s_instances[i] = inst;
            if (outError)
                *outError = SCErrorNone;
            return static_cast<int>(i);
        }
    }

    // Otherwise, add at the end
    s_instances.push_back(inst);
    if (outError)
        *outError = SCErrorNone;
    return static_cast<int>(s_instances.size() - 1);
}

/**
 * @brief Closes the serial port for the specified instance.
 *
 * @param instanceId Instance ID.
 * @return SerialCommError Error code, SCErrorNone if success.
 */
BSC_SDK_EXPORT SerialCommError SerialCommClose(int instanceId)
{
    if (instanceId < 0 || static_cast<size_t>(instanceId) >= s_instances.size() || !s_instances[instanceId])
    {
        return SCErrorInvalidFormat;
    }

    s_instances[instanceId]->Close();
    return SCErrorNone;
}

/**
 * @brief Deinitializes the instance, closing port and releasing resources.
 *
 * @param instanceId Instance ID.
 * @return SerialCommError Error code, SCErrorNone if success.
 */
BSC_SDK_EXPORT SerialCommError SerialCommDeinit(int instanceId)
{
    if (instanceId < 0 || static_cast<size_t>(instanceId) >= s_instances.size() || !s_instances[instanceId])
    {
        return SCErrorInvalidFormat;
    }

    s_instances[instanceId]->Close();
    s_instances[instanceId].reset();
    return SCErrorNone;
}

/**
 * @brief Opens the serial port for the specified instance.
 *
 * @param instanceId Instance ID.
 * @return SerialCommError Error code, SCErrorNone if success.
 */
BSC_SDK_EXPORT SerialCommError SerialCommOpen(int instanceId)
{
    if (instanceId < 0 || static_cast<size_t>(instanceId) >= s_instances.size() || !s_instances[instanceId])
    {
        return SCErrorInvalidFormat;
    }

    return s_instances[instanceId]->Open() ? SCErrorNone : SCErrorOpenFailed;
}

/**
 * @brief Writes data to the serial port of the specified instance.
 *
 * @param instanceId Instance ID.
 * @param buffer Pointer to data to write.
 * @param length Number of bytes to write.
 * @param outError Optional pointer to receive error code.
 * @return size_t Number of bytes written (0 on failure).
 */
BSC_SDK_EXPORT size_t SerialCommWrite(int instanceId, const void *buffer, size_t length, SerialCommError *outError)
{
    if (instanceId < 0 || static_cast<size_t>(instanceId) >= s_instances.size() ||
        !s_instances[instanceId] || !buffer || length == 0)
    {
        if (outError)
            *outError = SCErrorInvalidFormat;
        return 0;
    }

    try
    {
        s_instances[instanceId]->Write(buffer, length);
        if (outError)
            *outError = SCErrorNone;
        return length;
    }
    catch (const std::runtime_error &)
    {
        if (outError)
            *outError = SCErrorOpenFailed;
        return 0;
    }
}

/**
 * @brief Reads data from the serial port of the specified instance with a fixed 1000ms timeout.
 *
 * @param instanceId Instance ID.
 * @param buffer Pointer to buffer where read data will be stored.
 * @param length Maximum number of bytes to read.
 * @param outError Optional pointer to receive error code.
 * @return size_t Number of bytes read (0 on timeout or failure).
 */
BSC_SDK_EXPORT size_t SerialCommRead(int instanceId, void *buffer, size_t length, SerialCommError *outError)
{
    if (instanceId < 0 || static_cast<size_t>(instanceId) >= s_instances.size() ||
        !s_instances[instanceId] || !buffer || length == 0)
    {
        if (outError)
            *outError = SCErrorInvalidFormat;
        return 0;
    }

    try
    {
        size_t bytes = s_instances[instanceId]->Read(buffer, length, 1000);
        if (outError)
            *outError = SCErrorNone;
        return bytes;
    }
    catch (const std::runtime_error &)
    {
        if (outError)
            *outError = SCErrorNoData;
        return 0;
    }
}

/**
 * @brief Flushes input and output buffers of the specified instance.
 *
 * @param instanceId Instance ID.
 * @return SerialCommError Error code, SCErrorNone if success.
 */
BSC_SDK_EXPORT SerialCommError SerialCommFlush(int instanceId)
{
    if (instanceId < 0 || static_cast<size_t>(instanceId) >= s_instances.size() || !s_instances[instanceId])
    {
        return SCErrorInvalidFormat;
    }

    try
    {
        s_instances[instanceId]->Flush();
        return SCErrorNone;
    }
    catch (const std::runtime_error &)
    {
        return SCErrorNoData;
    }
}

} // extern "C"
