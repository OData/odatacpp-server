//---------------------------------------------------------------------
// <copyright file="odata_query_node_visitor.h" company="Microsoft">
//      Copyright (C) Microsoft Corporation. All rights reserved. See License.txt in the project root for license information.
// </copyright>
//---------------------------------------------------------------------

#pragma once

#include "odata/common/utility.h"
#include "odata/core/odata_query_node.h"

namespace odata { namespace core
{

template <typename T>
class odata_query_node_visitor : public std::enable_shared_from_this<odata_query_node_visitor<T>>
{
public:
	odata_query_node_visitor() {}
	virtual ~odata_query_node_visitor() {}

	virtual T visit(std::shared_ptr<::odata::core::odata_constant_node> node) = 0;
	virtual T visit(std::shared_ptr<::odata::core::odata_binary_operator_node> node) = 0;
	virtual T visit(std::shared_ptr<::odata::core::odata_unary_operator_node> node) = 0;
    virtual T visit(std::shared_ptr<::odata::core::odata_parameter_alias_node> node) = 0;
    virtual T visit(std::shared_ptr<::odata::core::odata_property_access_node> node) = 0;
	virtual T visit(std::shared_ptr<::odata::core::odata_type_cast_node> node) = 0;
    virtual T visit(std::shared_ptr<::odata::core::odata_lambda_node> node) = 0;
    virtual T visit(std::shared_ptr<::odata::core::odata_range_variable_node> node) = 0;
    virtual T visit(std::shared_ptr<::odata::core::odata_function_call_node> node) = 0;
	virtual T visit_any(std::shared_ptr<::odata::core::odata_query_node> node) = 0;
};

}}