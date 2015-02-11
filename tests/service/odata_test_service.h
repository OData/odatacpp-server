//---------------------------------------------------------------------
// <copyright file="odata_test_service.h" company="Microsoft">
//      Copyright (C) Microsoft Corporation. All rights reserved. See License.txt in the project root for license information.
// </copyright>
//---------------------------------------------------------------------

#include "cpprest/http_listener.h"
#include "cpprest/json.h"
#include <agents.h>
#include <locale>
#include <ctime>
#include "odata/edm/edm_model_reader.h"
#include "odata/core/odata_uri_parser.h"
#include "odata/core/odata_message_writer.h"
#include "odata/core/odata_select_expand_clause.h"
#include "odata/core/odata_uri.h"
#include "odata/core/odata_collection_value.h"
#include "odata/core/odata_value.h"
#include "odata/core/odata_context_url_builder.h"
#include "odata/core/odata_metadata_builder.h"

namespace odata { namespace service
{

class odata_test_service
{
public:
    odata_test_service(::odata::utility::string_t url, std::shared_ptr<::odata::edm::edm_model> model,std::shared_ptr<::odata::core::odata_service_document> service_document);

    pplx::task<void> open();
    pplx::task<void> close();

	void initialize_datasource();

    void handle_get(web::http::http_request message);

private:
	std::shared_ptr<::odata::core::odata_path> process_request_uri(const ::web::uri &relative_uri);
    std::shared_ptr<::odata::core::odata_value> get_people();
    std::shared_ptr<::odata::core::odata_entity_value> get_single_people(::odata::utility::string_t name);

	static void handle_error(pplx::task<void>& t);

	// HTTP listener 
    web::http::experimental::listener::http_listener m_listener;
	std::shared_ptr<::odata::core::odata_uri_parser> m_uri_parser;

	std::shared_ptr<::odata::edm::edm_model> m_model;
	std::shared_ptr<::odata::core::odata_service_document> m_service_document;

	web::uri m_service_root;
};

}}