/**
 * @file PortManager.cpp
 * @author Mairon Schneider Cardoso (mairon.cardoso_irede@perto.com.br)
 * @brief Source file for Port Manager (Windows and Linux)
 * @version 0.2
 * @date 2025-08-09
 * @copyright Copyright (c) 2025
 */

#include <vector>
#include <string>

#ifdef _WIN32
  #include <windows.h>
  #include <setupapi.h>
  #include <iostream>
  #include <cstring>

  /**
   * @brief Extracts the port name from a friendly name string.
   * @param[in] friendlyName A C-string containing the friendly name, which should include the port name within parentheses.
   * @return The extracted port name.
   *         Returns an empty string if the expected format (i.e., text within parentheses) is not found.
   */
  static std::string parsePortName(const char *friendlyName)
  {
      std::string s(friendlyName);
      auto openParen = s.find('(');
      if (openParen == std::string::npos)
          return "";
      auto closeParen = s.find(')', openParen + 1);
      if (closeParen == std::string::npos)
          return "";

      return s.substr(openParen + 1, closeParen - openParen - 1);
  }

  /**
   * @brief Finds available serial ports on Windows filtered by optional VID and PID.
   * 
   * @param vid USB Vendor ID to filter (0 to disable filter).
   * @param pid USB Product ID to filter (0 to disable filter).
   * @return std::vector<std::string> List of serial port names (e.g., "COM3").
   */
  std::vector<std::string> FindPorts(uint16_t vid, uint16_t pid)
  {
      bool applyFilter = (vid != 0 && pid != 0);
      HDEVINFO deviceInfoSet = INVALID_HANDLE_VALUE;
      std::vector<std::string> ports;

      deviceInfoSet = SetupDiGetClassDevsA(nullptr, "USB", nullptr, DIGCF_ALLCLASSES | DIGCF_PRESENT);
      if (deviceInfoSet == INVALID_HANDLE_VALUE)
          return ports;

      std::string filter;
      if (applyFilter)
      {
          char buff[32];
          sprintf(buff, "VID_%04X&PID_%04X", vid, pid);
          filter = buff;
      }

      SP_DEVINFO_DATA deviceInfoData;
      deviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

      for (DWORD i = 0; SetupDiEnumDeviceInfo(deviceInfoSet, i, &deviceInfoData); i++)
      {
          char instanceId[256];
          if (!SetupDiGetDeviceInstanceIdA(deviceInfoSet, &deviceInfoData, instanceId, sizeof(instanceId), nullptr))
              continue;

          if (!applyFilter || strstr(instanceId, filter.c_str()) != nullptr)
          {
              char friendlyName[256];
              DWORD dataType;
              if (SetupDiGetDeviceRegistryPropertyA(deviceInfoSet, &deviceInfoData, SPDRP_FRIENDLYNAME, &dataType,
                                                    reinterpret_cast<PBYTE>(friendlyName), sizeof(friendlyName), nullptr))
              {
                  std::string port = parsePortName(friendlyName);
                  if (!port.empty())
                      ports.push_back(port);
              }
          }
      }

      SetupDiDestroyDeviceInfoList(deviceInfoSet);
      return ports;
  }

#elif __linux__
  #include <dirent.h>
  #include <regex>
  #include <iostream>

  /**
   * @brief Finds available serial ports on Linux filtered by optional VID and PID (not implemented).
   * 
   * Since USB VID/PID filtering requires parsing sysfs entries, this function currently
   * ignores VID/PID and lists typical serial devices.
   * 
   * @param vid USB Vendor ID to filter (currently unused).
   * @param pid USB Product ID to filter (currently unused).
   * @return std::vector<std::string> List of serial port names (e.g., "/dev/ttyUSB0").
   */
  std::vector<std::string> FindPorts(int vid, int pid)
  {
      (void)vid; // unused
      (void)pid; // unused

      std::vector<std::string> ports;
      const char* devPath = "/dev";
      DIR* dir = opendir(devPath);
      if (!dir)
      {
          std::cerr << "Failed to open /dev directory\n";
          return ports;
      }

      std::regex serialRegex("(ttyUSB[0-9]+|ttyACM[0-9]+|ttyS[0-9]+)");
      struct dirent* entry;

      while ((entry = readdir(dir)) != nullptr)
      {
          std::string name(entry->d_name);
          if (std::regex_match(name, serialRegex))
          {
              ports.push_back(std::string(devPath) + "/" + name);
          }
      }
      closedir(dir);
      return ports;
  }

#else
  #error "Unsupported platform"
#endif
