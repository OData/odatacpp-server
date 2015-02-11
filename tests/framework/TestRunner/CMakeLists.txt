include_directories(
  ${UnitTestpp_INCLUDE_DIR}
  ${Boost_INCLUDE_DIR}
  )

set(TR_SOURCES
  test_runner.cpp
  test_module_loader.cpp
  )

add_definitions(-DDESKTOP_TEST_RUNNER)

if(BUILD_SHARED_LIBS)
  add_executable(test_runner
    test_runner.cpp
    test_module_loader.cpp
    )

  target_link_libraries(test_runner
    ${Boost_FRAMEWORK}
    ${Boost_SYSTEM_LIBRARY}
    ${Boost_FILESYSTEM_LIBRARY}
    ${LIB}unittestpp
    ${CMAKE_DL_LIBS}
    )
else()
  if (APPLE)
    add_executable(test_runner
      test_runner.cpp
      test_module_loader.cpp
      )

    target_link_libraries(test_runner
      ${Boost_FRAMEWORK}
      ${Boost_SYSTEM_LIBRARY}
      ${Boost_FILESYSTEM_LIBRARY}
      ${LIB}unittestpp
      ${CMAKE_DL_LIBS}
      )
  elseif(UNIX)
    add_executable(test_runner
      test_runner.cpp
      test_module_loader.cpp
      )

    target_link_libraries(test_runner
      ${Boost_FRAMEWORK}
      ${Boost_SYSTEM_LIBRARY}
      ${Boost_FILESYSTEM_LIBRARY}
      ${LIB}unittestpp
      ${CMAKE_DL_LIBS}
      -Wl,--no-whole-archive
      )
  else()
    # In order to achieve --whole-archive on windows, we link all the test files into the test_runner directly
    # This means that the tests themselves must be created as "OBJECT" libraries
    add_executable(test_runner
      test_runner.cpp
      test_module_loader.cpp
      )
    target_link_libraries(test_runner
      ${LIB}unittestpp
      ${CMAKE_DL_LIBS}
      )
  endif()
endif()

# add_custom_target(run_tests ALL DEPENDS run_tests.sh)
# add_custom_command(
#   OUTPUT run_tests.sh
#   COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/run_tests.sh $<TARGET_FILE_DIR:test_runner>
#   DEPENDS run_tests.sh
#   )
