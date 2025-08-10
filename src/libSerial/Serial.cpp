#include "Serial.hpp"
#include <stdexcept>
#include <cstring>

#ifdef _WIN32
#include <windows.h>
#else
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <poll.h>
#endif

// Factory method
std::shared_ptr<SerialCommunication> SerialCommunication::Create(const std::string& portName, uint32_t baudRate, 
                                                                 uint8_t dataBits, uint8_t stopBits, char parity,
                                                                 bool enableRts, bool enableDtr)
{
    auto instance = std::shared_ptr<SerialCommunication>(
        new SerialCommunication(portName, baudRate, dataBits, stopBits, parity, enableRts, enableDtr)
    );

    if (!instance->Open())
        return nullptr;

    return instance;
}

SerialCommunication::SerialCommunication(const std::string& portName, uint32_t baudRate, uint8_t dataBits,
                                         uint8_t stopBits, char parity, bool enableRts, bool enableDtr)
    : portName_(portName), baudRate_(baudRate), dataBits_(dataBits), stopBits_(stopBits),
      parity_(parity), enableRts_(enableRts), enableDtr_(enableDtr), isOpen_(false)
{
#ifdef _WIN32
    handle_ = INVALID_HANDLE_VALUE;
#else
    fd_ = -1;
#endif
}

SerialCommunication::~SerialCommunication()
{
    Close();
}

bool SerialCommunication::Open()
{
#ifdef _WIN32
    handle_ = CreateFileA(portName_.c_str(),
                         GENERIC_READ | GENERIC_WRITE,
                         0,
                         nullptr,
                         OPEN_EXISTING,
                         0,
                         nullptr);

    if (handle_ == INVALID_HANDLE_VALUE)
        return false;

    if (!configurePort())
    {
        Close();
        return false;
    }

#else
    fd_ = ::open(portName_.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (fd_ < 0)
        return false;

    if (!configurePort())
    {
        Close();
        return false;
    }
#endif

    isOpen_ = true;
    return true;
}

void SerialCommunication::Close()
{
    if (!isOpen_)
        return;

#ifdef _WIN32
    CloseHandle(handle_);
    handle_ = INVALID_HANDLE_VALUE;
#else
    ::close(fd_);
    fd_ = -1;
#endif

    isOpen_ = false;
}

size_t SerialCommunication::Write(const void* buffer, size_t length)
{
    if (!isOpen_)
        throw std::runtime_error("Port not open");

#ifdef _WIN32
    DWORD written;
    if (!WriteFile(handle_, buffer, static_cast<DWORD>(length), &written, nullptr))
        throw std::runtime_error("Write failed");
    if (written != length)
        throw std::runtime_error("Incomplete write");

#else
    ssize_t written = ::write(fd_, buffer, length);
    if (written < 0)
        throw std::runtime_error("Write failed");
    if (static_cast<size_t>(written) != length)
        throw std::runtime_error("Incomplete write");
#endif
}

size_t SerialCommunication::Read(void* buffer, size_t length, unsigned int timeoutMs)
{
    if (!isOpen_)
        throw std::runtime_error("Port not open");

#ifdef _WIN32
    // Setup timeout via COMMTIMEOUTS
    COMMTIMEOUTS timeouts = {0};
    timeouts.ReadIntervalTimeout = timeoutMs;
    timeouts.ReadTotalTimeoutConstant = timeoutMs;
    timeouts.ReadTotalTimeoutMultiplier = 0;

    if (!SetCommTimeouts(handle_, &timeouts))
        throw std::runtime_error("Failed to set timeouts");

    DWORD readBytes = 0;
    if (!ReadFile(handle_, buffer, static_cast<DWORD>(length), &readBytes, nullptr))
        throw std::runtime_error("Read failed");

    return static_cast<size_t>(readBytes);

#else
    struct pollfd pfd;
    pfd.fd = fd_;
    pfd.events = POLLIN;

    int ret = poll(&pfd, 1, timeoutMs);
    if (ret < 0)
        throw std::runtime_error("Poll error");
    else if (ret == 0)
        return 0; // timeout

    ssize_t n = ::read(fd_, buffer, length);
    if (n < 0)
        throw std::runtime_error("Read error");

    return static_cast<size_t>(n);
#endif
}

void SerialCommunication::Flush()
{
    if (!isOpen_)
        throw std::runtime_error("Port not open");

#ifdef _WIN32
    if (!PurgeComm(handle_, PURGE_RXCLEAR | PURGE_TXCLEAR))
        throw std::runtime_error("Flush failed");
#else
    if (tcflush(fd_, TCIOFLUSH) != 0)
        throw std::runtime_error("Flush failed");
#endif
}

bool SerialCommunication::configurePort()
{
#ifdef _WIN32
    DCB dcb = {0};
    dcb.DCBlength = sizeof(dcb);
    if (!GetCommState(handle_, &dcb))
        return false;

    dcb.BaudRate = baudRate_;

    switch (dataBits_)
    {
        case 5: dcb.ByteSize = 5; break;
        case 6: dcb.ByteSize = 6; break;
        case 7: dcb.ByteSize = 7; break;
        case 8: dcb.ByteSize = 8; break;
        default: dcb.ByteSize = 8; break;
    }

    switch (stopBits_)
    {
        case 1: dcb.StopBits = ONESTOPBIT; break;
        case 2: dcb.StopBits = TWOSTOPBITS; break;
        default: dcb.StopBits = ONESTOPBIT; break;
    }

    switch (parity_)
    {
        case 'N':
        case 'n': dcb.Parity = NOPARITY; break;
        case 'E':
        case 'e': dcb.Parity = EVENPARITY; break;
        case 'O':
        case 'o': dcb.Parity = ODDPARITY; break;
        default: dcb.Parity = NOPARITY; break;
    }

    dcb.fRtsControl = enableRts_ ? RTS_CONTROL_ENABLE : RTS_CONTROL_DISABLE;
    dcb.fDtrControl = enableDtr_ ? DTR_CONTROL_ENABLE : DTR_CONTROL_DISABLE;

    dcb.fOutxCtsFlow = FALSE;
    dcb.fOutxDsrFlow = FALSE;
    dcb.fOutX = FALSE;
    dcb.fInX = FALSE;

    if (!SetCommState(handle_, &dcb))
        return false;

    // Setup timeouts
    COMMTIMEOUTS timeouts = {0};
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;

    if (!SetCommTimeouts(handle_, &timeouts))
        return false;

    return true;

#else
    struct termios tty;
    if (tcgetattr(fd_, &tty) != 0)
        return false;

    speed_t speed;
    switch (baudRate_)
    {
        case 115200: speed = B115200; break;
        case 57600: speed = B57600; break;
        case 38400: speed = B38400; break;
        case 19200: speed = B19200; break;
        case 9600: speed = B9600; break;
        case 4800: speed = B4800; break;
        case 2400: speed = B2400; break;
        default: speed = B9600; break;
    }

    cfsetospeed(&tty, speed);
    cfsetispeed(&tty, speed);

    tty.c_cflag &= ~PARENB;
    tty.c_cflag &= ~(CSTOPB);
    tty.c_cflag &= ~CSIZE;

    switch (dataBits_)
    {
        case 5: tty.c_cflag |= CS5; break;
        case 6: tty.c_cflag |= CS6; break;
        case 7: tty.c_cflag |= CS7; break;
        case 8: tty.c_cflag |= CS8; break;
        default: tty.c_cflag |= CS8; break;
    }

    if (stopBits_ == 2)
        tty.c_cflag |= CSTOPB;
    else
        tty.c_cflag &= ~CSTOPB;

    switch (parity_)
    {
        case 'N':
        case 'n':
            tty.c_cflag &= ~PARENB;
            break;
        case 'E':
        case 'e':
            tty.c_cflag |= PARENB;
            tty.c_cflag &= ~PARODD;
            break;
        case 'O':
        case 'o':
            tty.c_cflag |= PARENB;
            tty.c_cflag |= PARODD;
            break;
        default:
            tty.c_cflag &= ~PARENB;
            break;
    }

    // RTS and DTR control (hardware flow)
    if (enableRts_)
        tty.c_cflag |= CRTSCTS;
    else
        tty.c_cflag &= ~CRTSCTS;

    tty.c_cflag |= CLOCAL | CREAD;

    tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);
    tty.c_oflag &= ~OPOST;

    tty.c_cc[VMIN] = 0;
    tty.c_cc[VTIME] = 10; // 1 second timeout

    if (tcsetattr(fd_, TCSANOW, &tty) != 0)
        return false;

    return true;
#endif
}
