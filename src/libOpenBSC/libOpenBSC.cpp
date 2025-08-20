#include "libOpenBSC.h"
#include "OpenBSC.hpp"
#include "PortManager.hpp"
#include <cstring>
#include <iostream>

static OpenBSC sdk;

extern "C"
{

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

    BSC_SDK_EXPORT void OpenBSCSDKClose(void)
    {
        sdk.Disconnect();
    }

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
