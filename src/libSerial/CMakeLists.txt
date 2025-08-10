add_library(Serial SHARED
    libSerial.cpp
    PortManager.cpp
    Serial.cpp
)

target_include_directories(Serial PUBLIC
    ${CMAKE_SOURCE_DIR}/include/libSerial
)

target_compile_definitions(Serial PRIVATE SERIALCOMM_EXPORTS)

target_compile_features(Serial PUBLIC cxx_std_17)

if(WIN32)
    target_link_libraries(Serial PRIVATE setupapi)
endif()

install(TARGETS Serial
    RUNTIME   DESTINATION bin
    LIBRARY   DESTINATION lib
    ARCHIVE   DESTINATION lib
)
