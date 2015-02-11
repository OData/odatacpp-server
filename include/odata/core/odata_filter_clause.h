//---------------------------------------------------------------------
// <copyright file="odata_filter_clause.h" company="Microsoft">
//      Copyright (C) Microsoft Corporation. All rights reserved. See License.txt in the project root for license information.
// </copyright>
//---------------------------------------------------------------------

#pragma once

#include "odata/common/utility.h"

namespace odata { namespace core
{

class odata_query_node;
class odata_range_variable;

class odata_filter_clause
{
public:
	odata_filter_clause(
        std::shared_ptr<::odata::core::odata_query_node> expression,
        std::shared_ptr<::odata::core::odata_range_variable> range_variable)
		: m_expression(expression),
		m_range_variable(range_variable) {}
	~odata_filter_clause() {}

	std::shared_ptr<::odata::core::odata_query_node> expression() const { return m_expression; }
	std::shared_ptr<::odata::core::odata_range_variable> range_variable() const { return m_range_variable; }

private:
	std::shared_ptr<::odata::core::odata_query_node> m_expression;
	std::shared_ptr<::odata::core::odata_range_variable> m_range_variable;
};

}}