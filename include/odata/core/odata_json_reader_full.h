//---------------------------------------------------------------------
// <copyright file="odata_json_reader_full.h" company="Microsoft">
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

class entity_json_reader_full
{
public:
	entity_json_reader_full(std::shared_ptr<::odata::edm::edm_model> model, const ::odata::utility::string_t& service_root_url)
		: m_model(model), m_service_root_url(service_root_url)
	{
	};
	
	ODATACPP_API std::shared_ptr<odata_value> deserilize(const ::odata::utility::json::value& response, std::shared_ptr<::odata::edm::edm_entity_set> set);
   
private:
    std::shared_ptr<::odata::edm::edm_model> m_model;
	::odata::utility::string_t m_service_root_url; 

};

}}