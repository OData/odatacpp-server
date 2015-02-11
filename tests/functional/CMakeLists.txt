include_directories(
  ${UnitTestpp_INCLUDE_DIR}
  ${Utilities_INCLUDE_DIR}
  )

# Target
set(ODATACPP_TESTS_FUNCTIONAL odata-tests-functional)

set(ODATACPP_TESTS_FUNCTIONAL_SOURCES
  odata_tests.cpp
  common_test/common_utility_test.cpp
  common_test/common_nullable_test.cpp
  core_test/odata_collection_value_test.cpp
  core_test/odata_json_writer_test.cpp
  core_test/odata_context_url_parser_test.cpp
  core_test/odata_value_test.cpp
  core_test/odata_json_reader_test.cpp
  core_test/odata_uri_parser_test.cpp
  edm_test/edm_model_reader_test.cpp
  edm_test/edm_model_utility_test.cpp
  )

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w")

add_library(${ODATACPP_TESTS_FUNCTIONAL} ${ODATACPP_TESTS_FUNCTIONAL_SOURCES})

target_link_libraries(${ODATACPP_TESTS_FUNCTIONAL}
  ${ODATACPP_LIBRARY}
  unittestpp
  utilities
  )
