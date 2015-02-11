//---------------------------------------------------------------------
// <copyright file="odata_search_clause.h" company="Microsoft">
//      Copyright (C) Microsoft Corporation. All rights reserved. See License.txt in the project root for license information.
// </copyright>
//---------------------------------------------------------------------

#pragma once

namespace odata { namespace core
{

class odata_search_clause
{
public:
	odata_search_clause(std::shared_ptr<::odata::core::odata_query_node> expression)
		: m_expression(expression) {}
	~odata_search_clause() {}

	std::shared_ptr<::odata::core::odata_query_node> expression() const { return m_expression; }

private:
	std::shared_ptr<::odata::core::odata_query_node> m_expression;
};

}}