#include "OpenBSC.hpp"
#include <cstring>

const uint16_t MAX_BUFF_SIZE = 1024;

OpenBSC::OpenBSC() = default;

bool OpenBSC::Open(const char* comSerial)
{
    bool status = false;

    do
    {
        if (comSerial == nullptr || comSerial[0] == '\0')
        {
            break;
        }

        if (!serial)
        {
            break;
        }

        status = serial->Open();
    } while (0);

    return status;
}

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

bool OpenBSC::SendCommand(const char* command, uint32_t length)
{
    bool success = false;

    do
    {
        if (!serial || command == nullptr || length == 0)
        {
            break;
        }

        uint8_t packet[MAX_BUFF_SIZE] = {0};
        uint32_t packetSize = 0;

        packet[packetSize++] = 0x02;

        if (length + 3 > sizeof(packet))
        {
            break;
        }

        std::memcpy(&packet[packetSize], command, length);
        packetSize += length;

        packet[packetSize++] = 0x03;

        uint8_t bcc = CalculateBCC(packet, packetSize);
        packet[packetSize++] = bcc;

        std::size_t written = serial->Write(packet, packetSize);
        success = (written == packetSize);
    } while (0);

    return success;
}

uint32_t OpenBSC::ReadResponse(char* buffer, uint32_t length, bool filtered)
{
    uint32_t bytesRead = 0;

    do
    {
        if (!serial || buffer == nullptr || length == 0)
        {
            break;
        }

        std::size_t read = serial->Read(buffer, length, 1000);
        if (read == 0)
        {
            break;
        }

        bytesRead = static_cast<uint32_t>(read);

        if (filtered)
        {
            if (bytesRead < 4)
            {
                bytesRead = 0;
                break;
            }

            const uint32_t offset = 2;
            uint32_t cmdLength = bytesRead - 4;
            std::memmove(buffer, &buffer[offset], cmdLength);
            bytesRead = cmdLength;
        }
    } while (0);

    return bytesRead;
}

bool OpenBSC::Disconnect()
{
    if (serial)
    {
        serial->Close();
        serial.reset();
        return true;
    }
    return false;
}

uint8_t OpenBSC::CalculateBCC(const uint8_t* data, uint32_t length)
{
    uint8_t bcc = 0;

    if (data != nullptr && length > 0)
    {
        for (uint32_t i = 0; i < length; ++i)
        {
            bcc ^= data[i];
        }
    }

    return bcc;
}
