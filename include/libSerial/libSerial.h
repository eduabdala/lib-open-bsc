#ifndef LIBSERIAL_H
#define LIBSERIAL_H

#ifndef BSC_SDK_EXPORT
#ifdef _WIN32
#define BSC_SDK_EXPORT __declspec(dllexport)
#else
#define BSC_SDK_EXPORT __attribute__((visibility("default")))
#endif
#endif

#include <cstdint>
#include <cstddef>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief List of available serial ports.
 * 
 * @struct SerialCommPortList
 * @param ports Pointer to array of C-string port names.
 * @param count Number of entries in @c ports.
 */
struct SerialCommPortList {
    char** ports;
    size_t  count;
};

/**
 * @enum SerialCommError
 * @brief Error codes for serial operations.
 * 
 * @var SCErrorNone
 *   No error.
 * @var SCErrorInvalidFormat
 *   Invalid parameter.
 * @var SCErrorPortNotFound
 *   Port not found.
 * @var SCErrorOpenFailed
 *   Failed to open port.
 * @var SCErrorNoData
 *   No data available.
 */
typedef enum {
    SCErrorNone = 0,
    SCErrorInvalidFormat,
    SCErrorPortNotFound,
    SCErrorOpenFailed,
    SCErrorNoData
} SerialCommError;

/**
 * @brief Enumerate available serial ports.
 * 
 * @param[in]  vendorId   USB vendor ID filter.
 * @param[in]  productId  USB product ID filter.
 * @return     SerialCommPortList List of matching ports.
 */
BSC_SDK_EXPORT SerialCommPortList SerialCommListPorts(uint16_t vendorId,
                                                     uint16_t productId);

/**
 * @brief Initialize a serial instance.
 * 
 * @param[in]   baudRate    Connection speed.
 * @param[in]   dataBits    Number of data bits.
 * @param[in]   stopBits    Number of stop bits.
 * @param[in]   parity      Parity: 'N','E','O'.
 * @param[in]   enableRts   Enable RTS line.
 * @param[in]   enableDtr   Enable DTR line.
 * @param[in]   portSerial  Port name string.
 * @param[out]  outError    Error code output.
 * @return      int         Instance ID or -1 on failure.
 */
BSC_SDK_EXPORT int SerialCommInit(uint32_t        baudRate,
                                 uint8_t         dataBits,
                                 uint8_t         stopBits,
                                 char            parity,
                                 bool            enableRts,
                                 bool            enableDtr,
                                 const char*     portSerial,
                                 SerialCommError* outError);

/**
 * @brief Close the port without destroying the instance.
 * 
 * @param[in]  instanceId  ID from SerialCommInit.
 * @return     SerialCommError Error code.
 */
BSC_SDK_EXPORT SerialCommError SerialCommClose(int instanceId);

/**
 * @brief Destroy a serial instance.
 * 
 * @param[in]  instanceId  ID from SerialCommInit.
 * @return     SerialCommError Error code.
 */
BSC_SDK_EXPORT SerialCommError SerialCommDeinit(int instanceId);

/**
 * @brief Open the serial port.
 * 
 * @param[in]  instanceId  ID from SerialCommInit.
 * @return     SerialCommError Error code.
 */
BSC_SDK_EXPORT SerialCommError SerialCommOpen(int instanceId);

/**
 * @brief Write data to the serial port.
 * 
 * @param[in]   instanceId  ID from SerialCommInit.
 * @param[in]   buffer      Pointer to data to send.
 * @param[in]   length      Number of bytes to write.
 * @param[out]  outError    Error code output.
 * @return      size_t      Number of bytes written.
 */
BSC_SDK_EXPORT size_t SerialCommWrite(int             instanceId,
                                     const void*     buffer,
                                     size_t          length,
                                     SerialCommError* outError);

/**
 * @brief Read data from the serial port.
 * 
 * @param[in]   instanceId  ID from SerialCommInit.
 * @param[out]  buffer      Buffer to receive data.
 * @param[in]   length      Maximum bytes to read.
 * @param[out]  outError    Error code output.
 * @return      size_t      Number of bytes read.
 */
BSC_SDK_EXPORT size_t SerialCommRead(int       instanceId,
                                    void*     buffer,
                                    size_t    length,
                                    SerialCommError* outError);

/**
 * @brief Flush port buffers.
 * 
 * @param[in]  instanceId  ID from SerialCommInit.
 * @return     SerialCommError Error code.
 */
BSC_SDK_EXPORT SerialCommError SerialCommFlush(int instanceId);

#ifdef __cplusplus
}
#endif

#endif // LIBSERIAL_H
