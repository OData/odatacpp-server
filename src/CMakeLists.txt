set(ODATACPP_LIBRARY_SOURCES
  common/asyncrt_utils.cpp
  common/base64.cpp
  common/json.cpp
  common/json_parsing.cpp
  common/json_serialization.cpp
  common/utility.cpp
  common/uri.cpp
  common/uri_builder.cpp
  common/uri_parser.cpp
  common/xmlhelpers.cpp
  core/odata_context_url_parser.cpp
  core/odata_entity_model_builder.cpp
  core/odata_entity_value.cpp
  core/odata_json_operation_payload_parameter_writer.cpp
  core/odata_json_operation_url_parameter_writer.cpp
  core/odata_json_reader_full.cpp
  core/odata_json_reader_minimal.cpp
  core/odata_json_writer.cpp
  core/odata_primitive_value.cpp
  core/odata_property_map.cpp
  core/odata_structured_value.cpp
  core/odata_path.cpp
  core/odata_path_segment.cpp
  core/odata_query_node.cpp
  core/odata_path_segment_visitor.cpp
  core/odata_query_node_visitor.cpp
  core/odata_uri.cpp
  core/odata_uri_parser.cpp
  edm/edm_entity_container.cpp
  edm/edm_model_reader.cpp
  edm/edm_schema.cpp
  edm/edm_model.cpp
  edm/edm_model_utility.cpp
  edm/edm_type.cpp
  )

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${WARNINGS} -Werror -pedantic")

add_library(${ODATACPP_LIBRARY} ${ODATACPP_LIBRARY_SOURCES})

target_link_libraries(${ODATACPP_LIBRARY}
  ${LIBXML2_LIBRARIES}
  ${Boost_FRAMEWORK}
  ${Boost_LOCALE_LIBRARY}
  ${Boost_SYSTEM_LIBRARY}
  ${EXTRALINKS}
  )

# Portions specific to odatacpp binary versioning.
set (ODATACPP_VERSION_MAJOR 1)
set (ODATACPP_VERSION_MINOR 0)
set (ODATACPP_VERSION_REVISION 0)
set_target_properties(${ODATACPP_LIBRARY} PROPERTIES
  SOVERSION ${ODATACPP_VERSION_MAJOR}.${ODATACPP_VERSION_MINOR})
