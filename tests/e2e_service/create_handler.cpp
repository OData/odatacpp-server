//---------------------------------------------------------------------
// <copyright file="create_handler.cpp" company="Microsoft">
//      Copyright (C) Microsoft Corporation. All rights reserved. See License.txt in the project root for license information.
// </copyright>
//---------------------------------------------------------------------

#include "cpprest/asyncrt_utils.h"
#include "odata_value_builder.h"
#include "odata/core/odata_message_writer.h"
#include "odata/core/odata_message_reader.h"
#include "data_source_task.h"
#include "odata_service_exception.h"
#include "create_handler.h"


using namespace ::std;
using namespace ::web::http;
using namespace ::odata::edm;

namespace odata { namespace service
{
	create_handler::create_handler(shared_ptr<http_request> request, shared_ptr<edm_model> model): m_request(request), m_model(model)
	{
		m_request_context = make_shared<odata_request_context>(m_request, m_model);
	}

	void create_handler::handle()
	{
		auto target = m_request_context->get_query_target();

		auto target_edm_type = target->target_edm_type();

		if (!target_edm_type || target_edm_type->get_type_kind() != edm_type_kind_t::Collection)
		{
			throw new odata_service_exception(U("Only creating an entity in entity set is supported."));
		}

		auto entity_collection_type = std::dynamic_pointer_cast<edm_collection_type>(target_edm_type);
		auto entity_type = std::dynamic_pointer_cast<edm_entity_type>(entity_collection_type->get_element_type());
		if (!entity_type)
		{
			throw new odata_service_exception(U("Only creating an entity in entity set is supported."));
		}

		::odata::core::odata_message_reader reader(m_model, U("http://localhost:4789"), m_request_context->get_request_body(), false);
		auto entity_value = reader.read_entity_value(entity_type);
		
		shared_ptr<mongo_task> ds_task = make_shared<mongo_insert_task>(m_request_context, entity_value);
		const std::vector<::mongo::BSONObj> &result = ds_task->run();

		auto value_builder = make_shared<odata_value_builder>();
		auto target_value = value_builder->build_odata_value(target->target_edm_type(), result);
	    
		::odata::core::odata_message_writer writer(m_model, U("http://localhost:4789"));
		auto content = writer.write_odata_value(target_value);

		m_request->reply(status_codes::Created, content).get();
	}
}}