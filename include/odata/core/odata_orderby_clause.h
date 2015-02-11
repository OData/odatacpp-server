//---------------------------------------------------------------------
// <copyright file="odata_orderby_clause.h" company="Microsoft">
//      Copyright (C) Microsoft Corporation. All rights reserved. See License.txt in the project root for license information.
// </copyright>
//---------------------------------------------------------------------

#pragma once

namespace odata { namespace core
{

class odata_orderby_clause
{
public:
	odata_orderby_clause(
		std::vector<std::pair<std::shared_ptr<::odata::core::odata_query_node>, bool>> &&items,
		std::shared_ptr<::odata::core::odata_range_variable> range_variable)
		: m_items(items),
		m_range_variable(range_variable) {}
	~odata_orderby_clause() {}

	const std::vector<std::pair<std::shared_ptr<::odata::core::odata_query_node>, bool>> &items() const
	{ return m_items; }
	const std::pair<std::shared_ptr<::odata::core::odata_query_node>, bool> &item_at(::size_t i) const
	{ return m_items[i]; }
	std::shared_ptr<::odata::core::odata_range_variable> range_variable() const { return m_range_variable; }

private:
	std::vector<std::pair<std::shared_ptr<::odata::core::odata_query_node>, bool>> m_items;
	std::shared_ptr<::odata::core::odata_range_variable> m_range_variable;
};

}}