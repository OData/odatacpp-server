//---------------------------------------------------------------------
// <copyright file="odata_json_reader_minimal.h" company="Microsoft">
//      Copyright (C) Microsoft Corporation. All rights reserved. See License.txt in the project root for license information.
// </copyright>
//---------------------------------------------------------------------

#pragma once

#include "odata/common/json.h"
#include "odata/common/utility.h"
#include "odata/core/odata_core.h"
#include "odata/edm/odata_edm.h"

namespace odata { namespace core
{

class odata_json_reader_minimal
{
public:
	odata_json_reader_minimal(std::shared_ptr<::odata::edm::edm_model> model, const ::odata::utility::string_t& service_root_url, bool is_reading_response = true) 
		: m_model(model), m_service_root_url(service_root_url), m_is_reading_response(is_reading_response)
	{
	};
	
	ODATACPP_API std::shared_ptr<odata_value> deserilize(const odata::utility::json::value& content);

	ODATACPP_API std::shared_ptr<odata_entity_value> deserilize_entity_value(const odata::utility::json::value& content, std::shared_ptr<::odata::edm::edm_entity_type> entity_type);

	ODATACPP_API std::shared_ptr<odata_collection_value> deserilize_entity_collection(const odata::utility::json::value& content, std::shared_ptr<::odata::edm::edm_entity_set> entity_set);
	
	ODATACPP_API std::shared_ptr<odata_value> deserilize_property(const odata::utility::json::value& content, std::shared_ptr<::odata::edm::edm_named_type> edm_type);
   
private:
	std::shared_ptr<odata_entity_value> handle_extract_entity_property(const odata::utility::json::value& value, std::shared_ptr<::odata::edm::edm_entity_type>& entity_type);
    std::shared_ptr<odata_complex_value> handle_extract_complex_property(const ::odata::utility::json::value& value, std::shared_ptr<::odata::edm::edm_complex_type>& edm_complex_type);
	std::shared_ptr<odata_collection_value> handle_extract_collection_property(std::shared_ptr<::odata::edm::edm_named_type> type, const odata::utility::json::value& value);
	void handle_extract_entity_annotation(const ::odata::utility::string_t& annotation, const ::odata::utility::string_t& value, std::shared_ptr<odata_structured_value>& entity_value);
	std::shared_ptr<odata_value> handle_extract_navigation_property(const ::odata::utility::json::value& value, std::shared_ptr<::odata::edm::edm_navigation_type> navigation_type);
	::odata::utility::string_t get_navigation_source_from_context_url(const ::odata::utility::string_t& context_url);
	void set_edit_link_for_entity_value(const std::shared_ptr<odata_entity_value>& entity_value, const ::odata::utility::string_t& expect_type_name, const ::odata::utility::string_t& navigation_source);
	void set_edit_link_for_entity_collection_value(const std::shared_ptr<odata_collection_value>& entity_collection_value, const ::odata::utility::string_t& expect_type_name, const ::odata::utility::string_t& navigation_source);
	std::shared_ptr<::odata::edm::edm_named_type> prepare_for_reading(const odata::utility::json::value& content, std::shared_ptr<::odata::edm::edm_named_type> edm_type);

	std::shared_ptr<::odata::edm::edm_model> m_model;
	::odata::utility::string_t m_service_root_url; 
    std::shared_ptr<::odata::edm::edm_entity_set> m_entity_set;
	bool m_is_reading_response;
};

}}