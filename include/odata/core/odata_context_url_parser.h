//---------------------------------------------------------------------
// <copyright file="odata_context_url_parser.h" company="Microsoft">
//      Copyright (C) Microsoft Corporation. All rights reserved. See License.txt in the project root for license information.
// </copyright>
//---------------------------------------------------------------------

#pragma once

#include "odata/common/utility.h"
#include "odata/core/odata_core.h"
#include "odata/common/json.h"
#include "odata/edm/odata_edm.h"

namespace odata { namespace core
{

class odata_contex_url_parser
{
public:
	odata_contex_url_parser(std::shared_ptr<::odata::edm::edm_model> model, const ::odata::utility::string_t& service_root_url) 
		: m_model(model), m_service_root_url(service_root_url)
	{
	}

	ODATACPP_API std::shared_ptr<::odata::edm::edm_named_type> get_payload_content_type(const ::odata::utility::string_t& context_url);

private:
	std::shared_ptr<::odata::edm::edm_named_type> parse_complex_or_primitive(const ::odata::utility::string_t& current_path);
	bool end_with(const ::odata::utility::string_t& s1, const ::odata::utility::string_t& s2);
	std::shared_ptr<::odata::edm::edm_named_type> resolve_literal_type(const ::odata::utility::string_t& type_name);

	std::shared_ptr<::odata::edm::edm_model>  m_model;
	::odata::utility::string_t m_service_root_url; 
};

}}