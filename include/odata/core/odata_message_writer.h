//---------------------------------------------------------------------
// <copyright file="odata_message_writer.h" company="Microsoft">
//      Copyright (C) Microsoft Corporation. All rights reserved. See License.txt in the project root for license information.
// </copyright>
//---------------------------------------------------------------------

#pragma once

#include "odata/common/utility.h"
#include "odata/edm/odata_edm.h"
#include "odata/edm/edm_model_writer.h"
#include "odata/core/odata_value.h"
#include "odata/core/odata_json_writer.h"
#include "odata/core/odata_service_document.h"
#include "odata/core/odata_batch_value.h"
#include "odata/core/odata_batch_part_value.h"
#include "odata/core/odata_entity_reference.h"
#include "odata/core/odata_entity_reference_collection.h"
#include "odata/core/odata_error.h"
#include "odata/common/json.h"
#include "odata/common/uri.h"

namespace odata { namespace core
{

class odata_message_writer
{
public:
    odata_message_writer(std::shared_ptr<::odata::edm::edm_model> model, ::odata::utility::uri service_root) 
		: m_model(model), m_service_root(service_root)
	{
	}

	ODATACPP_API ::odata::utility::string_t write_service_document(std::shared_ptr<odata_service_document> service_document);

	ODATACPP_API ::odata::utility::string_t write_metadata_document();

    ODATACPP_API ::odata::utility::string_t write_odata_value(std::shared_ptr<::odata::core::odata_value> value);

    ODATACPP_API ::odata::utility::string_t write_asynchronous_odata_value(std::shared_ptr<::odata::core::odata_value> value, int16_t status_code, ::odata::utility::string_t status_message, std::unordered_map<::odata::utility::string_t, ::odata::utility::string_t> headers);

    ODATACPP_API ::odata::utility::string_t write_odata_batch_value(std::shared_ptr<::odata::core::odata_batch_value> batch_value);

    ODATACPP_API ::odata::utility::string_t write_odata_error(std::shared_ptr<::odata::core::odata_error> error);

    ODATACPP_API ::odata::utility::string_t write_odata_entity_reference(std::shared_ptr<::odata::core::odata_entity_reference> entity_reference);

    ODATACPP_API ::odata::utility::string_t write_odata_entity_reference_collection(std::shared_ptr<::odata::core::odata_entity_reference_collection> entity_reference_collection);

private:
	std::shared_ptr<::odata::edm::edm_model> m_model;
	::odata::utility::uri m_service_root;
};

}}