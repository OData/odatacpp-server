//---------------------------------------------------------------------
// <copyright file="odata_query_node.h" company="Microsoft">
//      Copyright (C) Microsoft Corporation. All rights reserved. See License.txt in the project root for license information.
// </copyright>
//---------------------------------------------------------------------

#pragma once

#include "odata/common/utility.h"
#include "odata/edm/odata_edm.h"
#include "odata/core/odata_primitive_value.h"

namespace odata { namespace core
{

namespace odata_query_node_kind {
enum 
{
	None,
	Constant,
	BinaryOperator,
	UnaryOperator,
	ParameterAlias,
    PropertyAccess,
	TypeCast,
	Lambda,
    RangeVariable,
    FunctionCall
};
}

template <typename T>
class odata_query_node_visitor;

class odata_constant_node;
class odata_binary_operator_node;
class odata_unary_operator_node;
class odata_parameter_alias_node;
class odata_property_access_node;
class odata_type_cast_node;
class odata_lambda_node;
class odata_range_variable_node;
class odata_function_call_node;

class odata_query_node : public std::enable_shared_from_this<odata_query_node>
{
public:
	odata_query_node()
		: m_node_kind(odata_query_node_kind::None) {}
	virtual ~odata_query_node() {}

	int node_kind() const { return m_node_kind; }

	template <typename T>
	std::shared_ptr<T> as()
	{
		return std::static_pointer_cast<T>(shared_from_this());
	}

	ODATACPP_API static std::shared_ptr<odata_query_node> create_constant_node(std::shared_ptr<::odata::core::odata_primitive_value> value);
	ODATACPP_API static std::shared_ptr<odata_query_node> create_operator_or_node(std::shared_ptr<odata_query_node> left, std::shared_ptr<odata_query_node> right);
	ODATACPP_API static std::shared_ptr<odata_query_node> create_operator_and_node(std::shared_ptr<odata_query_node> left, std::shared_ptr<odata_query_node> right);
	ODATACPP_API static std::shared_ptr<odata_query_node> create_operator_eq_node(std::shared_ptr<odata_query_node> left, std::shared_ptr<odata_query_node> right);
	ODATACPP_API static std::shared_ptr<odata_query_node> create_operator_ne_node(std::shared_ptr<odata_query_node> left, std::shared_ptr<odata_query_node> right);
	ODATACPP_API static std::shared_ptr<odata_query_node> create_operator_gt_node(std::shared_ptr<odata_query_node> left, std::shared_ptr<odata_query_node> right);
	ODATACPP_API static std::shared_ptr<odata_query_node> create_operator_ge_node(std::shared_ptr<odata_query_node> left, std::shared_ptr<odata_query_node> right);
	ODATACPP_API static std::shared_ptr<odata_query_node> create_operator_lt_node(std::shared_ptr<odata_query_node> left, std::shared_ptr<odata_query_node> right);
	ODATACPP_API static std::shared_ptr<odata_query_node> create_operator_le_node(std::shared_ptr<odata_query_node> left, std::shared_ptr<odata_query_node> right);
	ODATACPP_API static std::shared_ptr<odata_query_node> create_operator_has_node(std::shared_ptr<odata_query_node> left, std::shared_ptr<odata_query_node> right);
	ODATACPP_API static std::shared_ptr<odata_query_node> create_operator_add_node(std::shared_ptr<odata_query_node> left, std::shared_ptr<odata_query_node> right);
	ODATACPP_API static std::shared_ptr<odata_query_node> create_operator_sub_node(std::shared_ptr<odata_query_node> left, std::shared_ptr<odata_query_node> right);
	ODATACPP_API static std::shared_ptr<odata_query_node> create_operator_mul_node(std::shared_ptr<odata_query_node> left, std::shared_ptr<odata_query_node> right);
	ODATACPP_API static std::shared_ptr<odata_query_node> create_operator_div_node(std::shared_ptr<odata_query_node> left, std::shared_ptr<odata_query_node> right);
	ODATACPP_API static std::shared_ptr<odata_query_node> create_operator_mod_node(std::shared_ptr<odata_query_node> left, std::shared_ptr<odata_query_node> right);
	ODATACPP_API static std::shared_ptr<odata_query_node> create_operator_not_node(std::shared_ptr<odata_query_node> operand);
	ODATACPP_API static std::shared_ptr<odata_query_node> create_operator_neg_node(std::shared_ptr<odata_query_node> operand);
	ODATACPP_API static std::shared_ptr<odata_query_node> create_parameter_alias_node(const ::odata::utility::string_t &alias);
	ODATACPP_API static std::shared_ptr<odata_query_node> create_property_access_node(
        const ::odata::utility::string_t &property_name,
        std::shared_ptr<odata_query_node> parent);
	ODATACPP_API static std::shared_ptr<odata_query_node> create_type_cast_node(
        const ::odata::utility::string_t &type_name,
        std::shared_ptr<odata_query_node> parent);
    ODATACPP_API static std::shared_ptr<odata_query_node> create_lambda_node(
        bool is_any,
        std::shared_ptr<odata_query_node> expression,
        const ::odata::utility::string_t &parameter,
        std::shared_ptr<odata_query_node> parent);
    ODATACPP_API static std::shared_ptr<odata_query_node> create_range_variable_node(const ::odata::utility::string_t &name);
    ODATACPP_API static std::shared_ptr<odata_query_node> create_function_call_node(
        const ::odata::utility::string_t &name,
        std::vector<std::pair<::odata::utility::string_t, std::shared_ptr<odata_query_node>>> parameters);

	template <typename T>
	T accept(std::shared_ptr<::odata::core::odata_query_node_visitor<T>> visitor)
	{
		switch (m_node_kind)
		{
		case odata_query_node_kind::Constant:
			return visitor->visit(as<::odata::core::odata_constant_node>());
		case odata_query_node_kind::BinaryOperator:
			return visitor->visit(as<::odata::core::odata_binary_operator_node>());
		case odata_query_node_kind::UnaryOperator:
			return visitor->visit(as<::odata::core::odata_unary_operator_node>());
        case odata_query_node_kind::ParameterAlias:
            return visitor->visit(as<::odata::core::odata_parameter_alias_node>());
        case odata_query_node_kind::PropertyAccess:
            return visitor->visit(as<::odata::core::odata_property_access_node>());
		case odata_query_node_kind::TypeCast:
			return visitor->visit(as<::odata::core::odata_type_cast_node>());
        case odata_query_node_kind::Lambda:
            return visitor->visit(as<::odata::core::odata_lambda_node>());
        case odata_query_node_kind::RangeVariable:
            return visitor->visit(as<::odata::core::odata_range_variable_node>());
        case odata_query_node_kind::FunctionCall:
            return visitor->visit(as<::odata::core::odata_function_call_node>());
		}

		return visitor->visit_any(shared_from_this());
	}

protected:
	odata_query_node(int kind)
		: m_node_kind(kind) {}

private:
	// Make odata_query_node and all its subclasses non-copyable.
	odata_query_node(const odata_query_node &);
	odata_query_node &operator=(const odata_query_node &);

	int m_node_kind;
};

//class odata_single_value_node : public odata_query_node
//{
//public:
//	odata_single_value_node(int node_kind, std::shared_ptr<::odata::edm::edm_named_type> value_type)
//		: odata_query_node(node_kind),
//		m_value_type(value_type) {}
//	virtual ~odata_single_value_node() {}
//
//	std::shared_ptr<::odata::edm::edm_named_type> value_type() const { return m_value_type.lock(); }
//
//	void set_value_type(std::shared_ptr<::odata::edm::edm_named_type> value_type)
//	{ m_value_type = value_type; }
//
//private:
//	std::weak_ptr<::odata::edm::edm_named_type> m_value_type;
//};

class odata_constant_node : public odata_query_node
{
public:
	odata_constant_node(std::shared_ptr<::odata::core::odata_primitive_value> value)
		: odata_query_node(odata_query_node_kind::Constant),
		m_value(value) {}
	~odata_constant_node() {}

	std::shared_ptr<::odata::core::odata_primitive_value> value() const { return m_value; }

private:
	std::shared_ptr<::odata::core::odata_primitive_value> m_value;
};

namespace binary_operator_kind {
enum {
	Or,
	And,
	Equal,
	NotEqual,
	GreaterThan,
	GreaterThanOrEqual,
	LessThan,
	LessThanOrEqual,
	Has,
	Add,
	Sub,
	Multiply,
	Divide,
	Modulo
};
}

class odata_binary_operator_node : public odata_query_node
{
public:
	odata_binary_operator_node(int operator_kind, std::shared_ptr<odata_query_node> left, std::shared_ptr<odata_query_node> right)
		: odata_query_node(odata_query_node_kind::BinaryOperator),
		m_operator_kind(operator_kind),
		m_left(left),
		m_right(right) {}
	~odata_binary_operator_node() {}

	int operator_kind() const { return m_operator_kind; }
	std::shared_ptr<odata_query_node> left() const { return m_left; }
	std::shared_ptr<odata_query_node> right() const { return m_right; }

private:
	int m_operator_kind;
	std::shared_ptr<odata_query_node> m_left;
	std::shared_ptr<odata_query_node> m_right;
};

namespace unary_operator_kind {
enum {
	Negate,
	Not
};
}

class odata_unary_operator_node : public odata_query_node
{
public:
	odata_unary_operator_node(int operator_kind, std::shared_ptr<odata_query_node> operand)
		: odata_query_node(odata_query_node_kind::UnaryOperator),
		m_operator_kind(operator_kind),
		m_operand(operand) {}
	~odata_unary_operator_node() {}

	int operator_kind() const { return m_operator_kind; }
	std::shared_ptr<odata_query_node> operand() const { return m_operand; }

private:
	int m_operator_kind;
	std::shared_ptr<odata_query_node> m_operand;
};

class odata_parameter_alias_node : public odata_query_node
{
public:
	odata_parameter_alias_node(const ::odata::utility::string_t &alias)
		: odata_query_node(odata_query_node_kind::ParameterAlias),
		m_alias(alias) {}
	~odata_parameter_alias_node() {}

	const ::odata::utility::string_t &alias() const { return m_alias; }

private:
	::odata::utility::string_t m_alias;
};

class odata_property_access_node : public odata_query_node
{
public:
	odata_property_access_node(
        const ::odata::utility::string_t &property_name,
        std::shared_ptr<odata_query_node> parent)
		: odata_query_node(odata_query_node_kind::PropertyAccess),
		m_property_name(property_name),
        m_parent(parent) {}
	~odata_property_access_node() {}

    const ::odata::utility::string_t &property_name() const { return m_property_name; }
    std::shared_ptr<odata_query_node> parent() const { return m_parent; }

private:
	::odata::utility::string_t m_property_name;
    std::shared_ptr<odata_query_node> m_parent;
};

class odata_type_cast_node : public odata_query_node
{
public:
	odata_type_cast_node(
        const ::odata::utility::string_t &type_name,
        std::shared_ptr<odata_query_node> parent)
		: odata_query_node(odata_query_node_kind::TypeCast),
		m_type_name(type_name),
        m_parent(parent) {}
	~odata_type_cast_node() {}

    const ::odata::utility::string_t &type_name() const { return m_type_name; }
    std::shared_ptr<odata_query_node> parent() const { return m_parent; }

private:
	::odata::utility::string_t m_type_name;
    std::shared_ptr<odata_query_node> m_parent;
};

class odata_lambda_node : public odata_query_node
{
public:
	odata_lambda_node(
        bool is_any,
        std::shared_ptr<odata_query_node> expression,
        const ::odata::utility::string_t &parameter,
        std::shared_ptr<odata_query_node> parent)
		: odata_query_node(odata_query_node_kind::Lambda),
        m_is_any(is_any),
		m_expression(expression),
        m_parameter(parameter),
        m_parent(parent) {}
	~odata_lambda_node() {}

    bool is_any() const { return m_is_any; }
	std::shared_ptr<odata_query_node> expression() const { return m_expression; }
    const ::odata::utility::string_t &parameter() const { return m_parameter; }
    std::shared_ptr<odata_query_node> parent() const { return m_parent; }

private:
    std::shared_ptr<odata_query_node> m_expression;
	::odata::utility::string_t m_parameter;
    std::shared_ptr<odata_query_node> m_parent;
    bool m_is_any;
};

class odata_range_variable_node : public odata_query_node
{
public:
	odata_range_variable_node(const ::odata::utility::string_t &name)
		: odata_query_node(odata_query_node_kind::RangeVariable),
		m_name(name) {}
	~odata_range_variable_node() {}

    const ::odata::utility::string_t &name() const { return m_name; }

private:
	::odata::utility::string_t m_name;
};

class odata_function_call_node : public odata_query_node
{
public:
    odata_function_call_node(
        const ::odata::utility::string_t &name,
        std::vector<std::pair<::odata::utility::string_t, std::shared_ptr<odata_query_node>>> &&parameters)
        : odata_query_node(odata_query_node_kind::FunctionCall),
        m_name(name),
        m_parameters(parameters) {}
    ~odata_function_call_node() {}
    
    const ::odata::utility::string_t &name() const { return m_name; }
    const std::pair<::odata::utility::string_t, std::shared_ptr<odata_query_node>> &parameter_at(::size_t i) const
    { return m_parameters[i]; }
    const std::vector<std::pair<::odata::utility::string_t, std::shared_ptr<odata_query_node>>> &parameters() const
    { return m_parameters; }
    
private:
    ::odata::utility::string_t m_name;
    std::vector<std::pair<::odata::utility::string_t, std::shared_ptr<odata_query_node>>> m_parameters;
};

}}