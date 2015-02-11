//---------------------------------------------------------------------
// <copyright file="odata_value_builder.h" company="Microsoft">
//      Copyright (C) Microsoft Corporation. All rights reserved. See License.txt in the project root for license information.
// </copyright>
//---------------------------------------------------------------------

#pragma once

#include <vector>
#include <cstdlib>
#include <WinSock2.h>

#include <mongo/bson/bson.h>
#include <mongo/client/dbclient.h>

#include "odata/core/odata_value.h"
#include "odata/core/odata_enum_value.h"
#include "odata/core/odata_entity_value.h"
#include "odata/core/odata_complex_value.h"
#include "odata/core/odata_collection_value.h"

namespace odata { namespace service
{
	
class odata_value_builder
{
public:
	odata_value_builder() {};

	std::shared_ptr<::odata::core::odata_value> build_odata_value(std::shared_ptr<::odata::edm::edm_named_type> edm_type, const std::vector<::mongo::BSONObj> &data);

	std::shared_ptr<::odata::core::odata_value> build_odata_value(std::shared_ptr<::odata::edm::edm_named_type> edm_type, ::mongo::BSONObj data);

	std::shared_ptr<::odata::core::odata_value> build_odata_value(std::shared_ptr<::odata::edm::edm_named_type> edm_type, ::mongo::BSONElement data);

	std::shared_ptr<::odata::core::odata_structured_value> build_odata_structured_value(std::shared_ptr<::odata::edm::edm_structured_type> edm_type, ::mongo::BSONObj data);

	std::shared_ptr<::odata::core::odata_collection_value> build_odata_collection_value(std::shared_ptr<::odata::edm::edm_collection_type> edm_type, ::mongo::BSONElement data);

	std::shared_ptr<::odata::core::odata_primitive_value> build_odata_primitive_value(std::shared_ptr<::odata::edm::edm_primitive_type> edm_type, ::mongo::BSONElement data);

	std::shared_ptr<::odata::core::odata_enum_value> build_odata_enum_value(std::shared_ptr<::odata::edm::edm_enum_type> edm_type, ::mongo::BSONElement data);

	::mongo::BSONObj build_BSONObj(std::shared_ptr<::odata::core::odata_value> odata_value);
	
	::mongo::BSONObj build_BSONObj(std::shared_ptr<::odata::edm::edm_named_type> edm_type, std::shared_ptr<::odata::core::odata_value> odata_value);

	::mongo::BSONObj build_BSONObj_from_structured_value(std::shared_ptr<::odata::core::odata_structured_value> odata_value);
};

}}