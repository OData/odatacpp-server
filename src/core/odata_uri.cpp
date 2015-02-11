//---------------------------------------------------------------------
// <copyright file="odata_uri.cpp" company="Microsoft">
//      Copyright (C) Microsoft Corporation. All rights reserved. See License.txt in the project root for license information.
// </copyright>
//---------------------------------------------------------------------

#include "odata/core/odata_uri.h"

namespace odata { namespace core
{

std::shared_ptr<odata_uri> odata_uri::create_uri(
	std::shared_ptr<::odata::core::odata_path> path,
	std::shared_ptr<::odata::core::odata_select_expand_clause> select_expand_clause,
	std::shared_ptr<::odata::core::odata_filter_clause> filter_clause,
	std::shared_ptr<::odata::core::odata_orderby_clause> orderby_clause,
	std::shared_ptr<::odata::core::odata_search_clause> search_clause,
	::odata::common::nullable<int64_t> top,
	::odata::common::nullable<int64_t> skip,
	::odata::common::nullable<bool> count)
{
	return std::shared_ptr<odata_uri>(new odata_uri(
		path,
		select_expand_clause,
		filter_clause,
		orderby_clause,
		search_clause,
		top,
		skip,
		count));
}

}}