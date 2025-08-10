# Toolchain for cross-compiling to Windows
set(CMAKE_SYSTEM_NAME Windows)

# Try to find C compiler
find_program(MINGW_C_COMPILER NAMES x86_64-w64-mingw32-gcc gcc)
if(NOT MINGW_C_COMPILER)
    message(FATAL_ERROR "No suitable C compiler found (x86_64-w64-mingw32-gcc or gcc not found in PATH)")
endif()
set(CMAKE_C_COMPILER ${MINGW_C_COMPILER})

# Try to find C++ compiler
find_program(MINGW_CXX_COMPILER NAMES x86_64-w64-mingw32-g++ g++)
if(NOT MINGW_CXX_COMPILER)
    message(FATAL_ERROR "No suitable C++ compiler found (x86_64-w64-mingw32-g++ or g++ not found in PATH)")
endif()
set(CMAKE_CXX_COMPILER ${MINGW_CXX_COMPILER})

# Try to find resource compiler
find_program(MINGW_RC_COMPILER NAMES x86_64-w64-mingw32-windres windres)
if(MINGW_RC_COMPILER)
    set(CMAKE_RC_COMPILER ${MINGW_RC_COMPILER})
endif()

# Static linking for runtime libraries
set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")

# Specify the executable format for Windows
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
