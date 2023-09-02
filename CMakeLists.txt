cmake_minimum_required(VERSION 3.12)

project("sfo_cpp"
        VERSION 0.1
        DESCRIPTION "Header-only C++ library for submodular function maximization"
        HOMEPAGE_URL "https://github.com/buntonj/SFO_CPP"
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

option(INSTALL_SFO_LIBRARY "Enable installing into default location" OFF)
option(BUILD_SFO_TESTS "Build tests." OFF)
option(BUILD_SFO_EXAMPLES "Build examples." OFF)

message("Installing in default location is set to ${INSTALL_SFO_LIBRARY}")
message("Test building is set to: ${BUILD_SFO_TESTS}")
message("Example building is set to: ${BUILD_SFO_EXAMPLES}")


include(GNUInstallDirs)

# Adding the base construct libraries  
add_library(sfo_cpp INTERFACE)

# setting the interface for library
target_include_directories(
  sfo_cpp
  INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
            $<INSTALL_INTERFACE:include>)

# requires C++17 to compile
target_compile_features(sfo_cpp INTERFACE cxx_std_17)

if(BUILD_SFO_TESTS)
    add_subdirectory(tests)
endif()

if(BUILD_SFO_EXAMPLES)
    add_subdirectory(examples)
endif()

if(SFO_INSTALL_LIBRARY)
  # locations are provided by GNUInstallDirs
  install(
    TARGETS sfo_cpp
    EXPORT SFO_CPP_TARGETS
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

  include(CMakePackageConfigHelpers)
  write_basic_package_version_file(
    "SFO_CPP_ConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion)

  configure_package_config_file(
    "${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in"
    "${PROJECT_BINARY_DIR}/${PROJECT_NAME}.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake)

  install(
    EXPORT ${PROJECT_NAME}_Targets
    FILE ${PRPJECT_NAME}Targets.cmake
    NAMESPACE ${PROJECT_NAME}::
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake)

  install(FILES "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
                "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
          DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake)

  install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/${PROJECT_NAME}
          DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
endif()