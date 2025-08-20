add_library(OpenBSC SHARED
    OpenBSC.cpp
    libOpenBSC.cpp
)

target_include_directories(OpenBSC PUBLIC
  ${CMAKE_SOURCE_DIR}/src/libSerial
  ${CMAKE_SOURCE_DIR}/include/libOpenBSC
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(OpenBSC PRIVATE Serial)

target_compile_features(OpenBSC PUBLIC cxx_std_17)

target_compile_definitions(OpenBSC PRIVATE EVENT_LOGGER_EXPORTS)

install(TARGETS OpenBSC
    RUNTIME   DESTINATION bin
    LIBRARY   DESTINATION lib
    ARCHIVE   DESTINATION lib
)
