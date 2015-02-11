//---------------------------------------------------------------------
// <copyright file="odata_query_node.cpp" company="Microsoft">
//      Copyright (C) Microsoft Corporation. All rights reserved. See License.txt in the project root for license information.
// </copyright>
//---------------------------------------------------------------------

#include "odata/core/odata_query_node.h"

namespace odata { namespace core
{

std::shared_ptr<odata_query_node> odata_query_node::create_constant_node(std::shared_ptr<::odata::core::odata_primitive_value> value)
{
	return std::make_shared<odata_constant_node>(value);
}

std::shared_ptr<odata_query_node> odata_query_node::create_operator_or_node(std::shared_ptr<odata_query_node> left, std::shared_ptr<odata_query_node> right)
{
	return std::make_shared<odata_binary_operator_node>(::odata::core::binary_operator_kind::Or, left, right);
}

std::shared_ptr<odata_query_node> odata_query_node::create_operator_and_node(std::shared_ptr<odata_query_node> left, std::shared_ptr<odata_query_node> right)
{
	return std::make_shared<odata_binary_operator_node>(::odata::core::binary_operator_kind::And, left, right);
}

std::shared_ptr<odata_query_node> odata_query_node::create_operator_eq_node(std::shared_ptr<odata_query_node> left, std::shared_ptr<odata_query_node> right)
{
	return std::make_shared<odata_binary_operator_node>(::odata::core::binary_operator_kind::Equal, left, right);
}

std::shared_ptr<odata_query_node> odata_query_node::create_operator_ne_node(std::shared_ptr<odata_query_node> left, std::shared_ptr<odata_query_node> right)
{
	return std::make_shared<odata_binary_operator_node>(::odata::core::binary_operator_kind::NotEqual, left, right);
}

std::shared_ptr<odata_query_node> odata_query_node::create_operator_gt_node(std::shared_ptr<odata_query_node> left, std::shared_ptr<odata_query_node> right)
{
	return std::make_shared<odata_binary_operator_node>(::odata::core::binary_operator_kind::GreaterThan, left, right);
}

std::shared_ptr<odata_query_node> odata_query_node::create_operator_ge_node(std::shared_ptr<odata_query_node> left, std::shared_ptr<odata_query_node> right)
{
	return std::make_shared<odata_binary_operator_node>(::odata::core::binary_operator_kind::GreaterThanOrEqual, left, right);
}

std::shared_ptr<odata_query_node> odata_query_node::create_operator_lt_node(std::shared_ptr<odata_query_node> left, std::shared_ptr<odata_query_node> right)
{
	return std::make_shared<odata_binary_operator_node>(::odata::core::binary_operator_kind::LessThan, left, right);
}

std::shared_ptr<odata_query_node> odata_query_node::create_operator_le_node(std::shared_ptr<odata_query_node> left, std::shared_ptr<odata_query_node> right)
{
	return std::make_shared<odata_binary_operator_node>(::odata::core::binary_operator_kind::LessThanOrEqual, left, right);
}

std::shared_ptr<odata_query_node> odata_query_node::create_operator_has_node(std::shared_ptr<odata_query_node> left, std::shared_ptr<odata_query_node> right)
{
	return std::make_shared<odata_binary_operator_node>(::odata::core::binary_operator_kind::Has, left, right);
}

std::shared_ptr<odata_query_node> odata_query_node::create_operator_add_node(std::shared_ptr<odata_query_node> left, std::shared_ptr<odata_query_node> right)
{
	return std::make_shared<odata_binary_operator_node>(::odata::core::binary_operator_kind::Add, left, right);
}

std::shared_ptr<odata_query_node> odata_query_node::create_operator_sub_node(std::shared_ptr<odata_query_node> left, std::shared_ptr<odata_query_node> right)
{
	return std::make_shared<odata_binary_operator_node>(::odata::core::binary_operator_kind::Sub, left, right);
}

std::shared_ptr<odata_query_node> odata_query_node::create_operator_mul_node(std::shared_ptr<odata_query_node> left, std::shared_ptr<odata_query_node> right)
{
	return std::make_shared<odata_binary_operator_node>(::odata::core::binary_operator_kind::Multiply, left, right);
}

std::shared_ptr<odata_query_node> odata_query_node::create_operator_div_node(std::shared_ptr<odata_query_node> left, std::shared_ptr<odata_query_node> right)
{
	return std::make_shared<odata_binary_operator_node>(::odata::core::binary_operator_kind::Divide, left, right);
}

std::shared_ptr<odata_query_node> odata_query_node::create_operator_mod_node(std::shared_ptr<odata_query_node> left, std::shared_ptr<odata_query_node> right)
{
	return std::make_shared<odata_binary_operator_node>(::odata::core::binary_operator_kind::Modulo, left, right);
}

std::shared_ptr<odata_query_node> odata_query_node::create_operator_not_node(std::shared_ptr<odata_query_node> operand)
{
	return std::make_shared<odata_unary_operator_node>(::odata::core::unary_operator_kind::Not, operand);
}

std::shared_ptr<odata_query_node> odata_query_node::create_operator_neg_node(std::shared_ptr<odata_query_node> operand)
{
	return std::make_shared<odata_unary_operator_node>(::odata::core::unary_operator_kind::Negate, operand);
}

std::shared_ptr<odata_query_node> odata_query_node::create_parameter_alias_node(const ::odata::utility::string_t &alias)
{
	return std::make_shared<odata_parameter_alias_node>(alias);
}

std::shared_ptr<odata_query_node> odata_query_node::create_property_access_node(
    const ::odata::utility::string_t &property_name,
    std::shared_ptr<odata_query_node> parent)
{
    return std::make_shared<odata_property_access_node>(property_name, parent);
}

std::shared_ptr<odata_query_node> odata_query_node::create_type_cast_node(
    const ::odata::utility::string_t &type_name,
    std::shared_ptr<odata_query_node> parent)
{
    return std::make_shared<odata_type_cast_node>(type_name, parent);
}

std::shared_ptr<odata_query_node> odata_query_node::create_lambda_node(
    bool is_any,
    std::shared_ptr<odata_query_node> expression,
    const ::odata::utility::string_t &parameter,
    std::shared_ptr<odata_query_node> parent)
{
    return std::make_shared<odata_lambda_node>(is_any, expression, parameter, parent);
}

std::shared_ptr<odata_query_node> odata_query_node::create_range_variable_node(const ::odata::utility::string_t &name)
{
    return std::make_shared<odata_range_variable_node>(name);
}

std::shared_ptr<odata_query_node> odata_query_node::create_function_call_node(
    const ::odata::utility::string_t &name,
    std::vector<std::pair<::odata::utility::string_t, std::shared_ptr<odata_query_node>>> parameters)
{
    return std::make_shared<odata_function_call_node>(name, std::move(parameters));
}

}}