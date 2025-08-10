#include "Serial.hpp"

#ifdef _WIN32

#include <windows.h>
#include <stdexcept>

/**
 * @copydoc SerialCommunication::Create
 */
std::shared_ptr<SerialCommunication> SerialCommunication::Create(const std::string &portName, uint32_t baudRate, uint8_t dataBits, uint8_t stopBits,
                                                                 char parity, bool enableRts, bool enableDtr)
{
    try
    {
        return std::shared_ptr<SerialCommunication>(new SerialCommunication(portName, baudRate, dataBits, stopBits, parity, enableRts, enableDtr));
    }
    catch (const std::runtime_error &)
    {
        return nullptr;
    }
}

/**
 * @copydoc SerialCommunication::SerialCommunication
 */
SerialCommunication::SerialCommunication(const std::string &portName, uint32_t baudRate, uint8_t dataBits, uint8_t stopBits, char parity,
                                         bool enableRts, bool enableDtr)
    : handle_(INVALID_HANDLE_VALUE), portName_(portName), baudRate_(baudRate), dataBits_(dataBits), stopBits_(stopBits), parity_(parity),
      enableRts_(enableRts), enableDtr_(enableDtr)
{
}

/**
 * @brief Destructor closes the port if still open.
 */
SerialCommunication::~SerialCommunication()
{
    Close();
}

/**
 * @copydoc SerialCommunication::open
 */
bool SerialCommunication::Open()
{
    std::string fullPort = "\\\\.\\" + portName_;
    handle_              = CreateFileA(fullPort.c_str(), GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, 0, nullptr);

    if (handle_ == INVALID_HANDLE_VALUE)
    {
        return false;
    }

    try
    {
        applySettings();
    }
    catch (const std::runtime_error &)
    {
        CloseHandle(handle_);
        handle_ = INVALID_HANDLE_VALUE;
        return false;
    }

    return true;
}

/**
 * @copydoc SerialCommunication::Close
 */
void SerialCommunication::Close()
{
    if (handle_ != INVALID_HANDLE_VALUE)
    {
        PurgeComm(handle_, PURGE_RXCLEAR | PURGE_TXCLEAR);
        CloseHandle(handle_);
        handle_ = INVALID_HANDLE_VALUE;
    }
}

/**
 * @copydoc SerialCommunication::Write
 */
std::size_t SerialCommunication::Write(const void *buffer, std::size_t size)
{
    if (handle_ == INVALID_HANDLE_VALUE)
    {
        throw std::runtime_error("Port not open");
    }

    DWORD bytesWritten = 0;
    if (!WriteFile(handle_, buffer, static_cast<DWORD>(size), &bytesWritten, nullptr))
    {
        throw std::runtime_error("WriteFile failed");
    }
    return static_cast<std::size_t>(bytesWritten);
}

/**
 * @copydoc SerialCommunication::Read
 */
std::size_t SerialCommunication::Read(void *buffer, std::size_t size, uint32_t timeoutMs)
{
    if (handle_ == INVALID_HANDLE_VALUE)
    {
        throw std::runtime_error("Port not open");
    }

    COMMTIMEOUTS to = {};
    if (timeoutMs > 0)
    {
        to.ReadIntervalTimeout        = timeoutMs;
        to.ReadTotalTimeoutConstant   = timeoutMs;
        to.ReadTotalTimeoutMultiplier = 0;
    }
    SetCommTimeouts(handle_, &to);

    DWORD bytesRead = 0;
    if (!ReadFile(handle_, buffer, static_cast<DWORD>(size), &bytesRead, nullptr))
    {
        throw std::runtime_error("ReadFile failed");
    }
    return static_cast<std::size_t>(bytesRead);
}

/**
 * @copydoc SerialCommunication::Flush
 */
void SerialCommunication::Flush()
{
    if (handle_ == INVALID_HANDLE_VALUE)
    {
        throw std::runtime_error("Port not open");
    }

    if (!PurgeComm(handle_, PURGE_RXCLEAR | PURGE_TXCLEAR))
    {
        throw std::runtime_error("PurgeComm failed");
    }
}

/**
 * @copydoc SerialCommunication::applySettings
 */
void SerialCommunication::applySettings()
{
    DCB dcb       = {};
    dcb.DCBlength = sizeof(DCB);
    if (!GetCommState(handle_, &dcb))
    {
        throw std::runtime_error("GetCommState failed");
    }

    dcb.BaudRate     = baudRate_;
    dcb.ByteSize     = dataBits_;
    dcb.StopBits     = (stopBits_ == 2 ? TWOSTOPBITS : ONESTOPBIT);
    dcb.Parity       = (parity_ == 'E' ? EVENPARITY : (parity_ == 'O' ? ODDPARITY : NOPARITY));
    dcb.fRtsControl  = enableRts_ ? RTS_CONTROL_ENABLE : RTS_CONTROL_DISABLE;
    dcb.fDtrControl  = enableDtr_ ? DTR_CONTROL_ENABLE : DTR_CONTROL_DISABLE;
    dcb.fOutxCtsFlow = FALSE;
    dcb.fOutxDsrFlow = FALSE;
    dcb.fInX         = FALSE;
    dcb.fOutX        = FALSE;

    if (!SetCommState(handle_, &dcb))
    {
        throw std::runtime_error("SetCommState failed");
    }

    COMMTIMEOUTS timeouts = {};
    if (!SetCommTimeouts(handle_, &timeouts))
    {
        throw std::runtime_error("SetCommTimeouts failed");
    }
}

#endif // _WIN32
