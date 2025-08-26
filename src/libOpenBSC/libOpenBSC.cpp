#include "libOpenBSC.h"
#include "OpenBSC.hpp"
#include "PortManager.hpp"
#include <cstring>
#include <iostream>

static OpenBSC sdk;

extern "C"
{
    /**
     * @brief Lists available COM ports filtered by VID and PID.
     * @param VID USB Vendor ID (default: 0)
     * @param PID USB Product ID (default: 0)
     * @return ComPortList_s containing serial numbers and names of available ports.
     */
    BSC_SDK_EXPORT ComPortList_s listPortSDK(uint16_t VID = 0, uint16_t PID = 0)
    {
        ComPortList_s list = {0};

        auto           ports        = FindPorts(VID, PID);
        const uint32_t COM_MAX_SIZE = sizeof(list.ComPort) / sizeof(list.ComPort[0]);
        uint32_t       count        = std::min(static_cast<uint32_t>(ports.size()), COM_MAX_SIZE);

        for (uint32_t i = 0; i < count; ++i)
        {
            auto       &portInfo = list.ComPort[i];
            const char *portStr  = ports[i].c_str();

            std::snprintf(portInfo.serial, sizeof(portInfo.serial), "%s", portStr);

            const char *baseName = std::strrchr(portStr, '/');
            if (baseName)
            {
                std::snprintf(portInfo.name, sizeof(portInfo.name), "%s", baseName + 1);
            }
            else
            {
                std::snprintf(portInfo.name, sizeof(portInfo.name), "%s", portStr);
            }
        }

        return list;
    }

    /**
     * @brief Initializes the OpenBSC SDK with specified serial port parameters.
     * @param comSerial COM port to open
     * @param baudRate Baud rate for serial communication
     * @param byte_size Number of data bits
     * @param stop_bits Number of stop bits
     * @param parity Parity ('N', 'E', 'O')
     * @param use_rts Enable RTS
     * @param use_dtr Enable DTR
     * @return errorList_e indicating success or type of failure
     */
    BSC_SDK_EXPORT enum errorList_e OpenBSCSDKInit(const char *comSerial, uint32_t baudRate, uint8_t byte_size, uint8_t stop_bits, char parity,
                                                   bool use_rts, bool use_dtr)
    {
        if (!comSerial || comSerial[0] == '\0')
        {
            return PORT_NOT_FOUND;
        }

        if (!sdk.Init(comSerial, baudRate, byte_size, stop_bits, parity, use_rts, use_dtr))
        {
            return CONFIG_FAILED;
        }

        return NONE;
    }

    /**
     * @brief Opens the specified COM port using the SDK.
     * @param comSerial COM port to open
     * @return errorList_e indicating success or type of failure
     */
    BSC_SDK_EXPORT enum errorList_e OpenBSCSDKOpen(const char *comSerial)
    {
        if (!comSerial || comSerial[0] == '\0')
        {
            return PORT_NOT_FOUND;
        }

        ComPortList_s comList  = listPortSDK();
        bool          findPort = false;

        for (uint32_t i = 0; i < sizeof(comList.ComPort) / sizeof(comList.ComPort[0]); i++)
        {
            if (std::strcmp(comList.ComPort[i].serial, comSerial) == 0)
            {
                findPort = true;
                break;
            }
        }

        if (!findPort)
        {
            return PORT_NOT_FOUND;
        }

        if (!sdk.Open(comSerial))
        {
            return PORT_OPEN_FAILED;
        }

        return NONE;
    }

    /**
     * @brief Closes the SDK connection and disconnects from the device.
     */
    BSC_SDK_EXPORT void OpenBSCSDKClose(void)
    {
        sdk.Disconnect();
    }

    /**
     * @brief Sends a command to the connected device and reads the response.
     * @param cmd Command string to send
     * @return CommandOutcome_s containing the response and error code
     */
    BSC_SDK_EXPORT struct CommandOutcome_s OpenBSCSDKSend(const char *cmd)
    {
        CommandOutcome_s resp{};
        uint32_t         length = std::strlen(cmd);

        if (!sdk.SendCommand(cmd, length))
        {
            return CommandOutcome_s{.error = SEND_FAILED};
        }
        else
        {
            uint32_t received     = sdk.ReadResponse(resp.answer, sizeof(resp.answer) - 1, true);
            resp.answer[received] = '\0';
        }

        return resp;
    }
}
