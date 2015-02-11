//---------------------------------------------------------------------
// <copyright file="odata_request_context.h" company="Microsoft">
//      Copyright (C) Microsoft Corporation. All rights reserved. See License.txt in the project root for license information.
// </copyright>
//---------------------------------------------------------------------

#pragma once

#include "cpprest/http_msg.h"
#include "odata/edm/edm_model.h"
#include "odata/core/odata_uri_parser.h"
#include "query_target.h"

namespace odata { namespace service
{

class odata_request_context
{
public:
	odata_request_context(::std::shared_ptr<::web::http::http_request> request, ::std::shared_ptr<::odata::edm::edm_model> model);
	~odata_request_context() {};

	::std::shared_ptr<::odata::core::odata_path> get_odata_path()
	{
		return m_odata_uri->path();
	}

	::std::shared_ptr<::odata::core::odata_filter_clause> get_filter_clause()
	{
		return m_odata_uri->filter_clause();
	}

	::std::shared_ptr<::odata::core::odata_orderby_clause> get_orderby_clause()
	{
		return m_odata_uri->orderby_clause();
	}

	::std::shared_ptr<query_target> get_query_target()
	{
		return query_target::resolve_path(get_odata_path());
	}

	::odata::utility::string_t &get_request_body()
	{
		return m_request_body;
	}

	::web::http::http_headers& request_headers()
	{
		return m_request->headers();
	}

private:
	::std::shared_ptr<::web::http::http_request> m_request;
	::std::shared_ptr<::odata::edm::edm_model> m_model;
	::std::shared_ptr<::odata::core::odata_uri_parser> m_uri_parser;
	::std::shared_ptr<::odata::core::odata_uri> m_odata_uri;

	//TODO: temporary member
	::odata::utility::string_t m_request_body;
};

}}