add_executable(bscTerm
  main.cpp
  MediumTerminal.cpp
  SdkWrapper.cpp
)

target_link_libraries(bscTerm PRIVATE OpenBSC)

target_include_directories(bscTerm PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include/libOpenBSC
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_features(bscTerm PRIVATE cxx_std_17)

set_target_properties(bscTerm PROPERTIES
    BUILD_WITH_INSTALL_RPATH TRUE
    INSTALL_RPATH "$ORIGIN/../lib"
)

set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

install(TARGETS bscTerm
    RUNTIME DESTINATION bin
)
