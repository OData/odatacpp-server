//---------------------------------------------------------------------
// <copyright file="odata_uri.h" company="Microsoft">
//      Copyright (C) Microsoft Corporation. All rights reserved. See License.txt in the project root for license information.
// </copyright>
//---------------------------------------------------------------------

#pragma once

#include "odata/core/odata_path.h"
#include "odata/core/odata_select_expand_clause.h"
#include "odata/core/odata_filter_clause.h"
#include "odata/core/odata_select_expand_clause.h"
#include "odata/core/odata_orderby_clause.h"
#include "odata/core/odata_search_clause.h"
#include "odata/common/nullable.h"

namespace odata { namespace core
{

class odata_uri
{
public:
	odata_uri(
		std::shared_ptr<::odata::core::odata_path> path,
		std::shared_ptr<::odata::core::odata_select_expand_clause> select_expand_clause,
		std::shared_ptr<::odata::core::odata_filter_clause> filter_clause,
		std::shared_ptr<::odata::core::odata_orderby_clause> orderby_clause,
		std::shared_ptr<::odata::core::odata_search_clause> search_clause,
		::odata::common::nullable<int64_t> top,
		::odata::common::nullable<int64_t> skip,
		::odata::common::nullable<bool> count)
		: m_path(path),
		m_select_expand_clause(select_expand_clause),
		m_filter_clause(filter_clause),
		m_orderby_clause(orderby_clause),
		m_search_clause(search_clause),
		m_top(top),
		m_skip(skip),
		m_count(count) {}
	~odata_uri() {}

	static std::shared_ptr<odata_uri> create_uri(
		std::shared_ptr<::odata::core::odata_path> path,
		std::shared_ptr<::odata::core::odata_select_expand_clause> select_expand_clause,
		std::shared_ptr<::odata::core::odata_filter_clause> filter_clause,
		std::shared_ptr<::odata::core::odata_orderby_clause> orderby_clause,
		std::shared_ptr<::odata::core::odata_search_clause> search_clause,
		::odata::common::nullable<int64_t> top,
		::odata::common::nullable<int64_t> skip,
		::odata::common::nullable<bool> count);

	std::shared_ptr<::odata::core::odata_path> path() const { return m_path; }

	std::shared_ptr<::odata::core::odata_select_expand_clause> select_expand_clause() const { return m_select_expand_clause; }

	std::shared_ptr<::odata::core::odata_filter_clause> filter_clause() const { return m_filter_clause; }

	std::shared_ptr<::odata::core::odata_orderby_clause> orderby_clause() const { return m_orderby_clause; }

	std::shared_ptr<::odata::core::odata_search_clause> search_clause() const { return m_search_clause; }

	::odata::common::nullable<int64_t> top() const { return m_top; }

	::odata::common::nullable<int64_t> skip() const { return m_skip; }

	::odata::common::nullable<bool> count() const { return m_count; }

	bool is_service_document() const { return m_path->empty(); }

	bool is_metadata_document() const { return m_path->single(::odata::core::odata_path_segment_type::Metadata); }

private:
	std::shared_ptr<::odata::core::odata_path> m_path;
	std::shared_ptr<::odata::core::odata_select_expand_clause> m_select_expand_clause;
	std::shared_ptr<::odata::core::odata_filter_clause> m_filter_clause;
	std::shared_ptr<::odata::core::odata_orderby_clause> m_orderby_clause;
	std::shared_ptr<::odata::core::odata_search_clause> m_search_clause;

	::odata::common::nullable<int64_t> m_top;
	::odata::common::nullable<int64_t> m_skip;
	::odata::common::nullable<bool> m_count;
};

}}