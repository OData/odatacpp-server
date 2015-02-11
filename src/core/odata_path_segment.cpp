//---------------------------------------------------------------------
// <copyright file="odata_path_segment.cpp" company="Microsoft">
//      Copyright (C) Microsoft Corporation. All rights reserved. See License.txt in the project root for license information.
// </copyright>
//---------------------------------------------------------------------

#include "odata/core/odata_path_segment.h"

namespace odata { namespace core
{

std::shared_ptr<odata_path_segment> odata_path_segment::create_metadata_segment()
{
	static auto instance = std::make_shared<odata_metadata_segment>();

	return instance;
}

std::shared_ptr<odata_path_segment> odata_path_segment::create_batch_segment()
{
	static auto instance = std::make_shared<odata_batch_segment>();

	return instance;
}

std::shared_ptr<odata_path_segment> odata_path_segment::create_entity_set_segment(
    std::shared_ptr<::odata::edm::edm_entity_set> entity_set)
{
	return std::make_shared<odata_entity_set_segment>(entity_set);
}

std::shared_ptr<odata_path_segment> odata_path_segment::create_singleton_segment(
    std::shared_ptr<::odata::edm::edm_singleton> singleton)
{
	return std::make_shared<odata_singleton_segment>(singleton);
}

std::shared_ptr<odata_path_segment> odata_path_segment::create_key_segment(
    std::shared_ptr<::odata::edm::edm_navigation_source> navigation_source,
	std::shared_ptr<::odata::edm::edm_entity_type> target_entity_type,
    std::vector<std::pair<::odata::utility::string_t, std::shared_ptr<::odata::core::odata_primitive_value>>> keys)
{
	return std::make_shared<odata_key_segment>(navigation_source, target_entity_type, std::move(keys));
}

std::shared_ptr<odata_path_segment> odata_path_segment::create_structural_property_segment(
    std::shared_ptr<::odata::edm::edm_structured_type> owning_type,
    std::shared_ptr<::odata::edm::edm_property_type> property)
{
    return std::make_shared<odata_structural_property_segment>(owning_type, property);
}

std::shared_ptr<odata_path_segment> odata_path_segment::create_navigation_property_segment(
    std::shared_ptr<::odata::edm::edm_structured_type> owning_type,
	std::shared_ptr<::odata::edm::edm_property_type> property,
    std::shared_ptr<::odata::edm::edm_navigation_type> navigation_type)
{
	return std::make_shared<odata_navigation_property_segment>(owning_type, property, navigation_type);
}

std::shared_ptr<odata_path_segment> odata_path_segment::create_dynamic_property_segment(const ::odata::utility::string_t &property_name)
{
	return std::make_shared<odata_dynamic_property_segment>(property_name);
}

std::shared_ptr<odata_path_segment> odata_path_segment::create_value_segment()
{
    return std::make_shared<odata_value_segment>();
}

std::shared_ptr<odata_path_segment> odata_path_segment::create_count_segment()
{
    return std::make_shared<odata_count_segment>();
}

std::shared_ptr<odata_path_segment> odata_path_segment::create_ref_segment()
{
    return std::make_shared<odata_ref_segment>();
}

std::shared_ptr<odata_path_segment> odata_path_segment::create_type_segment(std::shared_ptr<::odata::edm::edm_named_type> type)
{
    return std::make_shared<odata_type_segment>(type);
}

std::shared_ptr<odata_path_segment> odata_path_segment::create_operation_import_segment(
	std::shared_ptr<::odata::edm::edm_operation_import> operation_import,
	std::vector<std::pair<::odata::utility::string_t, std::shared_ptr<::odata::core::odata_primitive_value>>> parameters)
{
	return std::make_shared<odata_operation_import_segment>(operation_import, std::move(parameters));
}

std::shared_ptr<odata_path_segment> odata_path_segment::create_operation_segment(
	std::shared_ptr<::odata::edm::edm_operation_type> operation,
	std::vector<std::pair<::odata::utility::string_t, std::shared_ptr<::odata::core::odata_primitive_value>>> parameters)
{
	return std::make_shared<odata_operation_segment>(operation, std::move(parameters));
}

}}