/**
 * @file libOpenBSC.h
 * @author Eduardo Abdala
 * @brief Header of libOpenBSC file
 * @version 0.1
 * @date 2025-08-09
 * @copyright Copyright (c) 2025
 */
#ifndef LIBOPENBSC_H
#define LIBOPENBSC_H

#include <stddef.h>
#include <stdint.h>

#if defined(_WIN32)
#if defined(BSC_SDK_EXPORT)
#define BSC_SDK_EXPORT __declspec(dllexport)
#else
#define BSC_SDK_EXPORT __declspec(dllimport)
#endif
#else
#define BSC_SDK_EXPORT __attribute__((visibility("default")))
#endif

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Enum with possible error's occurred label
     */
    enum errorList_e
    {
        NONE = 0,
        INVALID_FORMAT,
        NO_DATA_RECEIVED,
        PORT_NOT_FOUND,
        PORT_OPEN_FAILED,
        CONFIG_FAILED,
        SEND_FAILED
    };

    /**
     * @brief Structure representing the command outcome.
     */
    struct CommandOutcome_s
    {
        char             answer[1024]; ///< Buffer containing the command's response.
        enum errorList_e error;        ///< Variable to store the error code.
    };

    /**
     * @brief Structure representing a list of communication ports.
     */
    struct ComPortList_s
    {
        struct ComPort_s
        {
            char name[10];   ///< Name of the communication port.
            char serial[32]; ///< Serial identifier associated with the communication port.
        } ComPort[10];       ///< Fixed-size array holding up to 10 communication port entries.
    };

    BSC_SDK_EXPORT struct ComPortList_s    listPortSDK(uint16_t VID, uint16_t PID);
    BSC_SDK_EXPORT enum errorList_e        OpenBSCSDKInit(const char* comSerial, uint32_t baudRate, uint8_t byte_size, uint8_t stop_bits, char parity, bool use_rts,
                                                                bool use_dtr);
    BSC_SDK_EXPORT enum errorList_e        OpenBSCSDKOpen(const char *comSerial);
    BSC_SDK_EXPORT void                    OpenBSCSDKClose(void);
    BSC_SDK_EXPORT struct CommandOutcome_s OpenBSCSDKSend(const char *cmd);

#ifdef __cplusplus
}
#endif

#endif // LIBOPENBSC_H
