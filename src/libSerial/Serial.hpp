#ifndef SERIAL_HPP
#define SERIAL_HPP

#include <string>
#include <vector>
#include <memory>
#include <cstdint>

/**
 * @brief Cross-platform serial communication class supporting Windows and Linux
 */
class SerialCommunication
{
public:
    /**
     * @brief Factory method to create and configure a SerialCommunication instance
     * @param portName Port name string (e.g., "COM3" or "/dev/ttyUSB0")
     * @param baudRate Baud rate (e.g., 9600, 115200)
     * @param dataBits Number of data bits (5,6,7,8)
     * @param stopBits Number of stop bits (1 or 2)
     * @param parity Parity character: 'N' (none), 'E' (even), 'O' (odd)
     * @param enableRts Enable RTS line (true/false)
     * @param enableDtr Enable DTR line (true/false)
     * @return shared_ptr to SerialCommunication instance or nullptr on failure
     */
    static std::shared_ptr<SerialCommunication> Create(const std::string& portName, uint32_t baudRate, 
                                                      uint8_t dataBits, uint8_t stopBits, char parity,
                                                      bool enableRts, bool enableDtr);

    virtual ~SerialCommunication();

    /**
     * @brief Open the serial port
     * @return true on success, false otherwise
     */
    virtual bool Open();

    /**
     * @brief Close the serial port
     */
    virtual void Close();

    /**
     * @brief Write data to the serial port
     * @param buffer Pointer to data buffer
     * @param length Number of bytes to write
     * @throws std::runtime_error on failure
     */
    virtual size_t Write(const void* buffer, size_t length);

    /**
     * @brief Read data from the serial port with timeout
     * @param buffer Pointer to buffer to fill
     * @param length Maximum bytes to read
     * @param timeoutMs Timeout in milliseconds
     * @return Number of bytes actually read
     * @throws std::runtime_error on failure or timeout
     */
    virtual size_t Read(void* buffer, size_t length, unsigned int timeoutMs);

    /**
     * @brief Flush input and output buffers
     * @throws std::runtime_error on failure
     */
    virtual void Flush();

protected:
    SerialCommunication(const std::string& portName, uint32_t baudRate, uint8_t dataBits,
                        uint8_t stopBits, char parity, bool enableRts, bool enableDtr);

    // Internal initialization/configuration function
    virtual bool configurePort();

    // Port parameters
    std::string portName_;
    uint32_t baudRate_;
    uint8_t dataBits_;
    uint8_t stopBits_;
    char parity_;
    bool enableRts_;
    bool enableDtr_;

#ifdef _WIN32
    void* handle_; // HANDLE on Windows
#else
    int fd_;       // File descriptor on Linux
#endif

    bool isOpen_;
};

#endif // SERIAL_HPP
