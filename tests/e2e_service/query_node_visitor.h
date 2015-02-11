//---------------------------------------------------------------------
// <copyright file="query_node_visitor.h" company="Microsoft">
//      Copyright (C) Microsoft Corporation. All rights reserved. See License.txt in the project root for license information.
// </copyright>
//---------------------------------------------------------------------

#pragma once

#include <memory>
#include <cstdlib>
#include <WinSock2.h>

#include <mongo/bson/bson.h>

#include "cpprest/basic_types.h"
#include "odata/core/odata_query_node_visitor.h"
#include "odata_service_exception.h"

namespace odata { namespace service
{

class node_to_bson_visitor : public ::odata::core::odata_query_node_visitor<::mongo::BSONObj>
{
public:
	::mongo::BSONObj translate_node(std::shared_ptr<::odata::core::odata_query_node> node);
	
	::mongo::BSONObj visit(std::shared_ptr<::odata::core::odata_constant_node> node);	
    
	::mongo::BSONObj visit(std::shared_ptr<::odata::core::odata_binary_operator_node> node);
    
	::mongo::BSONObj visit(std::shared_ptr<::odata::core::odata_unary_operator_node> node);
    
    ::mongo::BSONObj visit(std::shared_ptr<::odata::core::odata_parameter_alias_node> node);

    ::mongo::BSONObj visit(std::shared_ptr<::odata::core::odata_property_access_node> node);
        
    ::mongo::BSONObj visit(std::shared_ptr<::odata::core::odata_type_cast_node> node);

    ::mongo::BSONObj visit(std::shared_ptr<::odata::core::odata_lambda_node> node);
    
    ::mongo::BSONObj visit(std::shared_ptr<::odata::core::odata_range_variable_node> node);
    
    ::mongo::BSONObj visit(std::shared_ptr<::odata::core::odata_function_call_node> node);
    
	::mongo::BSONObj visit_any(std::shared_ptr<::odata::core::odata_query_node> node);
};

}}
