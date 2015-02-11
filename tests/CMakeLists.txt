set(ODATACPP_TEST_FRAMEWORK_DIR ${ODATACPP_TEST_DIR}/framework)
set(UnitTestpp_INCLUDE_DIR ${ODATACPP_TEST_FRAMEWORK_DIR}/UnitTestpp)
set(Utilities_INCLUDE_DIR ${ODATACPP_TEST_FRAMEWORK_DIR}/utilities/include)

add_subdirectory(framework)
add_subdirectory(functional)