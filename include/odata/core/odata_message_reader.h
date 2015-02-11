//---------------------------------------------------------------------
// <copyright file="odata_message_reader.h" company="Microsoft">
//      Copyright (C) Microsoft Corporation. All rights reserved. See License.txt in the project root for license information.
// </copyright>
//---------------------------------------------------------------------

#pragma once

#include "odata/common/utility.h"
#include "odata/edm/odata_edm.h"
#include "odata/core/odata_value.h"
#include "odata/common/json.h"
#include "odata/common/uri.h"

namespace odata { namespace core
{
class odata_message_reader
{
public:
	odata_message_reader(std::shared_ptr<::odata::edm::edm_model> model, ::odata::utility::uri base_uri, ::odata::utility::string_t message_body, bool is_response_message) 
		: m_model(model), m_base_uri(base_uri), m_message_body(message_body), m_is_response_message(is_response_message)
	{
	}

	ODATACPP_API std::shared_ptr<::odata::core::odata_value> read_odata_value();

	ODATACPP_API std::shared_ptr<::odata::core::odata_entity_value> read_entity_value(std::shared_ptr<::odata::edm::edm_entity_type> edm_type);
	ODATACPP_API std::shared_ptr<::odata::core::odata_entity_value> read_entity_value();

	ODATACPP_API std::shared_ptr<::odata::core::odata_collection_value> read_entity_collection(std::shared_ptr<::odata::edm::edm_entity_set> entity_set);
	ODATACPP_API std::shared_ptr<::odata::core::odata_collection_value> read_entity_collection();

	ODATACPP_API std::shared_ptr<::odata::core::odata_value> read_property(std::shared_ptr<::odata::edm::edm_named_type> edm_type);
	ODATACPP_API std::shared_ptr<::odata::core::odata_value> read_property();

	//ODATACPP_API std::shared_ptr<::odata::core::odata_batch_value> read_batch_value();

private:
	std::shared_ptr<::odata::edm::edm_model> m_model;
	::odata::utility::uri m_base_uri;
	bool m_is_response_message;
	::odata::utility::string_t m_message_body;
};

}}