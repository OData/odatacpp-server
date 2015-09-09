//---------------------------------------------------------------------
// <copyright file="odata_json_reader_full.cpp" company="Microsoft">
//      Copyright (C) Microsoft Corporation. All rights reserved. See License.txt in the project root for license information.
// </copyright>
//---------------------------------------------------------------------

#include "odata/core/odata_json_reader_full.h"

using namespace ::odata::edm;
using namespace ::odata::utility;

namespace odata { namespace core
{

std::shared_ptr<odata_value> entity_json_reader_full::deserilize(
    const odata::utility::json::value& /*response*/, std::shared_ptr<edm_entity_set> /*set*/)
{
	throw std::runtime_error("full metadata reader not implemented!"); 
}

}}