cmake_minimum_required(VERSION 3.16)
project(SuperJupiterCalibration VERSION 0.1.0 LANGUAGES CXX)

set(_superjupiter_calibration_top_level OFF)
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  set(_superjupiter_calibration_top_level ON)
endif()
option(SUPERJUPITER_CALIBRATION_BUILD_TESTS "Build Calibration tests" ${_superjupiter_calibration_top_level})
option(SUPERJUPITER_CALIBRATION_BUILD_EXAMPLES "Build Calibration host example" ${_superjupiter_calibration_top_level})
option(SUPERJUPITER_CALIBRATION_INSTALL "Generate Calibration install rules" ${_superjupiter_calibration_top_level})

add_library(SuperJupiterCalibration INTERFACE)
add_library(SuperJupiter::Calibration ALIAS SuperJupiterCalibration)
set_target_properties(SuperJupiterCalibration PROPERTIES EXPORT_NAME Calibration)
target_include_directories(SuperJupiterCalibration INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
  $<INSTALL_INTERFACE:include>)
target_compile_features(SuperJupiterCalibration INTERFACE cxx_std_11)

if(SUPERJUPITER_CALIBRATION_BUILD_TESTS)
  enable_testing()
  add_executable(superjupiter-calibration-tests tests/test.cpp)
  target_link_libraries(superjupiter-calibration-tests PRIVATE SuperJupiter::Calibration)
  add_test(NAME superjupiter-calibration-tests COMMAND superjupiter-calibration-tests)
  add_executable(superjupiter-calibration-namespace-test tests/namespace_isolation.cpp)
  target_link_libraries(superjupiter-calibration-namespace-test PRIVATE SuperJupiter::Calibration)
  add_test(NAME superjupiter-calibration-namespace-test COMMAND superjupiter-calibration-namespace-test)
endif()
if(SUPERJUPITER_CALIBRATION_BUILD_EXAMPLES)
  add_executable(superjupiter-calibration-replay tools/replay.cpp)
  target_link_libraries(superjupiter-calibration-replay PRIVATE SuperJupiter::Calibration)
endif()
if(SUPERJUPITER_CALIBRATION_INSTALL)
  include(CMakePackageConfigHelpers)
  set(_superjupiter_calibration_package_dir lib/cmake/SuperJupiterCalibration)
  install(TARGETS SuperJupiterCalibration EXPORT SuperJupiterCalibrationTargets)
  install(FILES src/SuperJupiterCalibration.h DESTINATION include)
  install(EXPORT SuperJupiterCalibrationTargets NAMESPACE SuperJupiter::
    DESTINATION ${_superjupiter_calibration_package_dir})
  configure_package_config_file(cmake/SuperJupiterCalibrationConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/SuperJupiterCalibrationConfig.cmake
    INSTALL_DESTINATION ${_superjupiter_calibration_package_dir})
  write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/SuperJupiterCalibrationConfigVersion.cmake
    VERSION ${PROJECT_VERSION} COMPATIBILITY SameMinorVersion ARCH_INDEPENDENT)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/SuperJupiterCalibrationConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/SuperJupiterCalibrationConfigVersion.cmake
    DESTINATION ${_superjupiter_calibration_package_dir})
  install(FILES LICENSE DESTINATION share/licenses/SuperJupiterCalibration)
endif()
