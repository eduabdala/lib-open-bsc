add_executable(MediumTerminal
  main.cpp
  MediumTerminal.cpp
  SdkWrapper.cpp
)

target_link_libraries(MediumTerminal PRIVATE OpenBSC)

target_include_directories(MediumTerminal PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include/libOpenBSC
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_features(MediumTerminal PRIVATE cxx_std_17)

set_target_properties(MediumTerminal PROPERTIES
    BUILD_WITH_INSTALL_RPATH TRUE
    INSTALL_RPATH "$ORIGIN/../lib"
)

set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

install(TARGETS MediumTerminal
    RUNTIME DESTINATION bin
)
