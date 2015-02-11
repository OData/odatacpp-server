//---------------------------------------------------------------------
// <copyright file="query_handler.cpp" company="Microsoft">
//      Copyright (C) Microsoft Corporation. All rights reserved. See License.txt in the project root for license information.
// </copyright>
//---------------------------------------------------------------------

#include "query_handler.h"
#include "data_source_task.h"
#include "odata_value_builder.h"
#include "odata_service_exception.h"
#include "odata/core/odata_message_writer.h"
#include "odata/core/odata_context_url_builder.h"

using namespace ::std;
using namespace ::web::http;
using namespace ::utility;
using namespace ::odata::edm;
using namespace ::odata::core;

namespace odata { namespace service
{
	query_handler::query_handler(shared_ptr<http_request> request, shared_ptr<edm_model> model): m_request(request), m_model(model)
	{
		m_request_context = make_shared<odata_request_context>(m_request, m_model);
	}

	void query_handler::handle()
	{
		::odata::utility::string_t media_type = U("application/json");

		auto target = m_request_context->get_query_target();

		shared_ptr<mongo_task> ds_task = make_shared<mongo_query_task>(m_request_context);
		const std::vector<::mongo::BSONObj> &result = ds_task->run();

		auto value_builder = make_shared<odata_value_builder>();
		auto target_value = value_builder->build_odata_value(target->target_edm_type(), result);
	    
		uri context_url;
		odata_context_url_builder context_url_builder(m_model, U("http://localhost:4789"));
		if (target->target_edm_type()->get_type_kind() == edm_type_kind_t::Collection)
		{
			auto collection_type = std::dynamic_pointer_cast<edm_collection_type>(target->target_edm_type());
			auto element_type = collection_type->get_element_type();
			if (element_type->get_type_kind() == edm_type_kind_t::Entity)
			{
				auto target_entity_set = std::dynamic_pointer_cast<edm_entity_set>(target->target_navigation_source());
				context_url = context_url_builder.get_context_uri_for_collection_of_entities(target_entity_set);
			}
		}
		else if (target->target_edm_type()->get_type_kind() == edm_type_kind_t::Entity)
		{
			auto entity_type = std::dynamic_pointer_cast<edm_entity_type>(target->target_edm_type());
			auto target_entity_set = std::dynamic_pointer_cast<edm_entity_set>(target->target_navigation_source());
			context_url = context_url_builder.get_context_uri_for_entity(target_entity_set);
		}

		if (!context_url.is_empty())
		{
			target_value->set_context_url(context_url);
		}

		odata_message_writer writer(m_model, U("http://localhost:4789"));
		auto content = writer.write_odata_value(target_value);

		http_response response(status_codes::OK);
		response.set_body(content, media_type);

		m_request->reply(response).get();
	}
}}