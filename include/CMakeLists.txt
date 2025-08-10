cmake_minimum_required(VERSION 3.16)
project(IncludeHeaders)

add_library(headers INTERFACE)

target_include_directories(headers
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/libOpenBSC>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/libSerial>
)

install(TARGETS headers EXPORT headersConfig)
install(DIRECTORY libOpenBSC libSerial
        DESTINATION include)
