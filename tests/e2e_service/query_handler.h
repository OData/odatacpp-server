//---------------------------------------------------------------------
// <copyright file="query_handler.h" company="Microsoft">
//      Copyright (C) Microsoft Corporation. All rights reserved. See License.txt in the project root for license information.
// </copyright>
//---------------------------------------------------------------------

#pragma once

#include "cpprest/http_msg.h"
#include "odata/edm/edm_model.h"
#include "odata_request_context.h"

namespace odata { namespace service
{

class query_handler
{
public:
	query_handler(::std::shared_ptr<::web::http::http_request> request, ::std::shared_ptr<::odata::edm::edm_model> model);
	~query_handler() {};

	void handle();

private:
    ::std::shared_ptr<::web::http::http_request> m_request;
	::std::shared_ptr<::odata::edm::edm_model> m_model;
	::std::shared_ptr<odata_request_context> m_request_context;
};

}}