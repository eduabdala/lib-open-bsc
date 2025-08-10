# Toolchain for building on Linux
set(CMAKE_SYSTEM_NAME Linux)

# Specify the compilers
set(CMAKE_C_COMPILER /usr/bin/gcc)
set(CMAKE_CXX_COMPILER /usr/bin/g++)

# Debugging and warning flags (opcional)
set(CMAKE_C_FLAGS "-g -Wall -Wextra")
set(CMAKE_CXX_FLAGS "-g -Wall -Wextra")

# Additional linker flags (opcional)
set(CMAKE_EXE_LINKER_FLAGS "")
