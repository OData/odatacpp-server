//---------------------------------------------------------------------
// <copyright file="odata_request_context.cpp" company="Microsoft">
//      Copyright (C) Microsoft Corporation. All rights reserved. See License.txt in the project root for license information.
// </copyright>
//---------------------------------------------------------------------

#include "odata_request_context.h"

using namespace ::std;
using namespace ::web::http;
using namespace ::odata::edm;
using namespace ::odata::core;

namespace odata { namespace service
{
	odata_request_context::odata_request_context(shared_ptr<http_request> request, shared_ptr<edm_model> model) : m_request(request), m_model(model)
	{
		m_uri_parser = make_shared<odata_uri_parser>(m_model);

		//TODO: handle exceptions here
		m_odata_uri = m_uri_parser->parse_uri(m_request->relative_uri());

		//TODO: temporary implementation here. 
		m_request_body = m_request->extract_json().get().serialize();
		
	}

}}