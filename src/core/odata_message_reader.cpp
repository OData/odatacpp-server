//---------------------------------------------------------------------
// <copyright file="odata_message_reader.cpp" company="Microsoft">
//      Copyright (C) Microsoft Corporation. All rights reserved. See License.txt in the project root for license information.
// </copyright>
//---------------------------------------------------------------------

#include "odata/core/odata_message_reader.h"
#include "odata/core/odata_json_reader_minimal.h"

using namespace ::odata::core;

namespace odata { namespace core
{
	std::shared_ptr<odata_value> odata_message_reader::read_odata_value()
	{
		auto json_reader = std::make_shared<odata_json_reader_minimal>(m_model, m_base_uri.to_string(), m_is_response_message);
		return json_reader->deserilize(::odata::utility::json::value::parse(m_message_body));
	}

	std::shared_ptr<odata_entity_value> odata_message_reader::read_entity_value(std::shared_ptr<::odata::edm::edm_entity_type> edm_type)
	{
		auto json_reader = std::make_shared<odata_json_reader_minimal>(m_model, m_base_uri.to_string(), m_is_response_message);
		return json_reader->deserilize_entity_value(::odata::utility::json::value::parse(m_message_body), edm_type);
	}

	std::shared_ptr<odata_entity_value> odata_message_reader::read_entity_value()
	{
		return read_entity_value(nullptr);
	}

	std::shared_ptr<odata_collection_value> odata_message_reader::read_entity_collection(std::shared_ptr<::odata::edm::edm_entity_set> entity_set)
	{
		auto json_reader = std::make_shared<odata_json_reader_minimal>(m_model, m_base_uri.to_string(), m_is_response_message);
		return json_reader->deserilize_entity_collection(::odata::utility::json::value::parse(m_message_body), entity_set);
	}

	std::shared_ptr<::odata::core::odata_collection_value> odata_message_reader::read_entity_collection()
	{
		return read_entity_collection(nullptr);
	}

	std::shared_ptr<odata_value> odata_message_reader::read_property(std::shared_ptr<::odata::edm::edm_named_type> edm_type)
	{
		auto json_reader = std::make_shared<odata_json_reader_minimal>(m_model, m_base_uri.to_string(), m_is_response_message);
		return json_reader->deserilize_property(::odata::utility::json::value::parse(m_message_body), edm_type);
	}

	std::shared_ptr<odata_value> odata_message_reader::read_property()
	{
		return read_property(nullptr);
	}
}}