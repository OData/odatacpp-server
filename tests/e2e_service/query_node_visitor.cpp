//---------------------------------------------------------------------
// <copyright file="query_node_visitor.cpp" company="Microsoft">
//      Copyright (C) Microsoft Corporation. All rights reserved. See License.txt in the project root for license information.
// </copyright>
//---------------------------------------------------------------------

#pragma once

#include "query_node_visitor.h"
#include "odata_service_exception.h"

using namespace ::odata::core;
using namespace ::odata::edm;

namespace odata { namespace service
{

::mongo::BSONObj node_to_bson_visitor::translate_node(std::shared_ptr<odata_query_node> node)
{
	if (!node)
	{
		throw new odata_service_exception(U("unexpected null node"));
	}

	return node->accept(shared_from_this());
}
	
::mongo::BSONObj node_to_bson_visitor::visit(std::shared_ptr<odata_constant_node> node)
{
	if (!node)
	{
		throw new odata_service_exception(U("unexpected null node"));
	}

	auto constant_value = node->value();
	auto primitive_type = std::dynamic_pointer_cast<edm_primitive_type>(constant_value->get_value_type());
	switch (primitive_type->get_primitive_kind())
	{
		case edm_primitive_type_kind_t::String:
			{
				auto string_value = constant_value->as<::odata::utility::string_t>();
				::std::string string_value_s(string_value.cbegin(), string_value.cend());
				return BSON("query" << string_value_s);
			}
			break;
		case edm_primitive_type_kind_t::Int32:
			{
				auto int_value = constant_value->as<int>();
				return BSON("query" << int_value);
			}
			break;
		default:
			{
				throw new odata_service_exception(U("service does not implement the primitive type in $query option"));
			}
			break;
	}
}
	
    
::mongo::BSONObj node_to_bson_visitor::visit(std::shared_ptr<odata_binary_operator_node> node)
{
	if (!node)
	{
		throw new odata_service_exception(U("unexpected null node"));
	}

	auto left = node->left();
	auto right = node->right();

	auto left_exp = translate_node(left);
	auto right_exp = translate_node(right);

	switch (node->operator_kind())
	{
	case binary_operator_kind::Equal:
		{
			auto field_name = left_exp["query"].String(); //catch exceptions
			auto right_element = right_exp["query"];
			switch (right_element.type())
			{
			case ::mongo::BSONType::String:
				{
					return BSON("query" << BSON(field_name << right_element.String()));
				}
				break;
			case ::mongo::BSONType::NumberInt:
				{
					return BSON("query" << BSON(field_name << right_element.Int()));
				}
				break;
			default:
				{
					throw new odata_service_exception(U("service does not support the right node"));
				}
			}
				
		}
		break;
	case binary_operator_kind::GreaterThan:
	case binary_operator_kind::GreaterThanOrEqual:
	case binary_operator_kind::LessThan:
	case binary_operator_kind::LessThanOrEqual:
		{
			std::string binary_operator;
			if (node->operator_kind() == binary_operator_kind::GreaterThan)
			{
				binary_operator = "$gt";
			}
			else if (node->operator_kind() == binary_operator_kind::GreaterThanOrEqual)
			{
				binary_operator = "$gte";
			}
			else if (node->operator_kind() == binary_operator_kind::LessThan)
			{
				binary_operator = "$lt";
			}
			else if (node->operator_kind() == binary_operator_kind::LessThanOrEqual)
			{
				binary_operator = "$lte";
			}
			auto field_name = left_exp["query"].String(); //catch exceptions
			auto right_element = right_exp["query"];
			switch (right_element.type())
			{
			case ::mongo::BSONType::String:
				{
					return BSON("query" << BSON(field_name << BSON(binary_operator << right_element.String())));
				}
				break;
			case ::mongo::BSONType::NumberInt:
				{
					return BSON("query" << BSON(field_name << BSON(binary_operator << right_element.Int())));
				}
				break;
			default:
				{
					throw new odata_service_exception(U("service does not support the right node"));
				}
			}
		}
		break;
	default:
		{
			throw new odata_service_exception(U("service does not support the binary operator"));
		}
		break;
	}
}
    
::mongo::BSONObj node_to_bson_visitor::visit(std::shared_ptr<odata_unary_operator_node> node)
{
    throw new odata_service_exception(U("service does not implement the node type"));
}
    
::mongo::BSONObj node_to_bson_visitor::visit(std::shared_ptr<odata_parameter_alias_node> node)
{
    throw new odata_service_exception(U("service does not implement the node type"));
}
    
::mongo::BSONObj node_to_bson_visitor::visit(std::shared_ptr<odata_property_access_node> node)
{
	if (!node)
	{
		throw new odata_service_exception(U("unexpected null node"));
	}

	auto property_name = node->property_name();
	std::string property_name_s(property_name.cbegin(), property_name.cend());
	return BSON("query" << property_name_s);
}
        
::mongo::BSONObj node_to_bson_visitor::visit(std::shared_ptr<odata_type_cast_node> node)
{
    throw new odata_service_exception(U("service does not implement the node type"));
}

::mongo::BSONObj node_to_bson_visitor::visit(std::shared_ptr<odata_lambda_node> node)
{
    throw new odata_service_exception(U("service does not implement the node type"));
}
    
::mongo::BSONObj node_to_bson_visitor::visit(std::shared_ptr<odata_range_variable_node> node)
{
    throw new odata_service_exception(U("service does not implement the node type"));
}
    
::mongo::BSONObj node_to_bson_visitor::visit(std::shared_ptr<odata_function_call_node> node)
{
    throw new odata_service_exception(U("service does not implement the node type"));
}
    
::mongo::BSONObj node_to_bson_visitor::visit_any(std::shared_ptr<odata_query_node> node)
{
	throw new odata_service_exception(U("service does not implement the node type"));
}

}}
