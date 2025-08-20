#include <vector>
#include <string>

#ifdef _WIN32
#include <windows.h>
#include <setupapi.h>
#include <initguid.h>
#include <devguid.h>
#include <regstr.h>
#pragma comment(lib, "setupapi.lib")

/**
 * @brief Lists available serial ports on Windows using SetupAPI
 * @param vid Vendor ID (optional filter, 0 = ignore)
 * @param pid Product ID (optional filter, 0 = ignore)
 * @return Vector of port names (e.g., "COM1", "COM3")
 */
std::vector<std::string> FindPorts(unsigned short vid, unsigned short pid)
{
    std::vector<std::string> ports;

    HDEVINFO hDevInfo = SetupDiGetClassDevs(&GUID_DEVCLASS_PORTS, nullptr, nullptr, DIGCF_PRESENT);
    if (hDevInfo == INVALID_HANDLE_VALUE)
        return ports;

    SP_DEVINFO_DATA DeviceInfoData = { sizeof(SP_DEVINFO_DATA) };

    for (DWORD i = 0; SetupDiEnumDeviceInfo(hDevInfo, i, &DeviceInfoData); ++i)
    {
        TCHAR portName[256];
        HKEY hKey = SetupDiOpenDevRegKey(hDevInfo, &DeviceInfoData, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_READ);
        if (hKey == INVALID_HANDLE_VALUE)
            continue;

        DWORD size = sizeof(portName);
        DWORD type = 0;
        if (RegQueryValueEx(hKey, TEXT("PortName"), nullptr, &type, (LPBYTE)portName, &size) == ERROR_SUCCESS)
        {
            if (type == REG_SZ)
            {
                // Aqui poderia aplicar filtro por VID/PID (não implementado ainda)
                ports.emplace_back(portName);
            }
        }
        RegCloseKey(hKey);
    }

    SetupDiDestroyDeviceInfoList(hDevInfo);
    return ports;
}

#else

#include <dirent.h>
#include <regex>

/**
 * @brief Lists available serial ports on Linux by scanning /dev
 * @param vid Vendor ID (optional filter, 0 = ignore)
 * @param pid Product ID (optional filter, 0 = ignore)
 * @return Vector of port names (e.g., "/dev/ttyS0", "/dev/ttyUSB0")
 */
std::vector<std::string> FindPorts(unsigned short vid, unsigned short pid)
{
    std::vector<std::string> ports;

    const char* devDir = "/dev/";
    DIR* dir = opendir(devDir);
    if (!dir)
        return ports;

    std::regex serialRegex("(ttyS\\d+|ttyUSB\\d+|ttyACM\\d+)");

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr)
    {
        std::string name(entry->d_name);
        if (std::regex_match(name, serialRegex))
        {
            // Filtro por VID/PID ainda não implementado (para isso precisaria
            // parsear /sys/class/tty/... e verificar os atributos USB).
            ports.emplace_back(std::string(devDir) + name);
        }
    }

    closedir(dir);
    return ports;
}

#endif
